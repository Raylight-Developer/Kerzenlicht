#pragma once

#include "Include.hpp"

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

namespace KL::OBJECT::DATA {
	enum struct Type {
		NONE,
		ATMOSPHERE,
		PRIMITIVE,
		SKELETON,
		CAMERA,
		VOLUME,
		CURVE,
		EMPTY,
		FORCE,
		GROUP,
		LIGHT,
		MESH,
		SFX,
		VFX
	};

}
namespace KL::OBJECT {
	struct Data {
		string name;
		DATA::Type type;
		void* data;

		Data();
		Data(const KL::OBJECT::DATA::Type& type, void* data);

		DATA::Atmosphere* getAtmosphere() const { return static_cast<DATA::Atmosphere*>(data); };
		DATA::Primitive* getPrimitive()   const { return static_cast<DATA::Primitive*>(data); };
		DATA::Camera* getCamera()         const { return static_cast<DATA::Camera*>(data); };
		DATA::Volume* getVolume()         const { return static_cast<DATA::Volume*>(data); };
		DATA::Curve* getCurve()           const { return static_cast<DATA::Curve*>(data); };
		DATA::Empty* getEmpty()           const { return static_cast<DATA::Empty*>(data); };
		DATA::Force* getForce()           const { return static_cast<DATA::Force*>(data); };
		DATA::Group* getGroup()           const { return static_cast<DATA::Group*>(data); };
		DATA::Light* getLight()           const { return static_cast<DATA::Light*>(data); };
		DATA::Mesh* getMesh()             const { return static_cast<DATA::Mesh*>(data); };
		DATA::Sfx* getSfx()               const { return static_cast<DATA::Sfx*>(data); };
		DATA::Vfx* getVfx()               const { return static_cast<DATA::Vfx*>(data); };
	};
}