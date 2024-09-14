#version 460
out vec4 fragColor;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

uniform vec3 camera_pos;

float LambertianShading(vec3 normal, vec3 lightDir) {
	return max(dot(normal, lightDir), 0.0);
}

float fresnel_dielectric_cos(float cosi, float eta) {
	float c = abs(cosi);
	float g = eta * eta - 1 + c * c;
	float result;

	if (g > 0) {
		g = sqrt(g);
		float A = (g - c) / (g + c);
		float B = (c * (g + c) - 1) / (c * (g - c) + 1);
		result = 0.5 * A * A * (1 + B * B);
	}
	else {
		result = 1.0;
	}
	return result;
}

float F0_from_ior(float eta) {
	float f0 = (eta - 1.0) / (eta + 1.0);
	return f0 * f0;
}

float ior_from_F0(float f0) {
	float sqrt_f0 = sqrt(clamp(f0, 0.0, 0.99));
	return (1.0 + sqrt_f0) / (1.0 - sqrt_f0);
}

void main() {
	vec3 lightDir = normalize(vec3(1.0, -1.0, 1.0));
	vec3 viewDir = normalize(camera_pos - fragPos);

	float cosi = dot(viewDir, fragNormal);
	float fresnel = fresnel_dielectric_cos(cosi, 1.5);

	vec3 color = vec3(fresnel);
	//color = fragNormal;
	fragColor = vec4(color, 1.0);
}