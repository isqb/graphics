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

in vec3 L;
in vec3 V;
in vec3 N;

out vec4 FragColor;

void main() {
    // Implement the normalized Blinn-Phong shading model here

	vec3 H = normalize(L+V);
	vec3 Ia = ambient_color;
	vec3 Id = diffuse_color*light_color*max(dot(N,L),0);
	vec3 Is = ((specular_power+8)/8)*(specular_color*light_color)*pow(max(dot(N,H),0.0),(specular_power));
	float intensity = Ia + Id + Is;

	vec3 output_color;
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

	//vec3 output_color = vec3(0.0,1.0,0.0);
	
	//vec3 color = vec3(1.0,0.5,0.5);
	vec3 color = output_color;
	if (intensity > 0.95)
		color = color;
	else if (intensity > 0.5)
		color = 0.5 * color;
	else if (intensity > 0.25)
		color = 0.25 * color;
	else
		color = 0.1 * color;
	
	float border = dot(V,N);

	if (border < 0){
		color = vec3(0.0,0.0,0.0);
	}

	/*
	if(border < 0.25){
		
	}
	*/
	gl_FragColor = vec4(color, 1.0);
    //FragColor = vec4(output_color, 1.0);

}
