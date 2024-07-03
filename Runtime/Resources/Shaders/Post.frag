#version 460 core

uniform float display_aspect_ratio;
uniform float render_aspect_ratio;

uniform sampler2D render;

in vec2 uv_coords;
out vec4 color_out;

void main() {
	vec4 color = texture(render, uv_coords);
	color_out = vec4(color.xyz / color.w, 1);
}