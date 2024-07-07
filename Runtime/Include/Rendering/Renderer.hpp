#pragma once

#include "Include.hpp"

#include "GLFW/glfw3.h"
#include "External/imgui-1.90/imgui.h"
#include "External/imgui-1.90/backends/imgui_impl_glfw.h"
#include "External/imgui-1.90/backends/imgui_impl_opengl3.h"

#include "OpenGL.hpp"

#include "Gpu_Scene.hpp"
#include "Camera.hpp"

struct Renderer {
	GLFWwindow* window;

	dvec1 display_aspect_ratio;
	dvec1 render_aspect_ratio;
	uvec2 display_resolution;
	uvec2 render_resolution;

	dvec1  runtime;
	uint   frame_counter;
	uint   frame_count;
	uint64 runframe;

	bool recompile;
	bool reset;

	Camera camera;

	dvec1 camera_move_sensitivity;
	dvec1 camera_view_sensitivity;
	vector<bool> keys;
	dvec2  last_mouse;

	clock_t last_time;
	clock_t current_time;
	dvec1  window_time;
	dvec1  frame_time;

	Renderer();

	void init();
	void exit();

	void initGlfw();
	void initImGui();
	void systemInfo();

	void pipeline();
	void dataTransfer();

	void guiLoop();
	void gameLoop();
	void displayLoop();

	void recompileShader();

	static void framebufferSize(GLFWwindow* window, int width, int height);
	static void cursorPos(GLFWwindow* window, dvec1 xpos, dvec1 ypos);
	static void mouseButton(GLFWwindow* window, int button, int action, int mods);
	static void scroll(GLFWwindow* window, dvec1 xoffset, dvec1 yoffset);
	static void key(GLFWwindow* window, int key, int scancode, int action, int mods);
};