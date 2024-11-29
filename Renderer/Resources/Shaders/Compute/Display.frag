#version 460 core

uniform float KL_UNIFORM_DISPLAY_RATIO;
uniform float KL_UNIFORM_RENDER_RATIO;
uniform uint  KL_UNIFORM_DEBUG_LAYER;
uniform bool  KL_UNIFORM_USE_DEBUG_VIEW;
uniform uint  KL_UNIFORM_CURRENT_SAMPLE;

uniform sampler2D KL_RENDER_TEXTURE_ACCUMULATION;
uniform sampler2D KL_RENDER_TEXTURE_RAW;
uniform sampler2D KL_RENDER_TEXTURE_DEBUG_BVH;
uniform sampler2D KL_RENDER_TEXTURE_DEBUG_NORMAL;

in  vec2 KL_FRAG_UV;
out vec4 KL_FRAG_COLOR;

float f_mapFloat(float from_min, float from_max, float to_min, float to_max, float value) {
	return (to_min + ((to_max - to_min) / (from_max - from_min)) * (value - from_min));
}

vec2 f_mapVec2(vec2 from_min, vec2 from_max, vec2 to_min, vec2 to_max, vec2 value) {
	return vec2(f_mapFloat(from_min.x, from_max.x, to_min.x, to_max.x, value.x), f_mapFloat(from_min.y, from_max.y, to_min.y, to_max.y, value.y));
}

vec3 ColorGrade( vec3 vColor ) {
	vec3 vHue = vec3(1.0, .7, .2);

	vec3 vGamma = 1.0 + vHue * 0.6;
	vec3 vGain = vec3(.9) + vHue * vHue * 8.0;

	vColor *= 1.5;

	float fMaxLum = 100.0;
	vColor /= fMaxLum;
	vColor = pow( vColor, vGamma );
	vColor *= vGain;
	vColor *= fMaxLum;
	return pow(tanh(vColor), vec3(0.57));
}

void main() {
	if (KL_UNIFORM_USE_DEBUG_VIEW) {
		switch (KL_UNIFORM_DEBUG_LAYER) {
			case 0:
				//KL_FRAG_COLOR = vec4(ColorGrade(texture(KL_RENDER_TEXTURE_ACCUMULATION, KL_FRAG_UV).rgb), 1);
				KL_FRAG_COLOR = texture(KL_RENDER_TEXTURE_ACCUMULATION, KL_FRAG_UV);
				break;
			case 1:
				KL_FRAG_COLOR = texture(KL_RENDER_TEXTURE_RAW, KL_FRAG_UV);
				break;
			case 2:
				KL_FRAG_COLOR = texture(KL_RENDER_TEXTURE_DEBUG_BVH, KL_FRAG_UV);
				break;
			case 3:
				KL_FRAG_COLOR = texture(KL_RENDER_TEXTURE_DEBUG_NORMAL, KL_FRAG_UV);
				break;
		}
	}
	else {
		//KL_FRAG_COLOR = vec4(ColorGrade(texture(KL_RENDER_TEXTURE_ACCUMULATION, KL_FRAG_UV).rgb), 1);
		KL_FRAG_COLOR = texture(KL_RENDER_TEXTURE_ACCUMULATION, KL_FRAG_UV);
	}
}