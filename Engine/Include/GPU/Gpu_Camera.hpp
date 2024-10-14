#pragma once

#include "Shared.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::GPU {
	struct alignas(16) Camera_Lens {
		vec1 curvature_radius; // if negative, it is a convex surface (the closest point to the camera is its center)
		vec1 aperture_radius;
		vec1 z_distance;
		vec1 ior;

		Camera_Lens(const dvec1& curvature_radius, const dvec1& aperture_radius, const dvec1& z_distance, const dvec1& ior);
	};
}