#version 460 core

uniform uint  runframe;
uniform bool  reset;

uniform sampler2D raw_frame;
uniform sampler2D last_frame;

in vec2 uv_coords;

out vec4 fragColor;

void main() {
	float weight = float(runframe);
	if (runframe <= 1 || !reset)
		fragColor = vec4((texture(last_frame, uv_coords).xyz * weight + texture(raw_frame, uv_coords).xyz) / (weight + 1.0), 1);
	else
		fragColor = texture(raw_frame, uv_coords);
}