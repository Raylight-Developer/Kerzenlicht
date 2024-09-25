#pragma once

#include "Shared.hpp"

#include "Gpu_Triangle.hpp"
#include "Gpu_Bvh.hpp"

#include "GPU/Bvh_Build.hpp"

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
			vector<Triangle>          triangles;
			vector<Bvh>               bvh_nodes;
			vector<Texture>           textures;
			vector<uint>              texture_data;
			vector<Point_Light>       point_lights;
			vector<Directional_Light> directional_lights;

			//Camera                    camera;
			//vector<Material>      materials;
			//vector<Light>         lights;
			//vector<Spline_Point>  spline_controls;
			//vector<Spline>        splines;
			//vector<Curve>         curves;
			//vector<Vertex>        vertices;
			//vector<Mesh>          meshes;

			Scene();

			void f_init();

			void updateTextures();
			void f_update();

			uint64 bvhNodesSize() const;
			uint64 texturesSize() const;
			uint64 trianglesSize() const;
			uint64 textureDataSize() const;
			uint64 pointLightsSize() const;
			uint64 directionalLightsSize() const;

			void printInfo() const;
		};

		enum struct Texture_Fromat {
			RGBA8,
			RGBA16,
			RGBA32
		};

		struct alignas(16) Texture {
			uint start;
			uint width;
			uint height;
			uint format;
			// 16

			Texture(
				const uint& start = 0U,
				const uint& width = 0U,
				const uint& height = 0U,
				const uint& format = 0U
			) :
				start(start),
				width(width),
				height(height),
				format(format)
			{}
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