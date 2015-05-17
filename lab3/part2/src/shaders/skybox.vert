//Skybox vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec4 a_position;

uniform mat4 MVPmatrix;

out vec3 position;

void main()
{
    //Position = a_position.xyz;
	position =  a_position; // I think maybe should be u_mv not MVP
    gl_Position = MVPmatrix * a_position;
}