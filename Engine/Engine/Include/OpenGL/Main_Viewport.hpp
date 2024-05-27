#pragma once

#include "Include.hpp"

#include "OpenGL.hpp"
#include <GLFW/glfw3.h>

struct Main_Viewport {
	vector<GLfloat> vertices;
	vector<GLuint> faces;

	double runtime;
	size_t runframe;
	uvec2  resolution;
	bool   reset;

	vector<bool> keys;
	dvec2  last_mouse;

	clock_t last_time;
	clock_t current_time;
	double window_time;
	double frame_time;

	OPENGL::VAO main_vao;
	OPENGL::VBO main_vbo;
	OPENGL::EBO main_ebo;
	OPENGL::FBT raw_tex;
	OPENGL::FBT acc_tex;
	OPENGL::FBO raw_fbo;
	OPENGL::FBO acc_fbo;
	OPENGL::Shader_Program raw_fp;
	OPENGL::Shader_Program acc_fp;
	OPENGL::Shader_Program pp_fp;

	Main_Viewport();

	void recompile();
	void init();

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};