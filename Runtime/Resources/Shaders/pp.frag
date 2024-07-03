#version 460 core

uniform sampler2D acc_frame;

in vec2 uv_coords;

out vec4 fragColor;

void main() {
	fragColor = texture(acc_frame, uv_coords);
}