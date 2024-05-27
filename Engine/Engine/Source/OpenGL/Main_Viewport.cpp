#include "OpenGL/Main_Viewport.hpp"

Main_Viewport::Main_Viewport() {
	vertices = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,
	};
	faces = {
		0, 1, 2,
		2, 3, 0
	};

	runtime = 0.0;
	runframe = 0;
	resolution = uvec2(1920, 1080);
	reset = false;

	last_mouse = u_to_d(resolution) / 2.0;

	last_time = 0;
	current_time = 0;
	window_time = 0.0;
	frame_time = 0.0;

	main_vao = OPENGL::VAO();
	main_vbo = OPENGL::VBO();
	main_ebo = OPENGL::EBO();
	raw_tex =  OPENGL::FBT();
	acc_tex =  OPENGL::FBT();
	raw_fbo =  OPENGL::FBO();
	acc_fbo =  OPENGL::FBO();
	raw_fp =   OPENGL::Shader_Program("Raw");
	acc_fp =   OPENGL::Shader_Program("Acc");
	pp_fp =    OPENGL::Shader_Program("PP");
}

void Main_Viewport::recompile() {
	raw_fp.f_compile();
	acc_fp.f_compile();
	pp_fp.f_compile();

	raw_fbo.f_bind();
	raw_tex.f_resize(resolution);
	raw_fbo.f_unbind();

	acc_fbo.f_bind();
	acc_tex.f_resize(resolution);
	acc_fbo.f_unbind();

	//camera = Camera();
	reset = true;
	runframe = 0;
	runtime = glfwGetTime();
}

void Main_Viewport::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Main_Viewport* instance = static_cast<Main_Viewport*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
	instance->resolution.x = width;
	instance->resolution.y = height;
	instance->runframe = 0;
	instance->runtime = glfwGetTime();

	instance->raw_fbo.f_bind();
	instance->raw_tex.f_resize(uvec2(width, height));
	instance->raw_fbo.f_unbind();

	instance->acc_fbo.f_bind();
	instance->acc_tex.f_resize(uvec2(width, height));
	instance->acc_fbo.f_unbind();
}

void Main_Viewport::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	Main_Viewport* instance = static_cast<Main_Viewport*>(glfwGetWindowUserPointer(window));
	if (instance->keys[GLFW_MOUSE_BUTTON_RIGHT]) {
		double xoffset = xpos - instance->last_mouse.x;
		double yoffset = instance->last_mouse.y - ypos;

		instance->last_mouse = dvec2(xpos, ypos);
		instance->reset = true;
		instance->runframe = 0;
	}
}

