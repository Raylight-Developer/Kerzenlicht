#version 460

layout(location = 0) in vec3 aPos;

out vec3 fragPos;

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat4 projection_matrix;

void main() {
	fragPos = mat3(model_matrix) * aPos;

	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(aPos, 1.0);
}