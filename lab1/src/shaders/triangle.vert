// Vertex shader
#version 130

in vec4 a_position;
uniform float u_time;
vec4 some_poss;

void main() {
	//some_poss = (a_position.x,sin(u_time+1),a_position.z,1.0);
	//some_poss += vec4(0, sin(u_time+1),cos(u_time),1.0);
    gl_Position = vec4(a_position.x,a_position.y+sin(u_time),a_position.z,1.0);
}
