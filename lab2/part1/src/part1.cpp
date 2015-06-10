// Assignment 2, Part 1
//
// Modify this file and the rgb_cube.vert and rgb_cube.frag shaders to
// implement the spinning RGB cube.
//

#include "GLSLProgram.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cstdlib>

// The attribute locations we will use in the vertex shader
enum AttributeLocation {
    POSITION = 0
};

// Struct for global resources
struct Globals {
    int width;
    int height;
    cgtk::GLSLProgram program;
    GLuint cubeVAO;
    GLuint cubeVBO;

    Globals()
    {
        width = 500;
        height = 500;
    }
};

Globals globals;

// Returns the value of the environment variable whose name is
// specified by the argument
std::string getEnvVar(const std::string &name)
{
    char const* value = std::getenv(name.c_str());
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
    std::string rootDir = getEnvVar("ASSIGNMENT2_ROOT");
    if (rootDir.empty()) {
        std::cout << "Error: ASSIGNMENT2_ROOT is not set." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return rootDir + "/part1/src/shaders/";
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

// MODIFY THIS FUNCTION
void createCube(void)
{
    // MODIFY THIS PART: Define the six faces (front, back, left,
    // right, top, and bottom) of the cube. Each face should be
    // constructed from two triangles, and each triangle should be
    // constructed from three vertices. That is, you should define 36
    // vertices that together make up 12 triangles. One triangle is
    // given; you have to define the rest!
    const GLfloat vertices[] = {
        // front face
        -0.5f, -0.5f,  0.5f, // first triangle starts here
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, // first triangle ends here

		 -0.5f, -0.5f,  0.5f, // second triangle starts here
		 -0.5f,  0.5f,  0.5f,
		  0.5f,  0.5f,  0.5f, // second triangle ends here

        // back face
		-0.5f, -0.5f,  -0.5f, // first triangle starts here
         0.5f, -0.5f,  -0.5f,
         0.5f,  0.5f,  -0.5f, // first triangle ends here

		 -0.5f, -0.5f,  -0.5f, // second triangle starts here
		 -0.5f,  0.5f,  -0.5f,
		  0.5f,  0.5f,  -0.5f, // second triangle ends here

        // left face
		-0.5f, -0.5f,  0.5f, //first triangle start
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f, //first triangle end

		-0.5f, -0.5f,  0.5f, //second triangle start
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f, //sencond triangle end

        // right face
		0.5f, -0.5f,  0.5f, //first triangle start
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f, //first triangle end

		0.5f, -0.5f,  0.5f, //second triangle start
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f, //sencond triangle end

        // top face
		-0.5f, 0.5f,  0.5f, //first triangle start
		-0.5f, 0.5f, -0.5f,
		 0.5f, 0.5f,  0.5f, //first triangle end

		 0.5f, 0.5f, -0.5f, //second triangle start
		-0.5f, 0.5f, -0.5f,
		 0.5f, 0.5f,  0.5f, //second triangle end

        // bottom face

		-0.5f, -0.5f,  0.5f, //first triangle start
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f, //first triangle end

		 0.5f, -0.5f, -0.5f, //second triangle start
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f, //second triangle end

    };

    // Generates and populates a vertex buffer object (VBO) for the
    // vertices (DO NOT CHANGE THIS)
    glGenBuffers(1, &globals.cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, globals.cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Creates a vertex array object (VAO) for drawing the cube
    // (DO NOT CHANGE THIS)
    glGenVertexArrays(1, &globals.cubeVAO);
    glBindVertexArray(globals.cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, globals.cubeVBO);
    glEnableVertexAttribArray(POSITION);
    glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindVertexArray(0); // unbinds the VAO
}

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    loadProgram(shaderDir() + "rgb_cube.vert",
                shaderDir() + "rgb_cube.frag",
                &globals.program);

    createCube();
}

// MODIFY THIS FUNCTION
void drawCube()
{
    globals.program.enable();

    double elapsed_time = double(glutGet(GLUT_ELAPSED_TIME)) / 1000.0;

    // Define the model, view, and projection matrices here
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Concatenate the model, view, and projection matrices to a
    // ModelViewProjection (MVP) matrix and pass it as a uniform
    // variable to the shader program
	glm::vec3 axis = glm::vec3(0.5f , 1.0f , 0.0f);
	model = glm::rotate(model, (float)elapsed_time*45.0f, axis);

	//view = glm::lookAt(glm::vec3(1.0f,1.0f,1.0f), glm::vec3(1.0f,1.0f,1.0f),glm::vec3(0.0f,0.0f,1.0f));

	//view = glm::rotate(view, 45.0f, glm::vec3(0.5f,0.5f,0.0f));

	glm::mat4 MVPmatrix = projection * view * model;
	globals.program.setUniformMatrix4f("MVPmatrix", glm::mat4(MVPmatrix));

    glBindVertexArray(globals.cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36); //Third argument defines number of verticies to draw
    glBindVertexArray(0);

    globals.program.disable();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST); // ensures that polygons overlap correctly
    drawCube();

    glutSwapBuffers();
}

void idle(void)
{
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    globals.width = width;
    globals.height = height;
    glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(globals.width, globals.height);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("RGB cube");
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
    glutReshapeFunc(&reshape);
    glutMainLoop();

    return EXIT_SUCCESS;
}
