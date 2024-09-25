#pragma once

#include "Include.hpp"

#include "Core/Property.hpp"

#include "Object/Data.hpp"

// FWD DECL OTHER
namespace KL::OBJECT::DATA {
	struct Mesh;
}

// FWD DECL THIS
namespace KL::OBJECT::DATA {
	namespace VFX {
		struct Particle;
	}
}

// DECL
namespace KL::OBJECT::DATA {
	namespace VFX {
		struct Particle {
			Particle();
		};

		struct Newtonian_Point_Particle : Particle {
			dvec3 position;
			dvec3 velocity;
			dvec3 acceleration;
			dquat rotation;

			dvec1 mass;
			dvec1 radius;
			dvec1 lifespan;
			dvec1 restitution;
			dvec1 friction_coefficient;
			// Internal
			dvec1 inertia;
			dvec1 angular_velocity;

			Newtonian_Point_Particle();
		};

		struct Custom_Particle : Particle {
			unordered_map<string, Prop> properties;
		};
		enum struct Type {
			NONE,
			NEWTONIAN_POINT,
			CUSTOM
		};
	}

	struct Vfx : Data {
		vector<VFX::Particle> particles;
		VFX::Type type;
		dvec1 duration;

		Mesh* instance;

		Vfx();
	};
}