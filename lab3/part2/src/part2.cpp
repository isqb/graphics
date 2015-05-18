// Assignment 3, Part 2
//
// Modify this file and the environment_mapping.vert and
// environment_mapping.frag shaders to implement environment mapping.
//

#include "GLSLProgram.h"
#include "OBJFileReader.h"
#include "Trackball.h"
#include "AntTweakBar.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "lodepng.h"

#include <iostream>
#include <cstdlib>
#include <algorithm>

// The attribute locations we will use in the vertex shader
enum AttributeLocation {
    POSITION = 0,
    NORMAL = 1
};

// Struct for representing an 8-bit bitmap image
struct Image_t {
  int width;
  int height;
  std::vector<unsigned char> data;
};

// Struct for representing an indexed triangle mesh
struct Mesh {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;
};

// Struct for represents a vertex array object (VAO) created from a
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
	cgtk::GLSLProgram program_sky_box;
    cgtk::Trackball trackball;
    Mesh mesh;
	
    MeshVAO meshVAO;
	GLuint skyboxVAO;
	GLuint skyboxVBO;
    GLuint cubemap_0;
	GLuint cubemap_1;
	GLuint cubemap_2;
	GLuint cubemap_3;
	GLuint cubemap_4;
	GLuint cubemap_5;
	GLuint cubemap_6;
	GLuint cubemap_7;
	GLuint cubemap_sky_box;
	glm::vec3 light_position;
	glm::vec3 light_color;
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
	glm::vec3 specular_color; 
	glm::vec3 bg_color;
	std::vector<std::string> textures;

	int ambient_switch;
	int diffuse_switch;
	int specular_switch;
	int normals_switch;
	int gamma_swtich;
	int invert_switch;
	int ortho_switch;
	int newText;
	int oldText;

	float cam_near;
	float cam_far;

	float zoomFactor; /* Global, if you want. Modified by user input. Initially 1.0 */

    Globals()
    {
        width = 800;
        height = 600;
		newText = 8;
		oldText = 0;
		ambient_switch = 0;
		diffuse_switch = 0;
		specular_switch = 0;
		gamma_swtich = 0;
		normals_switch = 0;
		invert_switch = 0;
		ortho_switch = 0;
		zoomFactor = 1.0f;
		light_position = glm::vec3(0.0f, 5.0f, -5.0f);
		light_color = glm::vec3(1.0f, 1.0f, 1.0f);
		ambient_color = glm::vec3(0.002f, 0.002f, 0.002f);
		diffuse_color = glm::vec3(0.0f, 1.0f, 0.0f);
		specular_color = glm::vec3(1.0f, 1.0f, 1.0f);
		bg_color = glm::vec3(0.0, 0.0, 0.0);
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
    return rootDir + "/part2/src/shaders/";
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

// Returns the absolute path to the directory that contains the cube
// map sets
std::string cubemapDir(void)
{
    std::string rootDir = getEnvVar("ASSIGNMENT3_ROOT");
    if (rootDir.empty()) {
        std::cout << "Error: ASSIGNMENT3_ROOT is not set." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return rootDir + "/cubemaps/";
}

Image_t loadPNG(const std::string &filename)
{
    std::cout << "Loading image from " << filename << " ..." << std::endl;  

    std::vector<unsigned char> data;
    unsigned width, height;
    unsigned error = lodepng::decode(data, width, height, filename);
    if (error != 0) {
        std::cout << "Error: " << lodepng_error_text(error) << std::endl;
        std::exit(EXIT_FAILURE);
    }
  
    Image_t image;
    image.width = width;
    image.height = height;
    image.data = data;
  
    std::cout << "Done!" << std::endl;
    std::cout << "Image width: " << image.width << std::endl;
    std::cout << "Image height: " << image.height << std::endl;
    std::cout << "Number of elements: " << image.data.size() << std::endl;
  
    return image;
}

GLuint loadCubeMap(const std::string &dirname)
{  
    GLuint cubemap;
    glGenTextures(1, &cubemap);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
    Image_t image_x_positive = loadPNG(std::string(dirname + "/posx.png"));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,                  // target, level of detail
                 GL_RGBA8,                                           // internal format
                 image_x_positive.width, image_x_positive.height, 0, // width, height, border
                 GL_RGBA, GL_UNSIGNED_BYTE,                          // external format, type
                 &(image_x_positive.data[0]));                       // pixels
  
    Image_t image_x_negative = loadPNG(std::string(dirname + "/negx.png"));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0,                  // target, level of detail
                 GL_RGBA8,                                           // internal format
                 image_x_negative.width, image_x_negative.height, 0, // width, height, border
                 GL_RGBA, GL_UNSIGNED_BYTE,                          // external format, type
                 &(image_x_negative.data[0]));                       // pixels
  
    Image_t image_y_positive = loadPNG(std::string(dirname + "/posy.png"));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0,                  // target, level of detail
                 GL_RGBA8,                                           // internal format
                 image_y_positive.width, image_y_positive.height, 0, // width, height, border
                 GL_RGBA, GL_UNSIGNED_BYTE,                          // external format, type
                 &(image_y_positive.data[0]));                       // pixels
  
    Image_t image_y_negative = loadPNG(std::string(dirname + "/negy.png"));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0,                  // target, level of detail
                 GL_RGBA8,                                           // internal format
                 image_y_negative.width, image_y_negative.height, 0, // width, height, border
                 GL_RGBA, GL_UNSIGNED_BYTE,                          // external format, type
                 &(image_y_negative.data[0]));                       // pixels
  
    Image_t image_z_positive = loadPNG(std::string(dirname + "/posz.png"));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0,                  // target, level of detail
                 GL_RGBA8,                                           // internal format
                 image_z_positive.width, image_z_positive.height, 0, // width, height, border
                 GL_RGBA, GL_UNSIGNED_BYTE,                          // external format, type
                 &(image_z_positive.data[0]));                       // pixels
  
    Image_t image_z_negative = loadPNG(std::string(dirname + "/negz.png"));
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0,                  // target, level of detail
                 GL_RGBA8,                                           // internal format
                 image_z_negative.width, image_z_negative.height, 0, // width, height, border
                 GL_RGBA, GL_UNSIGNED_BYTE,                          // external format, type
                 &(image_z_negative.data[0]));                       // pixels
  
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return cubemap;
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

