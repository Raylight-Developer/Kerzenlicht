#include "Rendering/Renderer.hpp"

KL::Renderer::Renderer() {
	window = nullptr;
	Lace* log = new Lace("\t");
	Session::getInstance().setLog(log);
	file = new KL::Render_File();
	file->f_loadAsciiFile("../Shared/Resources/Assets/Ganyu.krz");
	Session::getInstance().setFile(file);

	rasterizer = Rasterizer(this);
	pathtracer = PathTracer(this);

	frame_counter = 0;
	frame_count = 0;
	runframe = 0;

	resolution = uvec2(3840U, 2160U);
	aspect_ratio = u_to_d(resolution.x) / u_to_d(resolution.y);

	r_resolution = uvec2(2100U, 900U);
	render_aspect_ratio = u_to_d(r_resolution.x) / u_to_d(r_resolution.y);

	camera_move_sensitivity = 0.75;
	camera_view_sensitivity = 100.0;
	camera_orbit_sensitivity = 150.0;
	inputs = vector(348, false);
	current_mouse = dvec2(resolution) / 2.0;
	last_mouse = dvec2(resolution) / 2.0;

	current_time = 0.0;
	window_time = 0.0;
	frame_time = FPS_60;
	last_time = 0.0;

	render_mode = Mode::RASTERIZATION;
	direct_render = true;
	display_filter = GL_NEAREST; // GL_LINEAR for no filtering
}

