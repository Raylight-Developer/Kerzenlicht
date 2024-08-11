#include "Rendering/Renderer.hpp"

Renderer::Renderer() {
	window = nullptr;
	Lace* log = new Lace();
	Session::getInstance().setLog(log);
	file = CLASS::Render_File();
	file.f_loadAsciiFile("./Resources/Ganyu.krz");
	Session::getInstance().setFile(&file);

	frame_counter = 0;
	frame_count = 0;
	runframe = 0;

	display_resolution = uvec2(3840U, 2160U);
	display_aspect_ratio = u_to_d(display_resolution.x) / u_to_d(display_resolution.y);

	render_resolution = uvec2(2100U, 900U);
	render_aspect_ratio = u_to_d(render_resolution.x) / u_to_d(render_resolution.y);

	recompile = false;
	reset = false;
	debug = false;

	camera = Render_Camera();

	camera_move_sensitivity = 0.75;
	camera_view_sensitivity = 2.5;
	keys = vector(348, false);
	current_mouse = dvec2(display_resolution) / 2.0;
	last_mouse = dvec2(display_resolution) / 2.0;

	current_time = 0.0;
	window_time = 0.0;
	frame_time = FPS_60;
	last_time = 0.0;

	view_layer = 0;
}

void Renderer::init() {
	initGlfw();
	initImGui();
	systemInfo();

	pipeline();
	displayLoop();
}

void Renderer::exit() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Renderer::initGlfw() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	display_resolution = uvec2(mode->width, mode->height);
	display_aspect_ratio = u_to_d(display_resolution.x) / u_to_d(display_resolution.y);
	last_mouse = glm::dvec2(display_resolution) / 2.0;

	window = glfwCreateWindow(display_resolution.x, display_resolution.y, "Runtime", NULL, NULL);

	Image icon = Image();
	if (icon.init("./Resources/Icon.png")) {
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
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window, framebufferSize);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, cursorPos);
	glfwSetScrollCallback(window, scroll);
	glfwSetKeyCallback(window, key);
}

void Renderer::initImGui() {
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

void Renderer::systemInfo() {
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

	GLint uboMaxSize;
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE , &uboMaxSize);
	cout << "Maximum UBO size: " << static_cast<dvec1>(uboMaxSize) / (1024.0 * 1024.0) << " Mb" << endl;

	GLint ssboMaxSize;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &ssboMaxSize);
	cout << "Maximum SSBO size per binding: " << static_cast<dvec1>(ssboMaxSize) / (1024.0 * 1024.0) << " Mb" << endl;

	GLint maxSSBOBindings;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBindings);
	cout << "Maximum SSBO bindings: " << maxSSBOBindings << endl;

	GLint uniformBufferOffsetAlignment;
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &uniformBufferOffsetAlignment);
	cout << "SSBO struct alignment multiplier: " << uniformBufferOffsetAlignment << endl;
}

void Renderer::pipeline() {
	glViewport(0, 0, display_resolution.x , display_resolution.y);
}

void Renderer::tickUpdate() {
	for (const CLASS::Object* object : FILE->active_scene->ptr->objects) {
		if (object->node_tree) {
			object->node_tree->exec(&frame_time);
		}
	}

	GPU_Scene gpu_data = GPU_Scene(&file);
	
	GLint ssboMaxSize;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &ssboMaxSize);
	gpu_data.printInfo(ssboMaxSize);

	GLuint triangle_buffer;
	GLuint bvh_buffer;
	GLuint texture_buffer;
	GLuint texture_data_buffer;

	// SSBOs
	glGenBuffers(1, &triangle_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangle_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Triangle) * gpu_data.triangles.size(), gpu_data.triangles.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, triangle_buffer);

	glGenBuffers(1, &bvh_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvh_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_BVH) * gpu_data.bvh_nodes.size(), gpu_data.bvh_nodes.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, bvh_buffer);

	glGenBuffers(1, &texture_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, texture_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Texture) * gpu_data.textures.size(), gpu_data.textures.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, texture_buffer);

	glGenBuffers(1, &texture_data_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, texture_data_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint) * gpu_data.texture_data.size(), gpu_data.texture_data.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, texture_data_buffer);
}

