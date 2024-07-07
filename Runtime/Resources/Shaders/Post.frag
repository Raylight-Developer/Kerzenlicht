#version 460 core

uniform float display_aspect_ratio;
uniform float render_aspect_ratio;
#define DEBUG false

uniform sampler2D render;
uniform sampler2D raw_render;

in vec2 uv_coords;
out vec4 color_out;

float f_mapFloat(float from_min, float from_max, float to_min, float to_max, float value) {
	return (to_min + ((to_max - to_min) / (from_max - from_min)) * (value - from_min));
}

vec2 f_mapVec2(vec2 from_min, vec2 from_max, vec2 to_min, vec2 to_max, vec2 value) {
	return vec2(f_mapFloat(from_min.x, from_max.x, to_min.x, to_max.x, value.x), f_mapFloat(from_min.y, from_max.y, to_min.y, to_max.y, value.y));
}

void main() {
	if (DEBUG) {
		if (uv_coords.x < 0.5 && uv_coords.y >= 0.5) {
			vec4 color = texture(render, f_mapVec2(vec2(0.0, 0.5), vec2(0.5, 1.0), vec2(0.0), vec2(1.0), uv_coords));
			color_out = vec4(color.xyz / color.w, 1);
		}
		else if (uv_coords.x >= 0.5 && uv_coords.y >= 0.5) {
			color_out = texture(raw_render, f_mapVec2(vec2(0.5), vec2(1.0), vec2(0.0), vec2(1.0), uv_coords));
		}
		else {
			color_out = vec4(uv_coords.x, 0, uv_coords.y, 1);
		}
	}
	else {
		vec4 color = texture(render, uv_coords);
		color_out = vec4(color.xyz / color.w, 1);
	}
}