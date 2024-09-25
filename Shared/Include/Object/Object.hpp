#pragma once

#include "Shared.hpp"

#include "Nodes/Object_Nodes.hpp"

#include "Shader/Shader.hpp"

#include "Data.hpp"
#include "Data/Atmosphere.hpp"
#include "Data/Primitive.hpp"
#include "Data/Skeleton.hpp"
#include "Data/Camera.hpp"
#include "Data/Volume.hpp"
#include "Data/Curve.hpp"
#include "Data/Empty.hpp"
#include "Data/Force.hpp"
#include "Data/Group.hpp"
#include "Data/Light.hpp"
#include "Data/Mesh.hpp"
#include "Data/Sfx.hpp"
#include "Data/Vfx.hpp"

// FWD DECL OTHER
namespace KL {
	struct Scene;
	struct Prop;
}
namespace SHADER {
	struct Shader;
}

// FWD DECL THIS

// DECL
namespace KL {
	struct Object {
		string name;
		OBJECT::Data* data;
		Object* parent;
		Transform transform;
		Transform node_transform;
		vector<KL::Shader*> shaders;
		unordered_map<string, KL::Prop> properties;

		bool viewport_visibility;
		bool render_visibility;

		bool cpu_update;

		KL::Node_Tree* node_tree;

		dmat4 transform_matrix;

		Object();

		void f_compileMatrix();
		void f_intersects(const dvec3& ray_origin, const dvec3& ray_direction);

		OBJECT::DATA::Atmosphere* getAtmosphere() { return static_cast<OBJECT::DATA::Atmosphere*>(data); };
		OBJECT::DATA::Primitive* getPrimitive()   { return static_cast<OBJECT::DATA::Primitive*>(data); };
		OBJECT::DATA::Camera* getCamera()         { return static_cast<OBJECT::DATA::Camera*>(data); };
		OBJECT::DATA::Volume* getVolume()         { return static_cast<OBJECT::DATA::Volume*>(data); };
		OBJECT::DATA::Curve* getCurve()           { return static_cast<OBJECT::DATA::Curve*>(data); };
		OBJECT::DATA::Empty* getEmpty()           { return static_cast<OBJECT::DATA::Empty*>(data); };
		OBJECT::DATA::Force* getForce()           { return static_cast<OBJECT::DATA::Force*>(data); };
		OBJECT::DATA::Group* getGroup()           { return static_cast<OBJECT::DATA::Group*>(data); };
		OBJECT::DATA::Light* getLight()           { return static_cast<OBJECT::DATA::Light*>(data); };
		OBJECT::DATA::Mesh* getMesh()             { return static_cast<OBJECT::DATA::Mesh*>(data); };
		OBJECT::DATA::Sfx* getSfx()               { return static_cast<OBJECT::DATA::Sfx*>(data); };
		OBJECT::DATA::Vfx* getVfx()               { return static_cast<OBJECT::DATA::Vfx*>(data); };
	};
}