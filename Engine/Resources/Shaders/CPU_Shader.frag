#version 450 core

uniform uint current_frame;

uniform sampler2D render;
uniform sampler2D last_frame;

in vec2 UV_Coords;

out vec4 Frag_Color;

bool reset = false;
void main() {
	float weight = float(current_frame);
	if (current_frame <= 1 || !reset)
		Frag_Color = vec4((texture(last_frame, UV_Coords).xyz * weight + texture(render, UV_Coords).xyz) / (weight + 1.0), 1);
	else
		Frag_Color = texture(render, UV_Coords);
}