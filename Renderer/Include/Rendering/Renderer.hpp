#pragma once

#include "Shared.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "imgui-1.90/imgui.h"
#include "imgui-1.90/imgui_impl_glfw.h"
#include "imgui-1.90/imgui_impl_opengl3.h"

#include "OpenGL.hpp"

#include "Gpu_Scene.hpp"
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
		KL::Object default_camera;

		Rasterizer rasterizer;
		PathTracer pathtracer;

		dvec1 display_aspect_ratio;
		dvec1 render_aspect_ratio;
		uvec2 display_resolution;
		uvec2 render_resolution;

		uint   frame_counter;
		uint   frame_count;
		uint64 runframe;

		Mode render_mode;
		bool direct_render;
		GLuint display_filter;

		dvec1 camera_move_sensitivity;
		dvec1 camera_view_sensitivity;
		vector<bool> keys;

		dvec2 current_mouse;
		dvec2 last_mouse;

		dvec1 current_time;
		dvec1 window_time;
		dvec1 frame_time;
		dvec1 last_time;

		Renderer();

		void init();
		void exit();

		void initGlfw();
		void initImGui();
		void systemInfo();

		void f_pipeline();
		void f_recompile();

		void f_tickUpdate();

		void f_timings();
		void f_guiLoop();
		void f_gameLoop();
		void f_displayLoop();
		void f_frameUpdate();

		uvec2 f_res() const;
		dvec1 f_aspectRatio() const;

		static void glfwFramebufferSize(GLFWwindow* window, int width, int height);
		static void glfwMouseButton    (GLFWwindow* window, int button, int action, int mods);
		static void glfwCursorPos      (GLFWwindow* window, dvec1 xpos, dvec1 ypos);
		static void glfwScroll         (GLFWwindow* window, dvec1 xoffset, dvec1 yoffset);
		static void glfwKey            (GLFWwindow* window, int key, int scancode, int action, int mods);
	};
}