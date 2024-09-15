#version 460 core

uniform float display_aspect_ratio;
uniform float render_aspect_ratio;

uniform sampler2D render;

in vec2 fragUV;

out vec4 fragColor;

void main() {
	fragColor = texture(render, fragUV);
	//fragColor = vec4(fragUV.x,0,fragUV.y,1);
}