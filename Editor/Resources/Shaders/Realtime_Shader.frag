#version 450 core

uniform sampler2D render;

in vec2 frag_uv;

out vec4 frag_color;

void main() {
	frag_color = texture(render, frag_uv);
}