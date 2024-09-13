#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUV;

uniform mat4 view_mat;

void main() {
	fragUV = aUV;
	fragNormal = normalize(mat3(view_mat) * aNormal);

	vec4 global_pos = view_mat * vec4(aPos, 1.0);
	fragPos = global_pos.xyz;
	gl_Position = global_pos;
}