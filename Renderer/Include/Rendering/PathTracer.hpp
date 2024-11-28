#pragma once

#include "Shared.hpp"

#include "OpenGL.hpp"

#include "Gpu/Gpu_Scene.hpp"
#include "Core/Render_File.hpp"

namespace KL {
	struct Renderer;
}

namespace KL {
	struct PathTracer {
		Renderer* renderer;

		GPU::Scene* gpu_data;

		bool debug;
		uint current_sample;
		uint view_layer;

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