void KL::Renderer::f_displayLoop() {
	while (!glfwWindowShouldClose(window)) {
		f_timings();

		if (render_mode == Mode::PATHTRACING) {
			f_inputLoop();
			f_tickUpdate();
			pathtracer.f_render();
		}
		else if (render_mode == Mode::RASTERIZATION) {
			f_inputLoop();
			f_tickUpdate();
			rasterizer.f_render();
		}

		f_frameUpdate();
		f_guiLoop();

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
}

void KL::Renderer::f_tickUpdate() {
	for (KL::Object* object : FILE->active_scene.pointer->objects) {
		if (object->node_tree) {
			object->node_tree->exec(&frame_time);
			object->cpu_update = true;
		}
	}

	if (render_mode == Mode::PATHTRACING) {
		pathtracer.f_tickUpdate();
	}
	else if (render_mode == Mode::RASTERIZATION) {
		rasterizer.f_tickUpdate();
	}
}

void KL::Renderer::f_guiLoop() {
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

void KL::Renderer::f_inputLoop() {
	if (inputs[GLFW_KEY_D]) {
		FILE->f_activeCamera()->transform.moveLocal(dvec3(1.0, 0.0, 0.0) * camera_move_sensitivity * frame_time);
	}
	if (inputs[GLFW_KEY_A]) {
		FILE->f_activeCamera()->transform.moveLocal(dvec3(-1.0, 0.0, 0.0) * camera_move_sensitivity * frame_time);
	}
	if (inputs[GLFW_KEY_E] or inputs[GLFW_KEY_SPACE]) {
		FILE->f_activeCamera()->transform.moveLocal(dvec3(0.0, 1.0, 0.0) * camera_move_sensitivity * frame_time);
	}
	if (inputs[GLFW_KEY_Q] or inputs[GLFW_KEY_LEFT_CONTROL]) {
		FILE->f_activeCamera()->transform.moveLocal(dvec3(0.0, -1.0, 0.0) * camera_move_sensitivity * frame_time);
	}
	if (inputs[GLFW_KEY_W]) {
		FILE->f_activeCamera()->transform.moveLocal(dvec3(0.0, 0.0, -1.0) * camera_move_sensitivity * frame_time);
	}
	if (inputs[GLFW_KEY_S]) {
		FILE->f_activeCamera()->transform.moveLocal(dvec3(0.0, 0.0, 1.0) * camera_move_sensitivity * frame_time);
	}
	if (inputs[GLFW_KEY_LEFT_ALT] and inputs[GLFW_MOUSE_BUTTON_LEFT]) {
		const dvec1 xoffset = (last_mouse.x - current_mouse.x) * frame_time * camera_orbit_sensitivity;
		const dvec1 yoffset = (last_mouse.y - current_mouse.y) * frame_time * camera_orbit_sensitivity;

		FILE->f_activeCamera()->transform.orbit(dvec3(0), dvec3(yoffset, xoffset, 0.0));

		last_mouse = current_mouse;
	}
	else if (inputs[GLFW_MOUSE_BUTTON_RIGHT]) {
		const dvec1 xoffset = (last_mouse.x - current_mouse.x) * frame_time * camera_view_sensitivity;
		const dvec1 yoffset = (last_mouse.y - current_mouse.y) * frame_time * camera_view_sensitivity;

		FILE->f_activeCamera()->transform.rotate(dvec3(yoffset, xoffset, 0.0));

		last_mouse = current_mouse;
	}

	if (
		inputs[GLFW_KEY_W] or
		inputs[GLFW_KEY_A] or
		inputs[GLFW_KEY_S] or
		inputs[GLFW_KEY_D] or
		inputs[GLFW_KEY_Q] or
		inputs[GLFW_KEY_E] or
		inputs[GLFW_KEY_SPACE] or
		inputs[GLFW_KEY_LEFT_CONTROL] or
		inputs[GLFW_MOUSE_BUTTON_RIGHT] or
		(inputs[GLFW_KEY_LEFT_ALT] and inputs[GLFW_MOUSE_BUTTON_LEFT])
	) {
		pathtracer.current_sample = 0;
	}
}

void KL::Renderer::f_timings() {
	current_time = glfwGetTime();
	frame_time = current_time - last_time;
	last_time = current_time;
	window_time += frame_time;
}

void KL::Renderer::f_frameUpdate() {
	frame_counter++;
	runframe++;

	if (window_time > 1.0) {
		frame_count = frame_counter;
		window_time -= 1.0;
		frame_counter = 0;
	}
}

void KL::Renderer::f_pipeline() {
	if (render_mode == Mode::PATHTRACING) {
		pathtracer.f_initialize();
	}
	else if (render_mode == Mode::RASTERIZATION) {
		rasterizer.f_initialize();
	}

	f_displayLoop();
}

void KL::Renderer::f_recompile() {
	pathtracer.f_recompile();
	rasterizer.f_recompile();
}

uvec2 KL::Renderer::f_res() const {
	if (direct_render) {
		return resolution;
	}
	return r_resolution;
}

dvec1 KL::Renderer::f_aspectRatio() const {
	if (direct_render) {
		return aspect_ratio;
	}
	return render_aspect_ratio;
}

void KL::Renderer::init() {
	initGlfw();
	initImGui();
	f_systemInfo();

	f_pipeline();
}

void KL::Renderer::exit() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void KL::Renderer::initGlfw() {
	glfwInit();

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	resolution = uvec2(mode->width, mode->height);
	aspect_ratio = u_to_d(resolution.x) / u_to_d(resolution.y);
	last_mouse = glm::dvec2(resolution) / 2.0;

	window = glfwCreateWindow(resolution.x, resolution.y, "Runtime", NULL, NULL);

	//SHADER::Texture icon = SHADER::Texture();
	//if (icon.loadFromFile("./Resources/Icon.png")) {
	//	GLFWimage image_icon;
	//	image_icon.width = icon.resolution.x;
	//	image_icon.height = icon.resolution.y;
	//	image_icon.pixels = icon.data.data();
	//	glfwSetWindowIcon(window, 1, &image_icon);
	//}

	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(true); // Frame-rate Cap
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window, glfwFramebufferSize);
	glfwSetMouseButtonCallback(window, glfwMouseButton);
	glfwSetCursorPosCallback(window, glfwCursorPos);
	glfwSetScrollCallback(window, glfwScroll);
	glfwSetKeyCallback(window, glfwKey);
}

void KL::Renderer::initImGui() {
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

void KL::Renderer::f_systemInfo() {
	GLint work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	LOG ENDL << "Max work groups per compute shader" <<
		" x:" << work_grp_cnt[0] <<
		" y:" << work_grp_cnt[1] <<
		" z:" << work_grp_cnt[2];

	GLint work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	LOG ENDL << "Max work group sizes" <<
		" x:" << work_grp_size[0] <<
		" y:" << work_grp_size[1] <<
		" z:" << work_grp_size[2];

	GLint work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	LOG ENDL << "Max invocations count per work group: " << work_grp_inv;

	GLint ssboMaxSize;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &ssboMaxSize);
	LOG ENDL << "Maximum SSBO size per binding: " << d_to_ul(round(i_to_d(ssboMaxSize) / (1024.0 * 1024.0))) << " Mb";

	GLint maxSSBOBindings;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBindings);
	LOG ENDL << "Maximum SSBO bindings: " << maxSSBOBindings;

	GLint uniformBufferOffsetAlignment;
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &uniformBufferOffsetAlignment);
	LOG ENDL << "SSBO struct alignment multiplier: " << uniformBufferOffsetAlignment;
	FLUSH;
}

