#include "Rendering/Renderer.hpp"

Renderer::Renderer() {
	window = nullptr;

	runtime = 0.0;
	frame_counter = 0;
	frame_count = 0;
	runframe = 0;

	display_resolution = uvec2(3840U, 2160U);
	display_aspect_ratio = u_to_d(display_resolution.x) / u_to_d(display_resolution.y);

	render_resolution = uvec2(1920U, 800U);
	render_aspect_ratio = u_to_d(render_resolution.x) / u_to_d(render_resolution.y);

	recompile = false;
	reset = false;

	camera = Camera();

	camera_move_sensitivity = 0.15;
	camera_view_sensitivity = 0.075;
	keys = vector(348, false);
	last_mouse = dvec2(display_resolution) / 2.0;

	last_time = 0;
	current_time = 0;
	window_time = 0.0;
	frame_time = 0.0;
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

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	display_resolution = uvec2(mode->width, mode->height);
	display_aspect_ratio = u_to_d(display_resolution.x) / u_to_d(display_resolution.y);
	last_mouse = glm::dvec2(display_resolution) / 2.0;

	window = glfwCreateWindow(display_resolution.x, display_resolution.y, "Runtime", NULL, NULL);

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
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

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

	//GLint totalMemoryKB = 0;
	//glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemoryKB);
	//GLint currentMemoryKB = 0;
	//glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &currentMemoryKB);
	//
	//cout << "Total GPU Memory: " << totalMemoryKB << " KB\n";
	//cout << "Available GPU Memory: " << currentMemoryKB << " KB\n";

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
	glViewport(0, 0, display_resolution.x , display_resolution.y);
}

void Renderer::f_dataTransfer() {
	CPU_Scene cpu_data = f_parseCPUData();
	GPU_Scene gpu_data = f_parseGPUData(cpu_data);
	gpu_data.print();
	
	GLuint gpu_compute_program;
	GLuint material_buffer;
	GLuint light_buffer;
	GLuint vertex_buffer;
	GLuint triangle_buffer;
	GLuint mesh_buffer;
	GLuint spline_handle_buffer;
	GLuint spline_buffer;
	GLuint curve_buffer;
	GLuint bvh_buffer;
	
	GLint ssboMaxSize;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &ssboMaxSize);
	gpu_data.printInfo(ssboMaxSize);
	
	glGenBuffers(1, &material_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, material_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Material) * gpu_data.materials.size(), gpu_data.materials.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, material_buffer);
	
	glGenBuffers(1, &light_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, light_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Light) * gpu_data.lights.size(), gpu_data.lights.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, light_buffer);
	
	glGenBuffers(1, &spline_handle_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, spline_handle_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Spline_Point) * gpu_data.spline_controls.size(), gpu_data.spline_controls.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, spline_handle_buffer);
	
	glGenBuffers(1, &spline_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, spline_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Spline) * gpu_data.splines.size(), gpu_data.splines.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, spline_buffer);
	
	glGenBuffers(1, &curve_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, curve_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Curve) * gpu_data.curves.size(), gpu_data.curves.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, curve_buffer);
	
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Vertex) * gpu_data.vertices.size(), gpu_data.vertices.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, vertex_buffer);
	
	glGenBuffers(1, &triangle_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangle_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Triangle) * gpu_data.triangles.size(), gpu_data.triangles.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, triangle_buffer);
	
	glGenBuffers(1, &mesh_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mesh_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Mesh) * gpu_data.meshes.size(), gpu_data.meshes.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, mesh_buffer);
	
	glGenBuffers(1, &bvh_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvh_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_BVH) * gpu_data.bvh_nodes.size(), gpu_data.bvh_nodes.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, bvh_buffer);
}

void Renderer::f_guiLoop() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::Begin("Info");
	ImGui::Text((to_string(1.0/static_cast<double>(frame_count)) + "ms").c_str());
	ImGui::Text((to_string(frame_count) + "fps").c_str());
	ImGui::Text((to_string(runframe) + "frames").c_str());
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

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 4 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);

	// Compute Output
	GLuint render_result;
	glCreateTextures(GL_TEXTURE_2D, 1, &render_result);
	glTextureParameteri(render_result, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for raw pixels
	glTextureParameteri(render_result, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(render_result, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(render_result, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(render_result, 1, GL_RGBA32F, render_resolution.x, render_resolution.y);

	// Previous Frame
	GLuint previous_frame;
	glCreateTextures(GL_TEXTURE_2D, 1, &previous_frame);
	glTextureParameteri(previous_frame, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for raw pixels
	glTextureParameteri(previous_frame, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(previous_frame, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(previous_frame, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(previous_frame, 1, GL_RGBA32F, render_resolution.x, render_resolution.y);

	GLuint accumulate_program = f_computeShaderProgram("Accumulation");
	GLuint compute_program = f_computeShaderProgram("Render");
	GLuint post_program = f_fragmentShaderProgram("Post");

	const uvec3 compute_layout = uvec3(
		static_cast<uint32>(ceil(static_cast<vec1>(render_resolution.x) / 32.0f)),
		static_cast<uint32>(ceil(static_cast<vec1>(render_resolution.y) / 32.0f)),
		1U
	);

	glBindVertexArray(VAO);
	while (!glfwWindowShouldClose(window)) {
		f_gameLoop();

		current_time = clock();
		frame_time = float(current_time - last_time) / CLOCKS_PER_SEC;
		last_time = current_time;
		runtime += frame_time;
		window_time += frame_time;

		glUseProgram(compute_program);
		glUniform1ui(glGetUniformLocation(compute_program, "frame_count"), runframe);
		glUniform2ui(glGetUniformLocation(compute_program, "resolution"), render_resolution.x, render_resolution.y);
		glUniform1f (glGetUniformLocation(compute_program, "runtime"), d_to_f(runtime));
		glBindImageTexture(0, render_result, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glDispatchCompute(compute_layout.x, compute_layout.y, compute_layout.z);
		
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(accumulate_program);
		glUniform1ui(glGetUniformLocation(accumulate_program, "frame_count"), runframe);
		glUniform2ui(glGetUniformLocation(accumulate_program, "resolution"), render_resolution.x, render_resolution.y);
		glBindImageTexture(0, render_result , 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, previous_frame, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glDispatchCompute(compute_layout.x, compute_layout.y, compute_layout.z);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(post_program);
		glUniform1f (glGetUniformLocation(post_program, "display_aspect_ratio"), d_to_f(display_aspect_ratio));
		glUniform1f (glGetUniformLocation(post_program, "render_aspect_ratio"), d_to_f(render_aspect_ratio));
		glUniform1i(glGetUniformLocation(post_program, "render"), 0);
		glBindTextureUnit(0, render_result);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		std::swap(render_result, previous_frame);

		frame_counter++;
		runframe++;
		if (reset) reset = false;

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
	reset = true;
	runframe = 0;
	runtime = glfwGetTime();
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
	instance->display_resolution.x = width;
	instance->display_resolution.y = height;
	instance->display_aspect_ratio = u_to_d(instance->display_resolution.x) / u_to_d(instance->display_resolution.y);
	instance->runframe = 0;
	instance->runtime = glfwGetTime();
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