#pragma once

#include "Shared.hpp"

#include "OpenGL.hpp"

#include "Gpu_Scene.hpp"
#include "Core/Render_File.hpp"

namespace KL {
	struct Renderer;
}

namespace KL {
	struct PathTracer {
		Renderer* renderer;

		GPU_Scene* gpu_data;

		bool reset;
		bool debug;
		uint sample;

		uvec2 resolution;
		uvec2 r_resolution;
		vec1 aspect_ratio;
		vec1 r_aspect_ratio;

		unordered_map<string, GLuint> data;

		PathTracer(Renderer* renderer = nullptr);

		void f_initialize();
		void f_tickUpdate();

		void f_recompile();
		void f_cleanup();
		void f_resize();

		void f_render();
	};
}