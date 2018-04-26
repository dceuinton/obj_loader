#version 410

in vec3 vertex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
	gl_Position = u_projection * u_view * u_model * vec4(vertex, 1.0f);
}