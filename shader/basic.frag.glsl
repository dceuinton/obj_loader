#version 410

in vec2 texture_coordinates;
in vec4 frag_position;
in vec4 frag_normal;
in vec4 frag_light_direction;


// Light source
// uniform vec4 Ia = vec4(0.8f, 0.8f, 0.8f, 1.0f);
uniform vec4 Ia = vec4(0.3f, 0.3f, 0.3f, 1.0f);
// uniform vec4 Ia = vec4(0.1f, 0.1f, 0.1f, 1.0f);
uniform vec4 Id = vec4(1.0f, 1.0f, 1.0f, 1.0f);
// uniform vec4 Id = vec4(0.5f, 0.5f, 0.5f, 1.0f);
uniform vec4 Is = vec4(1.0f, 1.0f, 1.0f, 1.0f);
// uniform vec4 Is = vec4(0.5f, 0.5f, 0.5f, 1.0f);

// Material values
uniform vec4 Ka = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 Kd = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 Ks = vec4(0.0, 0.0, 0.0, 1.0);
uniform float a = 1.0;

uniform int useTexture;
uniform sampler2D u_texture; 
uniform int useSpecMap;
uniform sampler2D u_specMap;

out vec4 frag_colour;

void main() {

	// Direction to Light (normalised)
	vec4 l = normalize(-frag_light_direction);

	vec4 testing = vec4(0.0,0.0,0.0,0.0);

	// Surface Normal (normalised)
	vec4 n = normalize(frag_normal);

	// Reflected Vector
	vec4 r = reflect(-l, n);

	// View Vector
	vec4 v = normalize(-frag_position);

	vec4 KaVal = Ka;
	vec4 KdVal = Kd;
	vec4 KsVal = Ks;

	// frag_colour = texture(u_specMap, texture_coordinates);
	// return;

	if (useTexture == 1) {
		KaVal = texture(u_texture, texture_coordinates);
		KdVal = Ka;
	} 

	if (useSpecMap == 1) {
		KsVal = texture(u_specMap, texture_coordinates);
	}

	// ---------- Calculate Terms ----------
	// Ambient Term
	vec4 Ta = KaVal * Ia;

	// Diffuse Term
	vec4 Td = KdVal * max(dot(l, n), 0.0) * Id;

	// Specular Term
	vec4 Ts = KsVal * pow((max(dot(r, v), 0.0)), a) * Is;

	frag_colour = Ta + Td + Ts;
}