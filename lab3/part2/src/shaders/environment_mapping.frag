// Fragment shader
#version 150

uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float specular_power;
uniform vec3 light_color; //Color of light

uniform int ambient_switch;
uniform int diffuse_switch;
uniform int specular_switch;
uniform int gamma_swtich;
uniform int invert_switch;
uniform int normals_switch;
uniform samplerCube u_cubemap;

in vec3 L;
in vec3 V;
in vec3 N;

out vec4 FragColor;

void main() {
    // Implement reflective environment mapping here. Start by
    // calculating the reflection vector and then use it to look up
    // the color value in the cube map.
	vec3 H = normalize(L+V);
	vec3 Ia = ambient_color;
	vec3 Id = diffuse_color*light_color*max(dot(N,L),0);
	vec3 Is = ((specular_power+8)/8)*(specular_color*light_color)*pow(max(dot(N,H),0.0),(specular_power));
	// Calculate the reflection vector
	vec3 R = reflect(-V, N);

	vec3 output_color = texture(u_cubemap, R).rgb;
	
	if(ambient_switch == 1) {
		output_color += Ia;
	}
	if(diffuse_switch == 1) {
		output_color += Id;
	}
	if(specular_switch == 1) {
		output_color += Is;
	}
	if(normals_switch == 1) {
		output_color = N;
	}

	if(invert_switch == 1) {
		output_color = vec3(1) - output_color;
	}
	if(gamma_swtich == 1) {
		output_color = pow(output_color, vec3(1 / 2.2));
	}
	

    gl_FragColor = vec4(output_color, 1.0);
}
