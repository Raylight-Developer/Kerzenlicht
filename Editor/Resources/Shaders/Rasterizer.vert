#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUV;

uniform mat4 view_mat;

void main() {
	fragNormal = aNormal;
	fragUV = aUV;
	vec4 global_pos = view_mat * vec4(aPos, 1.0);
	gl_Position = global_pos;
	fragPos = global_pos.xyz;
}
/*
layout(location = 0) in vec3 aPos;

out vec2 TexCoord;

uniform mat4 view;

void main()
{
	TexCoord = (aPos.xy + vec2(1.0, 1.0)) / 2.0; // Map to [0, 1] range
	gl_Position = view * vec4(aPos, 1.0);
}
*/