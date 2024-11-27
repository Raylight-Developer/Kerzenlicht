#include "GPU/Gpu_Bvh.hpp"

#include "Utils/Session.hpp"

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

vec3 KL::GPU::Bvh::getSize() const {
	return p_max - p_min;
}

vec3 KL::GPU::Bvh::getCenter() const {
	return (p_min + p_max) / 2.0f;
}

KL::GPU::BVH::Tri::Tri(const vec3& p_min, const vec3& p_max, const vec3& center, const uint64& index, const GPU::Triangle& tri) :
	center(center),
	p_min(p_min),
	p_max(p_max),
	index(index),
	tri(tri)
{}

KL::GPU::BVH::BLAS::BLAS(const vector<GPU::Triangle>& triangles, const uint& depth) :
	mesh_triangles(triangles)
{
	for (uint i = 0; i < len32(triangles); i++) {
		const vec3 a = triangles[i].pos_a;
		const vec3 b = triangles[i].pos_b;
		const vec3 c = triangles[i].pos_c;
		const vec3 center = (a + b + c) / 3.0f;
		const vec3 max = glm::max(glm::max(a, b), c);
		const vec3 min = glm::min(glm::min(a, b), c);
		bvh_tris.push_back(BVH::Tri(min, max, center, i, triangles[i]));
		mesh_bounds.growToInclude(min, max);
	}

	mesh_blas.push_back(mesh_bounds);
	splitBvh(0, 0, len32(triangles), depth);

	this->mesh_triangles.clear();
	for (uint i = 0; i < len32(bvh_tris); i++) {
		this->mesh_triangles.push_back(bvh_tris[i].tri);
	}
}

void KL::GPU::BVH::BLAS::splitBvh(const uint& parentIndex, const uint& triGlobalStart, const uint& triNum, const uint& depth) {
	GPU::Bvh parent = mesh_blas[parentIndex];
	vec3 size = parent.getSize();
	vec1 parentCost = nodeCost(size, triNum);

	uint8 split_axis;
	vec1 splitPos;
	vec1 cost;

	splitAxis(parent, triGlobalStart, triNum, split_axis, splitPos, cost);

	if (cost < parentCost && depth > 0) {
		GPU::Bvh boundsLeft;
		GPU::Bvh boundsRight;
		uint numOnLeft = 0;

		for (uint i = triGlobalStart; i < triGlobalStart + triNum; i++) {
			BVH::Tri tri = bvh_tris[i];
			if (tri.center[split_axis] < splitPos) {
				boundsLeft.growToInclude(tri.p_min, tri.p_max);

				BVH::Tri swap = bvh_tris[triGlobalStart + numOnLeft];
				bvh_tris[triGlobalStart + numOnLeft] = tri;
				bvh_tris[i] = swap;
				numOnLeft++;
			}
			else {
				boundsRight.growToInclude(tri.p_min, tri.p_max);
			}
		}

		uint numOnRight = triNum - numOnLeft;
		uint triStartLeft = triGlobalStart;
		uint triStartRight = triGlobalStart + numOnLeft;

		mesh_blas.push_back(GPU::Bvh(boundsLeft.p_min, boundsLeft.p_max, triStartLeft));
		uint childIndexRight = len32(mesh_blas);
		uint childIndexLeft = childIndexRight - 1;
		mesh_blas.push_back(GPU::Bvh(boundsRight.p_min, boundsRight.p_max, triStartRight));

		parent.uptr = childIndexLeft;
		mesh_blas[parentIndex] = parent;

		splitBvh(childIndexLeft, triGlobalStart, numOnLeft, depth - 1);
		splitBvh(childIndexRight, triGlobalStart + numOnLeft, numOnRight, depth - 1);
	}
	else {
		parent.uptr = triGlobalStart;
		parent.tri_count = triNum;
		mesh_blas[parentIndex] = parent;
	}
}

void KL::GPU::BVH::BLAS::splitAxis(const GPU::Bvh& node, const uint& start, const uint& count, uint8& axis, vec1& pos, vec1& cost) const {
	if (count <= 1) {
		axis = 0;
		pos = 0.0f;
		cost = MAX_VEC1;
		return;
	}

	vec1 bestSplitPos = 0;
	uint8 bestSplitAxis = 0;
	const uint8 numSplitTests = 5;

	vec1 bestCost = MAX_VEC1;

	for (uint8 axis = 0; axis < 3; axis++) {
		for (uint8 i = 0; i < numSplitTests; i++) {
			vec1 splitT = (u_to_f(i) + 1.0f) / (u_to_f(numSplitTests) + 1.0f);
			vec1 splitPos = glm::lerp(node.p_min[axis], node.p_max[axis], splitT);
			vec1 cost = splitEval(axis, splitPos, start, count);
			if (cost < bestCost) {
				bestCost = cost;
				bestSplitPos = splitPos;
				bestSplitAxis = axis;
			}
		}
	}

	axis = bestSplitAxis;
	pos = bestSplitPos;
	cost = bestCost;
}

vec1 KL::GPU::BVH::BLAS::splitEval(const uint8& splitAxis, const vec1& splitPos, const uint& start, const uint& count) const {
	GPU::Bvh boundsLeft = GPU::Bvh();
	GPU::Bvh boundsRight;
	uint numOnLeft = 0;
	uint numOnRight = 0;

	for (uint i = start; i < start + count; i++) {
		BVH::Tri tri = bvh_tris[i];
		if (tri.center[splitAxis] < splitPos) {
			boundsLeft.growToInclude(tri.p_min, tri.p_max);
			numOnLeft++;
		}
		else {
			boundsRight.growToInclude(tri.p_min, tri.p_max);
			numOnRight++;
		}
	}

	vec1 costA = nodeCost(boundsLeft.getSize(), numOnLeft);
	vec1 costB = nodeCost(boundsRight.getSize(), numOnRight);
	return costA + costB;
}

vec1 KL::GPU::BVH::BLAS::nodeCost(const vec3& size, const uint& numTriangles) {
	const vec1 halfArea = size.x * size.y + size.x * size.z + size.y * size.z;
	return halfArea * numTriangles;
}

KL::GPU::BVH::TLAS::TLAS(const vector<GPU::Bvh>&mesh_blas, const uint& depth) :
	mesh_blas(mesh_blas)
{
}
