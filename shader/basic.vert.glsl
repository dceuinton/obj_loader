#version 410

in vec3 vertex;
in vec2 texcoord;
in vec3 normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform vec4 u_light_direction = vec4(0.0f, 0.0f, -1.0f, 0.0f);

out vec2 texture_coordinates;
out vec4 frag_position;
out vec4 frag_normal;
out vec4 frag_light_direction;

void main() {

	texture_coordinates = texcoord;

	frag_position = u_view * u_model * vec4(vertex, 1.0f);

	frag_normal = u_view * u_model * vec4(normal, 0.0f);

	frag_light_direction = /*u_view **/ u_light_direction;

	gl_Position = u_projection * u_view * u_model * vec4(vertex, 1.0f);
}