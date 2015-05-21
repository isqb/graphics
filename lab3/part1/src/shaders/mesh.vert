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

uniform int border_switch;

out vec3 L;
out vec3 N;
out vec3 V;

void main() {
	// Calculate the view-space normal, the view-space light vector,
    // and the view vector here and pass them as varying variables to
    // the fragment shader

	//input_color = 0.5 * a_normal + 0.5; // maps the normal vector to an RGB color
	//Transform the vertex position to view space (eye coordinates)
	vec3 position_eye = vec3(u_mv * a_position);
	
	// Calculate the view-space normal
	N = normalize(mat3(u_mv) * a_normal);
	
	// Calculate the view-space light direction
	//vec3 light_position = vec3(0.0, 0.0, 5.0);
	L = normalize(light_position - position_eye); // 

	// Calculate the view-vector 
	V = normalize(-position_eye);
	
	// Multiply the diffuse reflection term with the color of the
	// surface and send the resulting color to the fragment shader
    //v_color = diffuse * vec3(0.0, 1.0, 0.0);

	float dotVN = dot(V,N);
	vec3 newpos;

/*
	if (border_switch == 1){
		//gl_Position = MVPmatrix * (a_position + (0.05 * vec4(N,1.0)));
		gl_Position = MVPmatrix * (5 * a_position);
	}
	else
	*/
	gl_Position = MVPmatrix*a_position;
}