void KL::Renderer::glfwFramebufferSize(GLFWwindow* window, int width, int height) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	instance->resolution.x = width;
	instance->resolution.y = height;
	instance->aspect_ratio = u_to_d(instance->resolution.x) / u_to_d(instance->resolution.y);
	instance->current_mouse = dvec2(instance->resolution) / 2.0;
	instance->last_mouse = instance->current_mouse;
	instance->runframe = 0;
	if (instance->render_mode == Mode::PATHTRACING) {
		instance->pathtracer.f_resize();
	}
	else if (instance->render_mode == Mode::RASTERIZATION) {
		instance->rasterizer.f_resize();
	}
}

void KL::Renderer::glfwMouseButton(GLFWwindow* window, int button, int action, int mods) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS) {
		instance->inputs[button] = true;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		instance->last_mouse = dvec2(xpos, ypos);

		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	else if (action == GLFW_RELEASE) {
		instance->inputs[button] = false;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		instance->last_mouse = dvec2(xpos, ypos);

		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void KL::Renderer::glfwCursorPos(GLFWwindow* window, dvec1 xpos, dvec1 ypos) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	instance->current_mouse = dvec2(xpos, ypos);
}

void KL::Renderer::glfwScroll(GLFWwindow* window, dvec1 xoffset, dvec1 yoffset) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	if (yoffset < 0) {
		FILE->f_activeCamera()->transform.moveLocal(dvec3(0.0, 0.0,  25.0) * instance->camera_move_sensitivity * instance->frame_time);
	}
	if (yoffset > 0) {
		FILE->f_activeCamera()->transform.moveLocal(dvec3(0.0, 0.0, -25.0) * instance->camera_move_sensitivity * instance->frame_time);
	}
}

void KL::Renderer::glfwKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Renderer* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	// Input Handling
	if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
		instance->pathtracer.f_recompile();
		instance->rasterizer.f_recompile();
	}
	if (key == GLFW_KEY_RIGHT  && action == GLFW_PRESS) {
		if (instance->pathtracer.data["view_layer"] < 3)
			instance->pathtracer.data["view_layer"]++;
		else
			instance->pathtracer.data["view_layer"] = 0;
	}
	if (key == GLFW_KEY_LEFT  && action == GLFW_PRESS) {
		if (instance->pathtracer.data["view_layer"] > 0)
			instance->pathtracer.data["view_layer"]--;
		else
			instance->pathtracer.data["view_layer"] = 3;
	}
	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		instance->direct_render = !instance->direct_render;
		if (instance->render_mode == Mode::PATHTRACING) {
			instance->pathtracer.f_resize();
		}
		else if (instance->render_mode == Mode::RASTERIZATION) {
			instance->rasterizer.f_resize();
		}
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		instance->pathtracer.debug = !instance->pathtracer.debug;
		if (instance->pathtracer.data["view_layer"] == 0) {
			instance->pathtracer.data["view_layer"] = 1;
		}
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		if (instance->display_filter == GL_LINEAR) {
			instance->display_filter = GL_NEAREST;
		}
		else {
			instance->display_filter = GL_LINEAR;
		}
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		if (instance->render_mode == Mode::PATHTRACING) {
			instance->render_mode = Mode::RASTERIZATION;
			instance->pathtracer.f_cleanup();
			instance->rasterizer.f_initialize();
		}
		else if (instance->render_mode == Mode::RASTERIZATION) {
			instance->render_mode = Mode::PATHTRACING;
			instance->rasterizer.f_cleanup();
			instance->pathtracer.f_initialize();
		}
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (action == GLFW_PRESS) {
		instance->inputs[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		instance->inputs[key] = false;
	}
}