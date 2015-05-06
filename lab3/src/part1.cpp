// Assignment 3, Part 1
//
// Modify this file and the mesh.vert and mesh.frag shaders to
// implement per-fragment Blinn-Phong shading.
//

#include "GLSLProgram.h"
#include "OBJFileReader.h"
#include "Trackball.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdlib>
#include <algorithm>

// The attribute locations we will use in the vertex shader
enum AttributeLocation {
    POSITION = 0,
    NORMAL = 1
};

// Struct for representing an indexed triangle mesh
struct Mesh {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;
};

// Struct for representing a vertex array object (VAO) created from a
// mesh. Used for rendering.
struct MeshVAO {
    GLuint vao;
    GLuint vertexVBO;
    GLuint normalVBO;
    GLuint indexVBO;
    int numVertices;
    int numIndices;
};

// Struct for global resources
struct Globals {
    int width;
    int height;
    cgtk::GLSLProgram program;
    cgtk::Trackball trackball;
    Mesh mesh;
    MeshVAO meshVAO;

    Globals()
    {
        width = 800;
        height = 600;
    }
};

Globals globals;

// Returns the value of the environment variable whose name is
// specified by the argument
std::string getEnvVar(const std::string &name)
{
    char *value = std::getenv(name.c_str());
    if (value == NULL) {
        return std::string();
    }
    else {
        return std::string(value);
    }
}

