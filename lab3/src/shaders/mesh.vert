// Vertex shader
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 MVPmatrix;
uniform mat4 u_mv; // ModelView matrix
uniform vec3 light_position; // The position of your light source
uniform vec3 light_color; //Color of light

out vec3 v_color;

void main() {
	// Calculate the view-space normal, the view-space light vector,
    // and the view vector here and pass them as varying variables to
    // the fragment shader

	v_color = 0.5 * a_normal + 0.5; // maps the normal vector to an RGB color
    gl_Position = MVPmatrix * a_position;
	// Transform the vertex position to view space (eye coordinates)
	vec3 position_eye = vec3(MVPmatrix * a_position);
	
	// Calculate the view-space normal
	vec3 N = normalize(mat3(u_mv) * a_normal);
	
	// Calculate the view-space light direction
	vec3 light_position = vec3(0.0, 0.0, 5.0);
	vec3 L = normalize(light_position - position_eye);

	// Calculate the view-vector 

	vec3 V = -1*position_eye;
	
	// Calculate the diffuse (Lambertian) reflection term
	float diffuse = max(0.0, dot(N, L));
	
	// Multiply the diffuse reflection term with the color of the
	// surface and send the resulting color to the fragment shader
    v_color = diffuse * vec3(0.0, 1.0, 0.0);
	gl_Position = a_position;
}
