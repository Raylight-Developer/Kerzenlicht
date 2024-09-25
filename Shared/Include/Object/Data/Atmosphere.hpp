#pragma once

#include "Include.hpp"

#include "Object/Data.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL::OBJECT::DATA {
	struct Atmosphere : Data {
		dvec1 radius;
		dvec1 thickness;

		dvec1 rayleigh_exponential_distribution;
		dvec1 rayleigh_scattering_scale;

		dvec1 mie_exponential_distribution;
		dvec1 mie_scattering_scale;
		dvec1 mie_absorption_scale;
		dvec1 mie_anisotropy;

		dvec1 absorption_scale;

		dvec3 rayleigh_scattering_color;
		dvec3 mie_scattering_color;
		dvec3 mie_absorption_color;
		dvec3 absorption_color;
	};
}