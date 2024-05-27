#include "GUI/Main_Window.hpp"

// Core/Lace.hpp ----------------------------------------------------------------------------------
struct Lace;
struct Trace;

namespace CLASS {
// Core/File.hpp ----------------------------------------------------------------------------------
	struct File;
	struct Scene;
// Core/Nodes.hpp ---------------------------------------------------------------------------------
	struct Node;
	struct Node_Tree;
	namespace NODE {
		enum struct Type {
			CONSTRAINT, GENERATE, PHYSICS, MODIFY, EXEC, MATH, LINK, UTIL
		};
		struct Data;
		namespace DATA {
			enum struct Type {
				EMPTY,
				STRING, DOUBLE, BOOL, UINT, INT,
				MAT2, MAT3, MAT4, UMAT2, UMAT3, UMAT4, IMAT2, IMAT3, IMAT4,
				VEC2, VEC3, VEC4, UVEC2, UVEC3, UVEC4, IVEC2, IVEC3, IVEC4,
				TRANSFORM, TEXTURE, OBJECT, SCENE, DATA,
			};
			enum struct Modifier {
				SINGLE,
				VECTOR,
				MAP
			};
		}
		struct Port;
		struct Connection;
		struct Data_I_Port;
		struct Data_O_Port;
		struct Exec_I_Port;
		struct Exec_O_Port;
// Core/Node_Def.hpp ------------------------------------------------------------------------------
		namespace CONSTRAINT {
			enum struct Type { PARENT };
		}
		namespace GENERATE {
			enum struct Type { SUBDIVISION, TRIANGULATE, WIREFRAME, SOLIDIFY, BOOLEAN, MIRROR, ARRAY, BEVEL };
		}
		namespace PHYSICS {
			enum struct Type { COLLISION, CACHE, CLOTH, FLUID, RIGID, SOFT, GAS };
		}
		namespace MODIFY {
			enum struct Type { SURFACE_DEFORM, KEYFRAMING, SHRINKWRAP, ARMATURE, SMOOTH };
		}
		namespace EXEC {
			enum struct Type { SEQUENCE, COUNTER, SCRIPT, START, TIMER, TICK, FOR };
			struct Sequence;
			struct Counter;
			struct Script;
			struct Timer;
			struct Tick;
		}
		namespace MATH {
			enum struct Type { RAMP, MAP, ADD, DIV, MUL, SUB };
		}
		namespace LINK {
			enum struct Type { VARIABLE, POINTER, SELF };
		}
		namespace UTIL {
			enum struct Type { SWITCH, BREAK, MAKE, VIEW };
		}
	};
// Object/Object.hpp ------------------------------------------------------------------------------
	struct Object;
	namespace OBJECT {
// Object/Data.hpp --------------------------------------------------------------------------------
		struct Data;
		namespace DATA {
			enum struct Type { ATMOSPHERE, PRIMITIVE, SKELETON, CAMERA, VOLUME, CURVE, EMPTY, FORCE, GROUP, LIGHT, MESH, SFX, VFX };
// Object/Data/Atmosphere.hpp ---------------------------------------------------------------------
			struct Atmosphere;
// Object/Data/Primitive.hpp ----------------------------------------------------------------------
			struct Primitive;
			namespace PRIMITIVE {
				enum struct Type { ELLIPSOID, CYLINDER, CAPSULE, SPHERE, TORUS, CONE, BOX };
				struct Ellipsoid;
				struct Cylinder;
				struct Capsule;
				struct Sphere;
				struct Torus;
				struct Cone;
				struct Box;
			};
// Object/Data/Skeleton.hpp -----------------------------------------------------------------------
			struct Skeleton;
			namespace SKELETON {
				struct Bone;
			}
// Object/Data/Camera.hpp -------------------------------------------------------------------------
			struct Camera;
// Object/Data/Volume.hpp -------------------------------------------------------------------------
			struct Volume;
			namespace VOLUME {
				enum struct Type { HETEROGENEOUS, HOMOGENEOUS };
				namespace HETEROGENEOUS {
					struct Particle;
					struct Domain;
				}
				namespace HOMOGENEOUS {
					struct Domain;
				}
			}
// Object/Data/Empty.hpp --------------------------------------------------------------------------
			struct Empty;
			namespace EMPTY {
				enum struct Type { POINT, AXES };
				struct Point;
				struct Axes;
			}
// Object/Data/Force.hpp --------------------------------------------------------------------------
			struct Force;
			namespace FORCE {
				enum struct Type { TURBULENCE, BROWNIAN, HARMONIC, MAGNETIC, VORTEX, CURVE, POINT, DRAG, WIND };
				struct Turublence;
				struct Brownian;
				struct Harmonic;
				struct Magnetic;
				struct Vortex;
				struct Curve;
				struct Point;
				struct Drag;
				struct Wind;
			}
// Object/Data/Group.hpp --------------------------------------------------------------------------
			struct Group;
// Object/Data/Light.hpp --------------------------------------------------------------------------
			struct Light;
			namespace LIGHT {
				enum struct Type { DIRECTIONAL , POINT , AREA, SPOT };

				struct Directional;
				struct Point;
				struct Area;
				struct Spot;
			}
// Object/Data/Mesh.hpp ---------------------------------------------------------------------------
			struct Mesh;
			namespace MESH {
				struct Vertex;
				struct Face;
			}
// Object/Data/Sfx.hpp ----------------------------------------------------------------------------
			struct Sfx;
// Object/Data/Vfx.hpp ----------------------------------------------------------------------------
			struct Vfx;
		}
	}
// Rendering/Material/Material.hpp ----------------------------------------------------------------
	struct Material;
	namespace MATERIAL {
		namespace NODE {
			enum struct Type { SHADER, MATH, IO };
			namespace SHADER {
				enum struct Type { COMPLETE, VOLUME };
			}
			namespace MATH {
				enum struct Type { DISTANCE };
			}
			namespace IO {
				enum struct Type { ACTIVE_SCENE, OBJECT_THIS, OBJECT_INPUT };
			}
		}
	}
}

namespace WORKSPACE {
	enum struct Type { PROPERTIES, VIEWPORT, OUTLINER, NODES, API, LOG };
}

namespace GUI {

}