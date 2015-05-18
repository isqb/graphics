//Skybox fragment shader
#version 150

//uniform mat4 u_projection;
//uniform mat4 u_view;
uniform samplerCube u_cubemap;

in vec3 TexCoords;

out vec4 gl_FragColor;

void main()
{
    vec3 output_colour = texture(u_cubemap, TexCoords);
	//gl_FragColor = vec4(output_colour, 1.0);
	gl_FragColor = vec4(0.0,1.0,0.0,1.0);

}