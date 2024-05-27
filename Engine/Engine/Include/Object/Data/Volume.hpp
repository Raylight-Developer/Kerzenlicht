#pragma once

#include "Include.hpp"

namespace CLASS::OBJECT::DATA {
	namespace VOLUME {
		enum struct Type {
			HOMOGENEOUS,
			PER_PARTICLE
		};

		struct Particle {
			dvec1 density;
			dvec3 color;
		};

		struct Grid {
			vector<vector<Particle>> voxel_grid;
			dvec1 voxel_size;
		};
		struct Homogeneous_Grid {
			vector<vector<bool>> voxel_grid;
			dvec1 voxel_size;
			dvec1 density;
			dvec1 color;
		};
	}

	struct Volume {
		VOLUME::Type type;
		void* data;

		Volume();
		~Volume();
	};
}