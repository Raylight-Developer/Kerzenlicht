#version 460 core

uniform sampler2D accumulated_tex;

in vec2 uv_coords;
out vec4 color_out;

void main() {
	color_out = texture(accumulated_tex, uv_coords);
}