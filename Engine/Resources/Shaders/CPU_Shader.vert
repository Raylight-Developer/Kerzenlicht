#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv_coords;

uniform float display_aspect_ratio;
uniform float render_aspect_ratio;

out vec2 UV_Coords;

void main() {
	gl_Position = vec4(position * vec2(1.0, display_aspect_ratio/render_aspect_ratio), 0.0, 1.0);
	UV_Coords = uv_coords;
}