// Returns the absolute path to the shader directory
std::string shaderDir(void)
{
    std::string rootDir = getEnvVar("ASSIGNMENT3_ROOT");
    if (rootDir.empty()) {
        std::cout << "Error: ASSIGNMENT3_ROOT is not set." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return rootDir + "/part1/src/shaders/";
}

// Returns the absolute path to the 3D model directory
std::string modelDir(void)
{
    std::string rootDir = getEnvVar("ASSIGNMENT3_ROOT");
    if (rootDir.empty()) {
        std::cout << "Error: ASSIGNMENT3_ROOT is not set." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return rootDir + "/3d_models/";
}

void loadProgram(const std::string &vertexShaderFilename,
                 const std::string &fragmentShaderFilename,
                 cgtk::GLSLProgram *program)
{
    std::string vertexShaderSource = cgtk::readGLSLSource(vertexShaderFilename);
    std::string fragmentShaderSource = cgtk::readGLSLSource(fragmentShaderFilename);

    program->setShaderSource(GL_VERTEX_SHADER, vertexShaderSource);
    program->setShaderSource(GL_FRAGMENT_SHADER, fragmentShaderSource);
    program->update();
    if (!program->isValid()) {
        std::cerr << "Error: Could not create program." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void loadMesh(const std::string &filename, Mesh *mesh)
{
    cgtk::OBJFileReader reader;
    reader.load(filename.c_str());
    mesh->vertices = reader.getVertices();
    mesh->normals = reader.getNormals();
    mesh->indices = reader.getIndices();
}

void createMeshVAO(const Mesh &mesh, MeshVAO *meshVAO)
{
    // Generates and populates a VBO for the vertices
    glGenBuffers(1, &(meshVAO->vertexVBO));
    glBindBuffer(GL_ARRAY_BUFFER, meshVAO->vertexVBO);
    auto verticesNBytes = mesh.vertices.size() * sizeof(mesh.vertices[0]);
    glBufferData(GL_ARRAY_BUFFER, verticesNBytes, mesh.vertices.data(), GL_STATIC_DRAW);

    // Generates and populates a VBO for the vertex normals
    glGenBuffers(1, &(meshVAO->normalVBO));
    glBindBuffer(GL_ARRAY_BUFFER, meshVAO->normalVBO);
    auto normalsNBytes = mesh.normals.size() * sizeof(mesh.normals[0]);
    glBufferData(GL_ARRAY_BUFFER, normalsNBytes, mesh.normals.data(), GL_STATIC_DRAW);

    // Generates and populates a VBO for the element indices
    glGenBuffers(1, &(meshVAO->indexVBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshVAO->indexVBO);
    auto indicesNBytes = mesh.indices.size() * sizeof(mesh.indices[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesNBytes, mesh.indices.data(), GL_STATIC_DRAW);

    // Creates a vertex array object (VAO) for drawing the mesh
    glGenVertexArrays(1, &(meshVAO->vao));
    glBindVertexArray(meshVAO->vao);
    glBindBuffer(GL_ARRAY_BUFFER, meshVAO->vertexVBO);
    glEnableVertexAttribArray(POSITION);
    glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, meshVAO->normalVBO);
    glEnableVertexAttribArray(NORMAL);
    glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshVAO->indexVBO);
    glBindVertexArray(0); // unbinds the VAO

    // Additional information required by draw calls
    meshVAO->numVertices = mesh.vertices.size();
    meshVAO->numIndices = mesh.indices.size();
}

void initializeTrackball(void)
{
    double radius = double(std::min(globals.width, globals.height)) / 2.0;
    globals.trackball.setRadius(radius);
    glm::vec2 center = glm::vec2(globals.width, globals.height) / 2.0f;
    globals.trackball.setCenter(center);
}

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    loadProgram(shaderDir() + "mesh.vert",
                shaderDir() + "mesh.frag",
                &globals.program);

    loadMesh((modelDir() + "teapot.obj"), &globals.mesh);
    createMeshVAO(globals.mesh, &globals.meshVAO);

    initializeTrackball();
}

// MODIFY THIS FUNCTION
void drawMesh(cgtk::GLSLProgram &program, const MeshVAO &meshVAO)
{
    program.enable();
    
    // Define the model, view, and projection matrices here
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Construct the ModelViewProjection, ModelView, and normal
    // matrices here and pass them as uniform variables to the shader
    // program
	view = glm::lookAt(glm::vec3(0.0f,0.01f,5.0f),glm::vec3(0),glm::vec3(0.0f,1.0f,0.0f));

	projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);

	glm::mat4 trackMatrix = globals.trackball.getRotationMatrix();

	model = trackMatrix;

	glm::mat4 MVPmatrix = projection * view * model;
	glm::mat4 u_mv = view * model;
	globals.program.setUniformMatrix4f("MVPmatrix", glm::mat4(MVPmatrix));
	globals.program.setUniformMatrix4f("u_mv", u_mv);
    

    // Set up the light source and material properties and pass them
    // as uniform variables to the shader program, along with the
    // flags (uniform int variables) used for toggling on/off
    // different parts of the rendering
	glm::vec3 light_position = glm::vec3(0.0, 5.0, 10.0);
	glm::vec3 light_color = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 ambient_color = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 diffuse_color = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 specular_color = glm::vec3(1.0, 1.0, 1.0);
	

	globals.program.setUniform3f("light_position",light_position);
	globals.program.setUniform3f("light_color",light_color);
	globals.program.setUniform3f("ambient_color",ambient_color);
	globals.program.setUniform3f("diffuse_color",diffuse_color);
	globals.program.setUniform3f("specular_color",specular_color);
	globals.program.setUniform1f("specular_power",60.0);


    glBindVertexArray(meshVAO.vao);
    glDrawElements(GL_TRIANGLES, meshVAO.numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    program.disable();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    drawMesh(globals.program, globals.meshVAO);

    glutSwapBuffers();
}

void idle(void)
{
    glutPostRedisplay();
}

// MODIFY THIS FUNCTION
void keyboard(unsigned char key, int x, int y)
{
    // Define your keyboard shortcuts here
}

void mouseButtonPressed(int button, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        globals.trackball.setCenter(glm::vec2(x, y));
        globals.trackball.startTracking(glm::vec2(x, y));
    }
}

void mouseButtonReleased(int button, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        globals.trackball.stopTracking();
    }
}

void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        mouseButtonPressed(button, x, y);
    }
    else {
        mouseButtonReleased(button, x, y);
    }
}

void moveTrackball(int x, int y)
{
    if (globals.trackball.tracking()) {
        globals.trackball.move(glm::vec2(x, y));
    }
}

void motion(int x, int y)
{
    moveTrackball(x, y);
}

void reshape(int width, int height)
{
    globals.width = width;
    globals.height = height;
    globals.trackball.setRadius(double(std::min(width, height)) / 2.0);
    globals.trackball.setCenter(glm::vec2(width, height) / 2.0f);
    glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(globals.width, globals.height);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Blinn-Phong shading");
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        std::cerr << "Error: " << glewGetErrorString(status) << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    init();
    glutDisplayFunc(&display);
    glutIdleFunc(&idle);
    glutKeyboardFunc(&keyboard);
    glutMouseFunc(&mouse);
    glutMotionFunc(&motion);
    glutReshapeFunc(&reshape);
    glutMainLoop();

    return EXIT_SUCCESS;
}
