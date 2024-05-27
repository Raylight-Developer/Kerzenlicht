#pragma once

namespace CLASS {

	struct File;

	struct Scene;

	namespace OBJECT {

		namespace DATA {
			enum Type { ATMOSPHERE, PRIMITIVE, SKELETON, CAMERA, VOLUME, CURVE, EMPTY, FORCE, GROUP, LIGHT, MESH, SFX, VFX };

			struct Atmosphere;

			namespace PRIMITIVE {
				enum Type { ELLIPSOID, CYLINDER, CAPSULE, SPHERE, TORUS, CONE, BOX };

				struct Ellipsoid;
				struct Cylinder;
				struct Capsule;
				struct Sphere;
				struct Torus;
				struct Cone;
				struct Box;
			};
			struct Primitive;

			namespace SKELETON {
				struct Bone;
			}
			struct Skeleton;

			struct Camera;

			namespace VOLUME {
				enum Type { HETEROGENEOUS, HOMOGENEOUS };

				namespace HETEROGENEOUS {
					struct Particle;
					struct Domain;
				}

				namespace HOMOGENEOUS {
					struct Domain;
				}
			}
			struct Volume;

			namespace EMPTY {
				enum Type { POINT, AXES };

				struct Point;
				struct Axes;
			}
			struct Empty;

			namespace FORCE {
				enum Type { TURBULENCE, BROWNIAN, HARMONIC, MAGNETIC, VORTEX, CURVE, POINT, DRAG, WIND };

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
			struct Force;

			struct Group;

			namespace LIGHT {
				enum Type { DIRECTIONAL , POINT , AREA, SPOT };

				struct Directional;
				struct Point;
				struct Area;
				struct Spot;
			}
			struct Light;

			namespace MESH {
				struct Vertex;
				struct Face;
			}
			struct Mesh;

			struct Sfx;

			struct Vfx;
		}
		struct Data;

		namespace NODE {
			enum Type { CONSTRAINT, GENERATE, PHYSICS, MODIFY, MATH, IO };

			namespace CONSTRAINT {
				enum Type { PARENT };
			}

			namespace GENERATE {
				enum Type { SUBDIVISION, TRIANGULATE, WIREFRAME, SOLIDIFY, BOOLEAN, MIRROR, ARRAY, BEVEL };
			}

			namespace PHYSICS {
				enum Type { COLLISION, CACHE, CLOTH, FLUID, RIGID, SOFT, GAS };
			}

			namespace MODIFY {
				enum Type { SURFACE_DEFORM, KEYFRAMING, SHRINKWRAP, ARMATURE, SMOOTH };

				struct Keyframing;
			}

			namespace MATH {
				enum Type { RAMP, MAP, ADD, DIV, MUL, SUB };
			}

			namespace IO {
				enum Type { ACTIVE_SCENE, OBJECT_INPUT, OBJECT_STORE, OBJECT_THIS };

				struct Active_Scene;
				struct Object_Store;
			}

			namespace PORT {
				enum Type { TRANSFORM, TEXTURE, OBJECT, SCENE, DATA, STRING, DOUBLE, BOOL, MAT2, MAT3, MAT4, VEC2, VEC3, VEC4, UINT, INT };
			}
			struct I_Port;
			struct O_Port;
		}
		struct Node;
	}
	struct Object;

	namespace MATERIAL {

		namespace NODE {
			enum Type { SHADER, MATH, IO };

			namespace SHADER {
				enum Type { COMPLETE, VOLUME };
			}

			namespace MATH {
				enum Type { DISTANCE };
			}

			namespace IO {
				enum Type { ACTIVE_SCENE, OBJECT_THIS, OBJECT_INPUT };
			}

			namespace PORT {
				enum Type { TRANSFORM, TEXTURE, FRAGMENT, OBJECT, SCENE, DATA, STRING, DOUBLE, BOOL, MAT2, MAT3, MAT4, VEC2, VEC3, VEC4, UINT, INT };
			}
			struct I_Port;
			struct O_Port;
		}
		struct Node;
	}
	struct Material;
}

namespace WORKSPACE {
	enum Type { PROPERTIES, VIEWPORT, OUTLINER, NODES, API, LOG };
}

namespace GUI {
}