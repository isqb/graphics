// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec4 a_position;

uniform mat4 MVPmatrix;

out vec3 v_color;

void main() {
    v_color = a_position.xyz + 0.5; // converts vertex position to RGB color
    gl_Position = MVPmatrix * a_position;
}
