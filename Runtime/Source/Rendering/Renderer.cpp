#include "Rendering/Renderer.hpp"

Renderer::Renderer() {
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
	frame_counter = 0;
	runframe = 0;
	resolution = uvec2(1920, 1080);
	reset = false;

	camera = Camera();

	camera_move_sensitivity = 0.15;
	camera_view_sensitivity = 0.075;
	keys = vector(348, false);
	last_mouse = dvec2(resolution) / 2.0;

	last_time = 0;
	current_time = 0;
	window_time = 0.0;
	frame_time = 0.0;

	main_vao = VAO();
	main_vbo = VBO();
	main_ebo = EBO();
	raw_tex = FBT();
	acc_tex = FBT();
	raw_fbo = FBO();
	acc_fbo = FBO();
	raw_fp = Shader_Program("Raw");
	acc_fp = Shader_Program("Acc");
	pp_fp  = Shader_Program("PP");
}


void Renderer::f_init() {
	f_initGlfw();
	f_initImGui();
	f_systemInfo();

	f_pipeline();
	f_displayLoop();
}

void Renderer::f_exit() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Renderer::f_initGlfw() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(resolution.x, resolution.y, "Runtime", NULL, NULL);

	Image icon = Image();
	if (icon.f_load("./Resources/Icon.png")) {
		GLFWimage image_icon;
		image_icon.width = icon.width;
		image_icon.height = icon.height;
		image_icon.pixels = icon.data;
		glfwSetWindowIcon(window, 1, &image_icon);
	}

	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(false); // V-Sync
	gladLoadGL();

	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
}

void Renderer::f_initImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.FontGlobalScale = 1.25f;// 2.25f;
	io.IniFilename = "./Resources/GUI_state.ini";// nullptr;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void Renderer::f_systemInfo() {
	GLint work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	cout << "Max work groups per compute shader" <<
		" x:" << work_grp_cnt[0] <<
		" y:" << work_grp_cnt[1] <<
		" z:" << work_grp_cnt[2] << endl;

	GLint work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	cout << "Max work group sizes" <<
		" x:" << work_grp_size[0] <<
		" y:" << work_grp_size[1] <<
		" z:" << work_grp_size[2] << endl;

	GLint work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	cout << "Max invocations count per work group: " << work_grp_inv << endl;

	GLint ssboMaxSize;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &ssboMaxSize);
	cout << "Maximum SSBO size per binding: " << static_cast<double>(ssboMaxSize) / static_cast<double>(1024 * 1024 * 1024) << " Gb" << endl;

	GLint maxSSBOBindings;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBindings);
	cout << "Maximum SSBO bindings: " << maxSSBOBindings << endl;

	GLint uniformBufferOffsetAlignment;
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &uniformBufferOffsetAlignment);
	cout << "SSBO struct alignment multiplier: " << uniformBufferOffsetAlignment << endl;
}

void Renderer::f_pipeline() {
	glViewport(0, 0, resolution.x , resolution.y);

	raw_fp.f_init("./Resources/Shaders/Raw.glsl");
	acc_fp.f_init("./Resources/Shaders/Acc.glsl");
	pp_fp.f_init("./Resources/Shaders/PP.glsl");

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
}

void Renderer::f_guiLoop() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::Begin("Info");
	ImGui::Text((to_string(1.0/static_cast<double>(frame_count)) + "ms").c_str());
	ImGui::Text((to_string(frame_count) + "fps").c_str());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::f_gameLoop() {
	if (keys[GLFW_KEY_D]) {
		camera.f_move(1, 0, 0, camera_move_sensitivity);
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_A]) {
		camera.f_move(-1, 0, 0, camera_move_sensitivity);
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_E] || keys[GLFW_KEY_SPACE]) {
		camera.position += dvec3(0.0, 1.0, 0.0) * camera_move_sensitivity;
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_Q] || keys[GLFW_KEY_LEFT_CONTROL]) {
		camera.position -= dvec3(0.0, 1.0, 0.0) * camera_move_sensitivity;
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_W]) {
		camera.f_move(0, 0, 1, camera_move_sensitivity);
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_S]) {
		camera.f_move(0, 0, -1, camera_move_sensitivity);
		reset = true;
		runframe = 0;
	}
}

