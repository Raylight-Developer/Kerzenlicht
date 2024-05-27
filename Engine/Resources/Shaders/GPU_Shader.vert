#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv_coords;

uniform float display_aspect_ratio;
uniform float render_aspect_ratio;

out vec2 frag_uv;

void main() {
	gl_Position = vec4(position, 0.0, 1.0);
	frag_uv = uv_coords;
}