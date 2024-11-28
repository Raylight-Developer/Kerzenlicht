#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 coords;

uniform float KL_display_aspect_ratio;
uniform float KL_render_aspect_ratio;

out vec2 KL_uv_coords;

void main() {
	if (KL_display_aspect_ratio < KL_render_aspect_ratio)
		gl_Position = vec4(position * vec2(1.0, KL_display_aspect_ratio/KL_render_aspect_ratio), 0.0, 1.0);
	else
		gl_Position = vec4(position * vec2(KL_render_aspect_ratio/KL_display_aspect_ratio, 1.0), 0.0, 1.0);
	KL_uv_coords = coords;
}