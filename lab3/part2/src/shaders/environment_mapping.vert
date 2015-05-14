// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 MVPmatrix;
uniform mat4 u_mv; // ModelView matrix

//Uniforms needed in fragment shader as well
uniform vec3 light_position; // The position of your light source
uniform vec3 light_color; //Color of light
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float specular_power;
uniform samplerCube u_cubemap;

out vec3 L;
out vec3 N;
out vec3 V;
out vec3 R;

void main() {
    // Calculate the view vector and the normal vector and pass them
    // as varying variables to the fragment shader

	//Transform the vertex position to view space (eye coordinates)
	vec3 position_eye = vec3(u_mv * a_position);

	// Calculate the view-space normal
	N = normalize(mat3(u_mv) * a_normal);
	
	// Calculate the view-space light direction
	//vec3 light_position = vec3(0.0, 0.0, 5.0);
	L = normalize(light_position - position_eye);

	// Calculate the view vector 
	V = normalize(-position_eye);

	// Calculate the reflection vector
	R = reflect(-V, N);
	

    gl_Position = MVPmatrix*a_position;
}
