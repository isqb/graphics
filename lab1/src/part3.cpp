// Assignment 1, Part 3
//
// You don't have to modify this file.
//

#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "GLSLProgram.h"
#include "GLSLSourceFileReader.h"

// Struct for globals resources.
struct Globals {
    int width;
    int height;
    int windowID;
    cgtk::GLSLProgram program;
    GLuint triangleVAO;
    GLuint positionVBO;
};

Globals globals;

// Retrieves a string containing the value of the environment variable
// whose name is specified by the argument.
std::string getEnvVar(std::string const& name)
{
    char const* value = getenv(name.c_str());
    if (value == NULL) {
        return std::string();
    }
    else {
        return std::string(value);
    }
}

// Provides the absolute path to the shader directory.
std::string shaderDir(void)
{
    std::string rootDir = getEnvVar("ASSIGNMENT1_ROOT");
    if (rootDir.empty()) {
        std::cout << "Error: ASSIGNMENT1_ROOT is not set." << std::endl;
        exit(EXIT_FAILURE);
    }
    return rootDir + "/part3/src/shaders/";
}

// Initializes GLEW, which will load all available OpenGL extensions.
void initGLEW(void)
{
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        std::cerr << "Error: " << glewGetErrorString(status) << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Shows the supported OpenGL version.
void displayOpenGLInfo(void)
{
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}

// Creates the GLSL shader program that we will use to render the
// triangle.
void createTriangleShader(void)
{
    cgtk::GLSLSourceFileReader glslReader;
    glslReader.read(std::string(shaderDir() + "triangle.vert").c_str());
    std::string vertexShaderSource = glslReader.getSourceString();
    glslReader.read(std::string(shaderDir() + "triangle.frag").c_str());
    std::string fragmentShaderSource = glslReader.getSourceString();

    globals.program.create(vertexShaderSource, fragmentShaderSource);
    if (!globals.program.isProgram() || !globals.program.isValid()) {
        std::cerr << "Error: Could not create program." << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Generates the vertex data for the triangle and uploads it to the
// GPU memory.
void createTriangle(void)
{
    // Creates a vertex array object (VAO) for the triangle
    glGenVertexArrays(1, &globals.triangleVAO);
    glBindVertexArray(globals.triangleVAO);

    // Generates the three vertices defining the triangle and puts them
    // in a vertex buffer object (VBO)
    const GLfloat vertices[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f,-0.5f, 0.0f,
        0.5f,-0.5f, 0.0f,
    };
    glGenBuffers(1, &globals.positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, globals.positionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(globals.program.getAttribLocation("a_position"));
    glVertexAttribPointer(globals.program.getAttribLocation("a_position"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind the VBO

    glBindVertexArray(0); // unbind the VAO
}

// Draws the triangle.
void drawTriangle(void)
{
    globals.program.enable();

    double elapsed_time = double(glutGet(GLUT_ELAPSED_TIME)) / 1000.0;
    globals.program.setUniform1f("u_time", float(elapsed_time));

    glBindVertexArray(globals.triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    globals.program.disable();
}

// Initializes the rendering.
void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    createTriangleShader();
    createTriangle();
}

// The display callback for the current window. Clears the screen with
// the color specified with glClearColor() and then calls the triangle
// drawing function to render the triangle.
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawTriangle();
    glutSwapBuffers();
}

// The reshape callback for the current window. Updates the viewport
// when the window is resized.
void reshape(int width, int height)
{
    globals.width = width;
    globals.height = height;
    glViewport(0, 0, globals.width, globals.height);
}

// Idle callback.
void idle(void)
{
    glutPostRedisplay();
}

// Main entry point.
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    globals.width = 500;
    globals.height = 500;
    glutInitWindowSize(globals.width, globals.height);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Triangle");
    initGLEW();
    displayOpenGLInfo();
    init();
    glutReshapeFunc(&reshape);
    glutDisplayFunc(&display);
    glutIdleFunc(&idle);
    glutMainLoop();

    return EXIT_SUCCESS;
}
