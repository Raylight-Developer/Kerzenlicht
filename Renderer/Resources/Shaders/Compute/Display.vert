#version 460 core

layout(location = 0) in vec2 KL_VERT_POS;
layout(location = 1) in vec2 KL_VERT_UV;

uniform float KL_UNIFORM_DISPLAY_RATIO;
uniform float KL_UNIFORM_RENDER_RATIO;

out vec2 KL_FRAG_UV;

void main() {
	if (KL_UNIFORM_DISPLAY_RATIO < KL_UNIFORM_RENDER_RATIO)
		gl_Position = vec4(KL_VERT_POS * vec2(1.0, KL_UNIFORM_DISPLAY_RATIO/KL_UNIFORM_RENDER_RATIO), 0.0, 1.0);
	else
		gl_Position = vec4(KL_VERT_POS * vec2(KL_UNIFORM_RENDER_RATIO/KL_UNIFORM_DISPLAY_RATIO, 1.0), 0.0, 1.0);
	KL_FRAG_UV = KL_VERT_UV;
}