void createBoxVAO(){


#pragma region create_skybox
	// Create Skybox
	float size = 1.0f;
	
	GLfloat points[] = {
		-size, size, -size,
		-size, -size, -size,
		size, -size, -size,
		size, -size, -size,
		size, size, -size,
		-size, size, -size,

		-size, -size, size,
		-size, -size, -size,
		-size, size, -size,
		-size, size, -size,
		-size, size, size,
		-size, -size, size,

		size, -size, -size,
		size, -size, size,
		size, size, size,
		size, size, size,
		size, size, -size,
		size, -size, -size,

		-size, -size, size,
		-size, size, size,
		size, size, size,
		size, size, size,
		size, -size, size,
		-size, -size, size,

		-size, size, -size,
		size, size, -size,
		size, size, size,
		size, size, size,
		-size, size, size,
		-size, size, -size,

		-size, -size, -size,
		-size, -size, size,
		size, -size, -size,
		size, -size, -size,
		-size, -size, size,
		size, -size, size
	};
#pragma endregion


	glGenBuffers(1, &globals.skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, globals.skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	
	glGenVertexArrays(1, &globals.skyboxVAO);
	glBindVertexArray(globals.skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, globals.skyboxVAO);
	glEnableVertexAttribArray(POSITION);
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(0);

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
	// Yeah.. so.. we need an global array with all the avaliable textures to be used with keystrokes for texture selection... 
	std::string texture_array[] = {"0.125", "0.5", "2", "8", "32", "128", "512", "2048"};
	globals.textures = std::vector<std::string>(texture_array,end(texture_array));

	globals.cubemap_0 = loadCubeMap(cubemapDir() + "RomeChurch/prefiltered/"+globals.textures[0]);
	globals.cubemap_1 = loadCubeMap(cubemapDir() + "RomeChurch/prefiltered/"+globals.textures[1]);
	globals.cubemap_2 = loadCubeMap(cubemapDir() + "RomeChurch/prefiltered/"+globals.textures[2]);
	globals.cubemap_3 = loadCubeMap(cubemapDir() + "RomeChurch/prefiltered/"+globals.textures[3]);
	globals.cubemap_4 = loadCubeMap(cubemapDir() + "RomeChurch/prefiltered/"+globals.textures[4]);
	globals.cubemap_5 = loadCubeMap(cubemapDir() + "RomeChurch/prefiltered/"+globals.textures[5]);
	globals.cubemap_6 = loadCubeMap(cubemapDir() + "RomeChurch/prefiltered/"+globals.textures[6]);
    globals.cubemap_7 = loadCubeMap(cubemapDir() + "RomeChurch/prefiltered/"+globals.textures[7]);
	globals.cubemap_sky_box = loadCubeMap(cubemapDir() + "RomeChurch");
	std::string dir = shaderDir();

    loadProgram(shaderDir() + "environment_mapping.vert",
                shaderDir() + "environment_mapping.frag",
                &globals.program);
	
	loadProgram(shaderDir() + "skybox.vert",
				shaderDir() + "skybox.frag",
				&globals.program_sky_box);
		
	

    loadMesh((modelDir() + "gargo.obj"), &globals.mesh);
    createMeshVAO(globals.mesh, &globals.meshVAO);
	createBoxVAO();

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
    // matrices here and pass them (along with the model matrix) as
    // uniform variables to the shader program

	view = glm::lookAt(glm::vec3(0.0f,0.01f,5.0f),glm::vec3(0),glm::vec3(0.0f,1.0f,0.0f));

	if(!globals.ortho_switch) {
		projection = glm::perspective(globals.zoomFactor*45.0f, 1.0f, 0.1f, 100.0f);
	}
	else {
		projection = glm::ortho(globals.zoomFactor*-5.0, globals.zoomFactor*5.0, globals.zoomFactor*-5.0, globals.zoomFactor*5.0, 0.1, 100.0); 
	}

	glm::mat4 trackMatrix = globals.trackball.getRotationMatrix();

	model = trackMatrix;

	glm::mat4 MVPmatrix = projection * view * model;
	glm::mat4 u_mv = view * model;
	globals.program.setUniformMatrix4f("MVPmatrix", glm::mat4(MVPmatrix));
	globals.program.setUniformMatrix4f("u_mv", u_mv);

	globals.program.setUniform3f("light_position",globals.light_position);
	globals.program.setUniform3f("light_color",globals.light_color);
	globals.program.setUniform3f("ambient_color",globals.ambient_color);
	globals.program.setUniform3f("diffuse_color",globals.diffuse_color);
	globals.program.setUniform3f("specular_color",globals.specular_color);
	globals.program.setUniform1f("specular_power",420.0);

	globals.program.setUniform1i("ambient_switch",globals.ambient_switch);
	globals.program.setUniform1i("diffuse_switch",globals.diffuse_switch);
	globals.program.setUniform1i("specular_switch",globals.specular_switch);
	globals.program.setUniform1i("gamma_swtich",globals.gamma_swtich);
	globals.program.setUniform1i("invert_switch",globals.invert_switch);
	globals.program.setUniform1i("normals_switch",globals.normals_switch);
    
	

    // Select the active texture unit and bind the cubemap texture
	
    // Pass the number of the active texture unit as a uniform int
    // variable to the shader program

	// bind mesh and draw
	if (globals.newText != globals.oldText){
		switch (globals.newText){
			
		case 8:
			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_CUBE_MAP, globals.cubemap_7);
			globals.program.setUniform1i("u_cubemap", globals.cubemap_7);
			break;

		case 7:
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_CUBE_MAP, globals.cubemap_6);
			globals.program.setUniform1i("u_cubemap", globals.cubemap_6);
			break;

		case 6:
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_CUBE_MAP, globals.cubemap_5);
			globals.program.setUniform1i("u_cubemap", globals.cubemap_5);
			break;

		case 5:
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_CUBE_MAP, globals.cubemap_4);
			globals.program.setUniform1i("u_cubemap", globals.cubemap_4);
			break;

		case 4:
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_CUBE_MAP, globals.cubemap_3);
			globals.program.setUniform1i("u_cubemap", globals.cubemap_3);
			break;

		case 3:
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_CUBE_MAP, globals.cubemap_2);
			globals.program.setUniform1i("u_cubemap", globals.cubemap_2);
			break;
		case 2:
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, globals.cubemap_1);
			globals.program.setUniform1i("u_cubemap", globals.cubemap_1);
			break;
			/* Using TEXTURE1 for sky box
		case 1:
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, globals.cubemap_0);
			globals.program.setUniform1i("u_cubemap", globals.cubemap_0);
			break;*/

		default:
			break;
		}
		globals.oldText = globals.newText;
	}
	glBindVertexArray(meshVAO.vao);
	glDrawElements(GL_TRIANGLES, meshVAO.numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

    
	// Background Colour
	//glClearColor(globals.bg_color[0],globals.bg_color[1],globals.bg_color[2], 1.0);
	

    
	program.disable();
}

