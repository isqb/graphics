//Skybox fragment shader
#version 150

uniform samplerCube u_cubemap;

in vec3 position;

out vec4 FragColor;

void main()
{
    //vec3 output_colour = textureCube(u_cubemap, position);
	//gl_FragColor = vec4(output_colour, 1.0);
	gl_FragColor = vec4(1.0,0.0,0.0,1.0);

}