void Main_Viewport::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	Main_Viewport* instance = static_cast<Main_Viewport*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS) {
		instance->keys[button] = true;
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			instance->last_mouse = dvec2(xpos, ypos);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	else if (action == GLFW_RELEASE) {
		instance->keys[button] = false;
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			instance->last_mouse = dvec2(xpos, ypos);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void Main_Viewport::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Main_Viewport* instance = static_cast<Main_Viewport*>(glfwGetWindowUserPointer(window));
	if (yoffset < 0) {
		instance->reset = true;
		instance->runframe = 0;
	}
	if (yoffset > 0) {
		instance->reset = true;
		instance->runframe = 0;
	}
}

void Main_Viewport::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Main_Viewport* instance = static_cast<Main_Viewport*>(glfwGetWindowUserPointer(window));
	// Input Handling
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		instance->recompile();
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		instance->reset = true;
		instance->runframe = 0;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (action == GLFW_PRESS) {
		instance->keys[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		instance->keys[key] = false;
	}
}

void Main_Viewport::init() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(resolution.x, resolution.y, "Renderer", NULL, NULL);

	//Image icon = Image();
	//if (icon.f_load("./resources/Icon.png")) {
	//	GLFWimage image_icon;
	//	image_icon.width = icon.width;
	//	image_icon.height = icon.height;
	//	image_icon.pixels = icon.data;
	//	glfwSetWindowIcon(window, 1, &image_icon);
	//}

	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();

	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	glViewport(0, 0, resolution.x, resolution.y);

	raw_fp.f_init("./resources/Shaders/Raw.glsl");
	acc_fp.f_init("./resources/Shaders/Acc.glsl");
	pp_fp.f_init("./resources/Shaders/PP.glsl");

	main_vao.f_init();
	main_vao.f_bind();
	main_vbo.f_init(vertices.data(), vertices.size() * sizeof(float));
	main_ebo.f_init(faces.data(), faces.size() * sizeof(float));
	main_vao.f_linkVBO(main_vbo, 0, 2, GL_FLOAT, 4 * sizeof(GLfloat), (void*)0);
	main_vao.f_linkVBO(main_vbo, 1, 2, GL_FLOAT, 4 * sizeof(GLfloat), (void*)(2 * sizeof(float)));

	main_vao.f_unbind();
	main_vbo.f_unbind();
	main_ebo.f_unbind();

	raw_fbo.f_init();
	raw_fbo.f_bind();
	raw_tex.f_init(resolution);
	raw_fbo.f_unbind();

	acc_fbo.f_init();
	acc_fbo.f_bind();
	acc_tex.f_init(resolution);
	acc_fbo.f_unbind();

	glClearColor(0, 0, 0, 1);
	while (!glfwWindowShouldClose(window)) {
		if (keys[GLFW_KEY_D]) {
			reset = true;
			runframe = 0;
		}
		if (keys[GLFW_KEY_A]) {
			reset = true;
			runframe = 0;
		}
		if (keys[GLFW_KEY_E] || keys[GLFW_KEY_SPACE]) {
			reset = true;
			runframe = 0;
		}
		if (keys[GLFW_KEY_Q] || keys[GLFW_KEY_LEFT_CONTROL]) {
			reset = true;
			runframe = 0;
		}
		if (keys[GLFW_KEY_W]) {
			reset = true;
			runframe = 0;
		}
		if (keys[GLFW_KEY_S]) {
			reset = true;
			runframe = 0;
		}
		current_time = clock();
		frame_time = float(current_time - last_time) / CLOCKS_PER_SEC;
		last_time = current_time;
		runtime += frame_time;
		window_time += frame_time;

		main_vao.f_bind();

		raw_fbo.f_bind();
		glClear(GL_COLOR_BUFFER_BIT);
		raw_fp.f_activate();

		glUniform1f(glGetUniformLocation(raw_fp.ID, "runtime"), GLfloat(runtime));
		glUniform1ui(glGetUniformLocation(raw_fp.ID, "runframe"), GLuint(runframe));
		glUniform2fv(glGetUniformLocation(raw_fp.ID, "resolution"), 1, value_ptr(vec2(resolution)));

		//glUniform3fv(glGetUniformLocation(raw_fp.ID, "camera_pos"), 1, value_ptr(vec3(camera.position)));
		//glUniform3fv(glGetUniformLocation(raw_fp.ID, "camera_z"), 1, value_ptr(vec3(camera.z_vector)));
		//glUniform3fv(glGetUniformLocation(raw_fp.ID, "camera_y"), 1, value_ptr(vec3(camera.y_vector)));
		glUniform1i(glGetUniformLocation(raw_fp.ID, "reset"), reset);
		acc_tex.f_bind(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(raw_fp.ID, "last_frame"), 1);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		raw_fbo.f_unbind();


		acc_fbo.f_bind();
		acc_fp.f_activate();

		glUniform1ui(glGetUniformLocation(acc_fp.ID, "runframe"), GLuint(runframe));
		glUniform1i(glGetUniformLocation(acc_fp.ID, "reset"), reset);
		raw_tex.f_bind(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(acc_fp.ID, "raw_frame"), 0);
		acc_tex.f_bind(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(acc_fp.ID, "last_frame"), 1);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		raw_tex.f_unbind();
		acc_tex.f_unbind();
		acc_fbo.f_unbind();


		pp_fp.f_activate();

		acc_tex.f_bind(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(pp_fp.ID, "acc_frame"), 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		runframe++;
		reset = false;

		if (window_time > 1.0) {
			window_time -= 1.0;
			stringstream title;
			title << "Voxel Renderer | " << 1.0 / frame_time << " Fps";
			glfwSetWindowTitle(window, title.str().c_str());
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}