void drawSkyBox(cgtk::GLSLProgram &program){

	glDepthMask(GL_FALSE);
	
	// Bind Skybox and Draw
	program.enable(); // glUseProgram()

	// Pass uniforms ///////////// 
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = projection = glm::perspective(globals.zoomFactor*45.0f, 
														1.0f, 0.1f, 100.0f);

		view = glm::lookAt(glm::vec3(0.0f, 0.01f, 5.0f), glm::vec3(0),
											glm::vec3(0.0f, 1.0f, 0.0f));

		program.setUniformMatrix4f("u_projection", glm::mat4(projection));
		program.setUniformMatrix4f("u_view", glm::mat4(view));
		program.setUniform1i("u_cubemap", globals.cubemap_sky_box);
		
	//////////////////////////////

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, globals.cubemap_sky_box);

	glBindVertexArray(globals.skyboxVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // not sure about 36
	glBindVertexArray(0);

	
	program.disable();
	glDepthMask(GL_TRUE);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
	drawSkyBox(globals.program_sky_box);
    //drawMesh(globals.program, globals.meshVAO);
	TwDraw();  // draw the tweak bar(s)
    glutSwapBuffers();
}

void idle(void)
{
    glutPostRedisplay();
}

// MODIFY THIS FUNCTION
void keyboard(unsigned char key, int x, int y)
{
	std::cout << key << std::endl;
	std::cout <<globals.newText << std::endl;
	if(!TwEventKeyboardGLUT(key,x,y)) {

	}
    // Define your keyboard shortcuts here
	if(key == '1') {
		globals.newText = 1;
	}
	if(key == '2') {
		globals.newText = 2;
	}
	if(key == '3') {
		globals.newText = 3;
	}
	if(key == '4') {
		globals.newText = 4;
	}
	if(key == '5') {
		globals.newText = 5;
	}
	if(key == '6') {
		globals.newText = 6;
	}
	if(key == '7') {
		globals.newText = 7;
	}
	if(key == '8') {
		globals.newText = 8;
	}
}