void Renderer::guiLoop() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::Begin("Info");
	ImGui::Text((to_string(1.0 / u_to_d(frame_count)) + "ms").c_str());
	ImGui::Text((to_string(frame_count) + "fps").c_str());
	ImGui::Text((to_string(runframe) + "frames").c_str());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::gameLoop() {
	if (keys[GLFW_KEY_D]) {
		camera.move(-1.0, 0.0, 0.0, camera_move_sensitivity * frame_time);
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_A]) {
		camera.move(1.0, 0.0, 0.0, camera_move_sensitivity * frame_time);
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_E] || keys[GLFW_KEY_SPACE]) {
		camera.move(0.0, 1.0, 0.0, camera_move_sensitivity * frame_time);
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_Q] || keys[GLFW_KEY_LEFT_CONTROL]) {
		camera.move(0.0, -1.0, 0.0, camera_move_sensitivity * frame_time);
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_W]) {
		camera.move(0.0, 0.0, 1.0, camera_move_sensitivity * frame_time);
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_KEY_S]) {
		camera.move(0.0, 0.0, -1.0, camera_move_sensitivity * frame_time);
		reset = true;
		runframe = 0;
	}
	if (keys[GLFW_MOUSE_BUTTON_RIGHT]) {
		const dvec1 xoffset =   (last_mouse.x - current_mouse.x) * frame_time;
		const dvec1 yoffset = - (last_mouse.y - current_mouse.y) * frame_time;

		camera.rotate(xoffset * camera_view_sensitivity, yoffset * camera_view_sensitivity);
		reset = true;
		runframe = 0;

		last_mouse = current_mouse;
	}
}

