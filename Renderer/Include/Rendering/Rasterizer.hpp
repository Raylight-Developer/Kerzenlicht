#pragma once

#include "Shared.hpp"

#include "OpenGL.hpp"

#include "Gpu_Scene.hpp"
#include "Core/Render_File.hpp"

namespace KL {
	struct Renderer;
}

namespace KL {
	struct Rasterizer {
		Renderer* renderer;

		uvec2 resolution;
		uvec2 r_resolution;
		vec1 aspect_ratio;
		vec1 r_aspect_ratio;

		unordered_map<string, GLuint> data;
		unordered_map<uint64, vector<vec1>> gl_triangle_cache;
		unordered_map<KL::Object*, unordered_map<string, GLuint>> gl_data;

		Rasterizer(Renderer* renderer = nullptr);

		void f_initialize();
		void f_tickUpdate();

		void f_recompile();
		void f_cleanup();
		void f_resize();

		void f_render();
		void f_renderMesh(const GLuint raster_program, KL::Object* object);
		void f_renderGroup(const GLuint raster_program, KL::Object* object);
		void f_renderCurve(const GLuint raster_program, KL::Object* object);
	};
}