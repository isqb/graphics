// Fragment shader
#version 150

out vec4 FragColor;
//out vec4 output_color;
uniform vec3 light_position;
uniform vec3 light_color;

void main() {
    // Implement the normalized Blinn-Phong shading model here
	//output_color = pow(output_color, vec3(1 / 2.2));
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
