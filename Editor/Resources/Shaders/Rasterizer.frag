#version 460
out vec4 FragColor;

//in vec3 fragNormal;
//in vec2 fragUV;

in vec2 TexCoord;

void main() {
	FragColor = vec4(TexCoord, 0.0, 1.0); // Simple gradient for testing
}