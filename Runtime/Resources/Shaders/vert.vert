#version 460 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex_coord;

uniform float display_aspect_ratio;
uniform float render_aspect_ratio;

out vec2 uv_coords;

void main() {
	gl_Position = vec4(pos * vec2(1.0, 1.0/*display_aspect_ratio/render_aspect_ratio*/), 0.0, 1.0);
	uv_coords = tex_coord;
}