// Fragment shader
#version 150

out vec4 FragColor;

//in vec3 v_color;

in vec3 input_color;

void main() {
    // Implement the normalized Blinn-Phong shading model here
	vec3 output_color = input_color;
    FragColor = vec4(output_color, 1.0);

}
