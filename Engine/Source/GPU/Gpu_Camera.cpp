#include "GPU/Gpu_Camera.hpp"

KL::GPU::Camera_Lens::Camera_Lens(const dvec1& curvature_radius, const dvec1& aperture_radius, const dvec1& z_distance, const dvec1& ior) :
	curvature_radius(d_to_f(curvature_radius * 0.01)),
	aperture_radius(d_to_f(aperture_radius * 0.01)),
	z_distance(d_to_f(z_distance * 0.01)),
	ior(d_to_f(ior))
{}