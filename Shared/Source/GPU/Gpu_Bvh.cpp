#include "GPU/Gpu_Bvh.hpp"

KL::GPU::Bvh::Bvh(const vec3& p_min, const vec3& p_max, const uint& uptr, const uint& tri_count) :
	p_min(p_min),
	p_max(p_max),
	uptr(uptr),
	tri_count(tri_count)
{}

void KL::GPU::Bvh::growToInclude(const vec3& min, const vec3& max) {
	p_min.x = min.x < p_min.x ? min.x : p_min.x;
	p_min.y = min.y < p_min.y ? min.y : p_min.y;
	p_min.z = min.z < p_min.z ? min.z : p_min.z;
	p_max.x = max.x > p_max.x ? max.x : p_max.x;
	p_max.y = max.y > p_max.y ? max.y : p_max.y;
	p_max.z = max.z > p_max.z ? max.z : p_max.z;
}

vec3 KL::GPU::Bvh::getSize() {
	return p_max - p_min;
}

vec3 KL::GPU::Bvh::getCenter() {
	return (p_min + p_max) / 2.0f;
}