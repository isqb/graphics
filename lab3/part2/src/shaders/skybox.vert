//Skybox vertex shader

layout(location = 0) in vec4 a_position;

uniform mat4 MVPmatrix;

out vec3 Position;

void main()
{
    Position = a_position.xyz;
    gl_Position = MVPmatrix*a_position;
}