void Renderer::displayLoop() {
	const GLfloat vertices[16] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
	};
	const GLuint indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	// Display Quad
	GLuint VAO, VBO, EBO;
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib (VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat (VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib (VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat (VAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

	glVertexArrayVertexBuffer (VAO, 0, VBO, 0, 4 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);

	GLuint compute_program = computeShaderProgram("Render");
	GLuint post_program = fragmentShaderProgram("Post");

	const uvec3 compute_layout = uvec3(
		d_to_u(ceil(u_to_d(render_resolution.x) / 32.0)),
		d_to_u(ceil(u_to_d(render_resolution.y) / 32.0)),
		1U
	);

	tickUpdate();

	// Compute Output
	GLuint accumulation_render_layer = renderLayer(render_resolution);
	GLuint raw_render_layer = renderLayer(render_resolution);
	GLuint bvh_render_layer = renderLayer(render_resolution);
	GLuint normal_render_layer = renderLayer(render_resolution);

	glBindVertexArray(VAO);
	while (!glfwWindowShouldClose(window)) {
		current_time = glfwGetTime();
		frame_time = current_time - last_time;
		last_time = current_time;
		window_time += frame_time;

		gameLoop();

		glUseProgram(compute_program);
		glUniform1ui(glGetUniformLocation(compute_program, "frame_count"), ul_to_u(runframe));
		glUniform1f (glGetUniformLocation(compute_program, "aspect_ratio"), d_to_f(render_aspect_ratio));
		glUniform1f (glGetUniformLocation(compute_program, "current_time"), d_to_f(current_time));
		glUniform2ui(glGetUniformLocation(compute_program, "resolution"), render_resolution.x, render_resolution.y);
		glUniform1ui(glGetUniformLocation(compute_program, "reset"), static_cast<GLuint>(reset));
		glUniform1ui(glGetUniformLocation(compute_program, "debug"), static_cast<GLuint>(debug));

		glUniform1ui(glGetUniformLocation(compute_program, "ray_bounces"), 1U);
		glUniform1ui(glGetUniformLocation(compute_program, "samples_per_pixel"), 1U);

		camera.compile();
		glUniform3fv(glGetUniformLocation(compute_program, "camera_projection_center") , 1, value_ptr(d_to_f(camera.projection_center)));
		glUniform3fv(glGetUniformLocation(compute_program, "camera_projection_u"), 1, value_ptr(d_to_f(camera.projection_u)));
		glUniform3fv(glGetUniformLocation(compute_program, "camera_projection_v"), 1, value_ptr(d_to_f(camera.projection_v)));
		glUniform3fv(glGetUniformLocation(compute_program, "camera_pos"), 1, value_ptr(d_to_f(camera.transform.position)));

		glBindImageTexture(0, accumulation_render_layer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, raw_render_layer         , 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(2, bvh_render_layer         , 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(3, normal_render_layer      , 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		//glBindImageTexture(4, tex.ID, 0, GL_FALSE      , 0, GL_READ_ONLY, GL_RGBA8);

		glDispatchCompute(compute_layout.x, compute_layout.y, compute_layout.z);
		
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(post_program);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniform1f (glGetUniformLocation(post_program, "display_aspect_ratio"), d_to_f(display_aspect_ratio));
		glUniform1f (glGetUniformLocation(post_program, "render_aspect_ratio"), d_to_f(render_aspect_ratio));
		glUniform1ui(glGetUniformLocation(post_program, "view_layer"), view_layer);
		glUniform1ui(glGetUniformLocation(post_program, "debug"), static_cast<GLuint>(debug));
		bindRenderLayer(post_program, 0, accumulation_render_layer, "accumulation_render_layer");
		bindRenderLayer(post_program, 1, raw_render_layer         , "raw_render_layer"         );
		bindRenderLayer(post_program, 2, bvh_render_layer         , "bvh_render_layer"         );
		bindRenderLayer(post_program, 3, normal_render_layer      , "normal_render_layer"      );
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		frame_counter++;
		runframe++;
		if (reset) reset = false;
		if (recompile) {
			compute_program = computeShaderProgram("Render");
			post_program = fragmentShaderProgram("Post");
			recompile = false;
		}

		if (window_time > 1.0) {
			frame_count = frame_counter;
			window_time -= 1.0;
			frame_counter = 0;
		}

		guiLoop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Renderer::framebufferSize(GLFWwindow* window, int width, int height) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
	instance->display_resolution.x = width;
	instance->display_resolution.y = height;
	instance->display_aspect_ratio = u_to_d(instance->display_resolution.x) / u_to_d(instance->display_resolution.y);
	instance->current_mouse = dvec2(instance->display_resolution) / 2.0;
	instance->last_mouse = instance->current_mouse;
	instance->runframe = 0;
}

void Renderer::cursorPos(GLFWwindow* window, double xpos, double ypos) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	instance->current_mouse = dvec2(xpos, ypos);
}

void Renderer::mouseButton(GLFWwindow* window, int button, int action, int mods) {
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

void Renderer::scroll(GLFWwindow* window, double xoffset, double yoffset) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	if (yoffset < 0) {
		instance->reset = true;
		instance->runframe = 0;
		instance->camera_move_sensitivity /= 1.05;
	}
	if (yoffset > 0) {
		instance->reset = true;
		instance->runframe = 0;
		instance->camera_move_sensitivity *= 1.05;
	}
}

void Renderer::key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	// Input Handling
	if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
		instance->recompile = true;
	}
	if (key == GLFW_KEY_V  && action == GLFW_PRESS) {
		instance->debug = !instance->debug;
	}
	if (key == GLFW_KEY_RIGHT  && action == GLFW_PRESS) {
		if (instance->view_layer < 3)
			instance->view_layer++;
		else
			instance->view_layer = 0;
	}
	if (key == GLFW_KEY_LEFT  && action == GLFW_PRESS) {
		if (instance->view_layer > 0)
			instance->view_layer--;
		else
			instance->view_layer = 3;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		instance->camera = Render_Camera();
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