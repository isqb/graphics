//Skybox vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec4 a_position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform samplerCube u_cubemap;


out vec3 TexCoords;

void main()
{
    gl_Position = u_projection * u_view * a_position;
    //gl_Position = a_position;// vec4(a_position, 1.0);
	//TexCoords  = a_position.xyz;
	TexCoords  =  a_position; // maybe not '.xyz'
}