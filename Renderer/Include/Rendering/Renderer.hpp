#pragma once

#include "Shared.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "OpenGL.hpp"

#include "Core/Render_File.hpp"

#include "Rendering/Rasterizer.hpp"
#include "Rendering/PathTracer.hpp"

namespace KL {
	enum struct Mode {
		PATHTRACING,
		RASTERIZATION
	};
	struct Renderer {
		GLFWwindow* window;

		Render_File* file;

		Rasterizer rasterizer;
		PathTracer pathtracer;

		vec1  aspect_ratio;
		vec1  render_aspect_ratio;
		uvec2 resolution;
		uvec2 render_resolution;

		uint   frame_counter;
		uint   frame_count;
		uint64 runframe;

		Mode render_mode;
		bool direct_render;
		GLuint display_filter;

		dvec1 camera_move_sensitivity;
		dvec1 camera_view_sensitivity;
		dvec1 camera_orbit_sensitivity;
		vector<bool> inputs;

		dvec2 current_mouse;
		dvec2 last_mouse;

		dvec1 current_time;
		dvec1 window_time;
		dvec1 frame_time;
		dvec1 last_time;

		Renderer();

		void f_init();
		void f_exit();

		void f_initGlfw();
		void f_initImGui();
		void f_systemInfo();

		void f_pipeline();
		void f_recompile();
		void f_resize(const uint& width, const uint& height);

		void f_tickUpdate();

		void f_timings();
		void f_guiLoop();
		void f_inputLoop();
		void f_displayLoop();
		void f_frameUpdate();

		static void glfwFramebufferSize(GLFWwindow* window, int width, int height);
		static void glfwMouseButton    (GLFWwindow* window, int button, int action, int mods);
		static void glfwCursorPos      (GLFWwindow* window, dvec1 xpos, dvec1 ypos);
		static void glfwScroll         (GLFWwindow* window, dvec1 xoffset, dvec1 yoffset);
		static void glfwKey            (GLFWwindow* window, int key, int scancode, int action, int mods);
	};
}