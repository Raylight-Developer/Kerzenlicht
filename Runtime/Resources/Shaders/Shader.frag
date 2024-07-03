#version 460 core

uniform float display_aspect_ratio;
uniform float render_aspect_ratio;

uniform uint frame_count;

uniform sampler2D render_tex;
uniform sampler2D previous_frame_tex;

in vec2 uv_coords;
out vec4 color_out;

void main() {
	float weight = float(frame_count);

	if (frame_count > 1)
		color_out = vec4((texture(previous_frame_tex, uv_coords).xyz * weight + texture(render_tex, uv_coords).xyz) / (weight + 1.0), 1) ;
	else
		color_out = texture(render_tex, uv_coords);
}