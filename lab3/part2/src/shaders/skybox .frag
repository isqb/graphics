//Skybox fragment shader

uniform samplerCube SkyTexture;

in vec3 Position;

out vec4 FragColor;

void main()
{
    gl_FragColor = textureCube(SkyTexture, Position);
}