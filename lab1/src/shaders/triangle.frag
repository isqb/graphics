// Fragment shader
#version 130

uniform float u_time;
vec3 a_color;


void main() {
	a_color = vec3(sin(u_time),cos(u_time-1),0);
    gl_FragColor = vec4(a_color, 1.0);
}
