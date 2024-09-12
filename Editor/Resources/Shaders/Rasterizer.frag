#version 460
out vec4 fragColor;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

uniform vec3 camera_pos;

float LambertianShading(vec3 normal, vec3 lightDir) {
	return max(dot(normal, lightDir), 0.0);
}

float FresnelSchlick(float F0, vec3 viewDir, vec3 normal) {
	float cosTheta = min(abs(dot(viewDir, normal)), 1.0);
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(vec3(1.0, -1.0, 1.0));
	vec3 viewDir = normalize(camera_pos - fragPos);

	float diffuse = LambertianShading(normal, lightDir);
	float fresnel = FresnelSchlick(0.5, viewDir, normal);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 color = vec3((fresnel * 0.2 + 0.8) * (diffuse * 0.5 + 0.5) * (1.0 - length(camera_pos - fragPos) * 0.05));
	color = vec3(abs(dot(normal, viewDir)));
	fragColor = vec4(color, 1.0);
}