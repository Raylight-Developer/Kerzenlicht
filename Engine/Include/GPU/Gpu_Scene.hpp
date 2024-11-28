#pragma once

#include "Shared.hpp"

#include "Core/File.hpp"

#include "Gpu_Instance.hpp"
#include "Gpu_Triangle.hpp"
#include "Gpu_Texture.hpp"
#include "Gpu_Camera.hpp"
#include "Gpu_Bvh.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
namespace KL {
	namespace GPU {
		struct Scene;
		struct alignas(16) Triangle;
		struct alignas(16) Texture;
		struct alignas(16) Bvh;
		struct alignas(16) Point_Light;
		struct alignas(16) Directional_Light;

		struct Scene {
			// Meshes
			vector<Object_Instance>   mesh_instances;
			vector<Triangle>          mesh_triangles;
			vector<Bvh>               mesh_blas;
			vector<Bvh>               mesh_tlas;

			vector<Texture>           textures;
			vector<uint>              texture_data;
			vector<Camera_Lens>       camera_lenses;
			vector<Point_Light>       point_lights;
			vector<Directional_Light> directional_lights;

			Scene();

			void f_init();

			void updateTextures();
			void f_update();

			void printInfo() const;
		};

		struct alignas(16) Point_Light {
			vec3 pos;
			vec1 radius;
			vec3 color;
			vec1 intentsity;
			// 32

			Point_Light(
				const vec3& pos = vec3(0.0f),
				const vec1& radius = 0U,
				const vec3& color = vec3(0.0f),
				const vec1& intentsity = 0U
			) :
				pos(pos),
				radius(radius),
				color(color),
				intentsity(intentsity)
			{}
		};

		struct alignas(16) Directional_Light {
			vec3 rotation;
			vec1 angle;
			vec3 color;
			vec1 intentsity;
			// 32

			Directional_Light(
				const vec3& rotation = vec3(0.0f),
				const vec1& angle = 0U,
				const vec3& color = vec3(0.0f),
				const vec1& intentsity = 0U
			) :
				rotation(rotation),
				angle(angle),
				color(color),
				intentsity(intentsity)
			{}
		};
	}
}