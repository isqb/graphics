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
uniform int u_cel_switch;
uniform int border_switch;
uniform vec3 u_intensity_bounds;

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
	float intensity = dot(L,N);

	vec3 output_color;
	
	if (border_switch == 1){
		output_color = vec3(0.0, 0.0, 0.0);
	}
	else{
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
	
		//vec3 output_color = vec3(1.0,0.5,0.5);
		if (u_cel_switch == 1){
			if (intensity > u_intensity_bounds.x)		// Upper Bound	
				output_color = 0.9 * output_color;
			else if (intensity > u_intensity_bounds.y)	// Middle Bound	
				output_color = 0.75 * output_color;
			else if (intensity > u_intensity_bounds.z)	// Lower Bound
				output_color = 0.5 * output_color;
			else
				output_color = 0.26 * output_color;
	
			float border = dot(V,N);

			/*
			if (border < 0){
				output_color = vec3(0.0,0.0,0.0);
			}
			*/
		}
	}
	

	gl_FragColor = vec4(output_color, 1.0);
    //FragColor = vec4(output_color, 1.0);

}