void Renderer::f_displayLoop() {
	Texture background_tex = Texture();
	background_tex.f_init("./Resources/Bg.jpg");

	Texture blocks_tex = Texture();
	blocks_tex.f_init("./Resources/Blocks.png");

	glClearColor(0, 0, 0, 1);
	while (!glfwWindowShouldClose(window)) {
		f_gameLoop();

		current_time = clock();
		frame_time = float(current_time - last_time) / CLOCKS_PER_SEC;
		last_time = current_time;
		runtime += frame_time;
		window_time += frame_time;

		main_vao.f_bind();

		raw_fbo.f_bind();
		glClear(GL_COLOR_BUFFER_BIT);
		raw_fp.f_activate();

		glUniform1f (glGetUniformLocation(raw_fp.ID, "runtime"), GLfloat(runtime));
		glUniform1ui(glGetUniformLocation(raw_fp.ID, "runframe"), GLuint(runframe));
		glUniform2fv(glGetUniformLocation(raw_fp.ID, "resolution"), 1, value_ptr(vec2(resolution)));

		glUniform3fv(glGetUniformLocation(raw_fp.ID, "camera_pos"), 1, value_ptr(vec3(camera.position)));
		glUniform3fv(glGetUniformLocation(raw_fp.ID, "camera_z"), 1, value_ptr(vec3(camera.z_vector)));
		glUniform3fv(glGetUniformLocation(raw_fp.ID, "camera_y"), 1, value_ptr(vec3(camera.y_vector)));
		glUniform1i (glGetUniformLocation(raw_fp.ID, "reset"), reset);
		acc_tex.f_bind(GL_TEXTURE1);
		glUniform1i (glGetUniformLocation(raw_fp.ID, "last_frame"), 1);
		blocks_tex.f_bind(GL_TEXTURE3);
		glUniform1i (glGetUniformLocation(raw_fp.ID, "block_textures"), 3);
		background_tex.f_bind(GL_TEXTURE2);
		glUniform1i (glGetUniformLocation(raw_fp.ID, "environment_texture"), 2);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		raw_fbo.f_unbind();

		acc_fbo.f_bind();
		acc_fp.f_activate();

		glUniform1ui(glGetUniformLocation(acc_fp.ID, "runframe"), GLuint(runframe));
		glUniform1i (glGetUniformLocation(acc_fp.ID, "reset"), reset);
		raw_tex.f_bind(GL_TEXTURE0);
		glUniform1i (glGetUniformLocation(acc_fp.ID, "raw_frame"), 0);
		acc_tex.f_bind(GL_TEXTURE1);
		glUniform1i (glGetUniformLocation(acc_fp.ID, "last_frame"), 1);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		raw_tex.f_unbind();
		acc_tex.f_unbind();
		acc_fbo.f_unbind();


		pp_fp.f_activate();

		acc_tex.f_bind(GL_TEXTURE0);
		glUniform1i (glGetUniformLocation(pp_fp.ID, "acc_frame"), 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		frame_counter++;
		runframe++;
		reset = false;

		if (window_time > 1.0) {
			frame_count = frame_counter;
			window_time -= 1.0;
			frame_counter = 0;
		}

		f_guiLoop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Renderer::f_recompile() {
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

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
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

void Renderer::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	if (instance->keys[GLFW_MOUSE_BUTTON_RIGHT]) {
		double xoffset = xpos - instance->last_mouse.x;
		double yoffset = instance->last_mouse.y - ypos;

		instance->last_mouse = dvec2(xpos, ypos);

		instance->camera.f_rotate(xoffset * instance->camera_view_sensitivity, yoffset * instance->camera_view_sensitivity);
		instance->reset = true;
		instance->runframe = 0;
	}
}

void Renderer::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
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

void Renderer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	if (yoffset < 0) {
		instance->reset = true;
		instance->runframe = 0;
		instance->camera_move_sensitivity /= 1.1;
	}
	if (yoffset > 0) {
		instance->reset = true;
		instance->runframe = 0;
		instance->camera_move_sensitivity *= 1.1;
	}
}

void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	// Input Handling
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		instance->f_recompile();
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		instance->camera = Camera();
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