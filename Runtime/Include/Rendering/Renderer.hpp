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

	vector<GLfloat> vertices;
	vector<GLuint> faces;

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

	VAO main_vao;
	VBO main_vbo;
	EBO main_ebo;

	FBT raw_tex;
	FBT acc_tex;

	FBO raw_fbo;
	FBO acc_fbo;

	Shader_Program raw_fp;
	Shader_Program acc_fp;
	Shader_Program pp_fp;

	Renderer();

	void f_init();
	void f_exit();

	void f_initGlfw();
	void f_initImGui();
	void f_systemInfo();

	void f_pipeline();
	void f_dataTransfer();

	void f_guiLoop();
	void f_gameLoop();
	void f_displayLoop();

	void f_recompile();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void cursor_position_callback(GLFWwindow* window, dvec1 xpos, dvec1 ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, dvec1 xoffset, dvec1 yoffset);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};