void mouseButtonPressed(int button, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        globals.trackball.setCenter(glm::vec2(x, y));
        globals.trackball.startTracking(glm::vec2(x, y));
    }
	if(button == 3) {
		globals.zoomFactor-=0.1f;
	}
	if(button ==4) {
		globals.zoomFactor+=0.1f;
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
	if(!TwEventMouseButtonGLUT(button,state,x,y) ) {

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
	if(!TwEventMouseMotionGLUT(x, y)) {

	}
}

void reshape(int width, int height)
{
    globals.width = width;
    globals.height = height;
    globals.trackball.setRadius(double(std::min(width, height)) / 2.0);
    globals.trackball.setCenter(glm::vec2(width, height) / 2.0f);
    glViewport(0, 0, width, height);
	TwWindowSize(width,height);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(globals.width, globals.height);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Environment mapping");
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        std::cerr << "Error: " << glewGetErrorString(status) << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    init();
	TwInit(TW_OPENGL_CORE, NULL);
	TwBar *myBar;
	myBar = TwNewBar("AwesomeTweak2k");
	TwAddVarRW(myBar, "AmbientColor", TW_TYPE_COLOR3F, &globals.ambient_color[0], "colormode=rgb group='ambient'");
	TwAddVarRW(myBar, "AmbientColor_switch", TW_TYPE_BOOL32, &globals.ambient_switch, "label= 'Ambient on/off' group='ambient'");
	TwAddVarRW(myBar, "DiffuseColor", TW_TYPE_COLOR3F, &globals.diffuse_color[0], "colormode=rgb group='diffuse'");
	TwAddVarRW(myBar, "DiffuseColor_switch", TW_TYPE_BOOL32, &globals.diffuse_switch,"label= 'Diffuse on/off' group='diffuse'");
	TwAddVarRW(myBar, "SpecularColor", TW_TYPE_COLOR3F, &globals.specular_color[0], "colormode=rgb group='specular'");
	TwAddVarRW(myBar, "SpecularColor_switch", TW_TYPE_BOOL32, &globals.specular_switch,"label= 'Specular on/off' group='specular'");
	TwAddVarRW(myBar, "LightColor", TW_TYPE_COLOR3F, &globals.light_color[0], "colormode=rgb group='light'");
	TwAddVarRW(myBar, "Pos x", TW_TYPE_FLOAT, &globals.light_position[0], "group='light'");
	TwAddVarRW(myBar, "Pos y", TW_TYPE_FLOAT, &globals.light_position[1], "group='light'");
	TwAddVarRW(myBar, "Pos z", TW_TYPE_FLOAT, &globals.light_position[2], "group='light'");
	TwAddVarRW(myBar, "BgColor", TW_TYPE_COLOR3F, &globals.bg_color, "colormode=rgb");
	TwAddVarRW(myBar, "GammaCorrection_switch", TW_TYPE_BOOL32, &globals.gamma_swtich,"label= 'Gamma on/off'");
	TwAddVarRW(myBar, "InvertNormals_switch", TW_TYPE_BOOL32, &globals.invert_switch,"label= 'Invert on/off'");
	TwAddVarRW(myBar, "Ortho toggle", TW_TYPE_BOOL32, &globals.ortho_switch,"");
    glutDisplayFunc(&display);
    glutIdleFunc(&idle);
    glutKeyboardFunc(&keyboard);
    glutMouseFunc(&mouse);
    glutMotionFunc(&motion);
    glutReshapeFunc(&reshape);
    glutMainLoop();

    return EXIT_SUCCESS;
}
