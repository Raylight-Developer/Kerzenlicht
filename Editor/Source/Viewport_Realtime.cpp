#include "Viewport_Realtime.hpp"

#include "Workspaces/Viewport.hpp"
#include "Workspaces/Manager.hpp"
#include "Main_Window.hpp"

#include "OpenGL.hpp"

GUI::WORKSPACE::Viewport_Realtime::Viewport_Realtime(Workspace_Viewport* parent) :
	QObject(),
	parent(parent)
{
	window = nullptr;

	frame_counter = 0;
	frame_count = 0;
	runframe = 0;

	display_resolution = uvec2(3840U, 2160U);
	display_aspect_ratio = u_to_d(display_resolution.x) / u_to_d(display_resolution.y);

	render_resolution = uvec2(2100U, 900U);
	render_aspect_ratio = u_to_d(render_resolution.x) / u_to_d(render_resolution.y);

	current_time = 0.0;
	window_time = 0.0;
	frame_time = FPS_60;
	last_time = 0.0;

	vector<VIEWPORT_REALTIME::GPU_Triangle> triangles;
	map<CLASS::Object*, vector<VIEWPORT_REALTIME::Triangle>> triangle_map;
}

void GUI::WORKSPACE::Viewport_Realtime::init() {
	initGlfw();

	pipeline();
	displayLoop();
}

void GUI::WORKSPACE::Viewport_Realtime::exit() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void GUI::WORKSPACE::Viewport_Realtime::initGlfw() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	display_resolution = uvec2(mode->width, mode->height);
	display_aspect_ratio = u_to_d(display_resolution.x) / u_to_d(display_resolution.y);

	window = glfwCreateWindow(display_resolution.x, display_resolution.y, "Runtime", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(false); // V-Sync
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window, framebufferSize);
}

void GUI::WORKSPACE::Viewport_Realtime::pipeline() {
	glad_glViewport(0, 0, display_resolution.x , display_resolution.y);
}

void GUI::WORKSPACE::Viewport_Realtime::renderTick() {
	for (const CLASS::Object* object : FILE->active_scene->ptr->objects)
		if (object->nodes)
			object->nodes->exec(&frame_time);
	dataTransfer();
}

void GUI::WORKSPACE::Viewport_Realtime::dataTransfer() {
	triangles.clear();
	triangle_map.clear();
	for (CLASS::Object* object : FILE->active_scene->ptr->objects) {
		object->f_compileMatrix();
		vector <VIEWPORT_REALTIME::Triangle>  temp;
		if (object->data->type == CLASS::OBJECT::DATA::Type::MESH) {
			const CLASS::OBJECT::DATA::Mesh* mesh = object->data->getMesh();
			for (const CLASS::OBJECT::DATA::MESH::Face* face : mesh->faces) {
				const dvec4 vert4_a = object->transform_matrix * dvec4(face->vertices[0]->position, 1.0);
				const dvec4 vert4_b = object->transform_matrix * dvec4(face->vertices[1]->position, 1.0);
				const dvec4 vert4_c = object->transform_matrix * dvec4(face->vertices[2]->position, 1.0);
				const dvec3 vert_a = dvec3(vert4_a.x, vert4_a.y, vert4_a.z) / vert4_a.w;
				const dvec3 vert_b = dvec3(vert4_b.x, vert4_b.y, vert4_b.z) / vert4_b.w;
				const dvec3 vert_c = dvec3(vert4_c.x, vert4_c.y, vert4_c.z) / vert4_c.w;
				triangles.push_back(VIEWPORT_REALTIME::GPU_Triangle(vert_a, vert_b, vert_c));
				temp.push_back(VIEWPORT_REALTIME::Triangle(vert_a, vert_b, vert_c));
			}
		}
		else if (object->data->type == CLASS::OBJECT::DATA::Type::GROUP) {
			const CLASS::OBJECT::DATA::Group* group = object->data->getGroup();
			for (CLASS::Object* sub_object : group->objects) {
				sub_object->f_compileMatrix();
				sub_object->transform_matrix *= object->transform_matrix;
				if (sub_object->data->type == CLASS::OBJECT::DATA::Type::MESH) {
					const CLASS::OBJECT::DATA::Mesh* mesh = sub_object->data->getMesh();
					for (const CLASS::OBJECT::DATA::MESH::Face* face : mesh->faces) {
						const dvec4 vert4_a = sub_object->transform_matrix * dvec4(face->vertices[0]->position, 1.0);
						const dvec4 vert4_b = sub_object->transform_matrix * dvec4(face->vertices[1]->position, 1.0);
						const dvec4 vert4_c = sub_object->transform_matrix * dvec4(face->vertices[2]->position, 1.0);
						const dvec3 vert_a = dvec3(vert4_a.x, vert4_a.y, vert4_a.z) / vert4_a.w;
						const dvec3 vert_b = dvec3(vert4_b.x, vert4_b.y, vert4_b.z) / vert4_b.w;
						const dvec3 vert_c = dvec3(vert4_c.x, vert4_c.y, vert4_c.z) / vert4_c.w;
						triangles.push_back(VIEWPORT_REALTIME::GPU_Triangle(vert_a, vert_b, vert_c));
						temp.push_back(VIEWPORT_REALTIME::Triangle(vert_a, vert_b, vert_c));
					}
				}
			}
		}
		triangle_map[object] = temp;
	}

	GLuint triangle_buffer;
	glad_glGenBuffers(1, &triangle_buffer);
	glad_glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangle_buffer);
	glad_glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VIEWPORT_REALTIME::GPU_Triangle) * triangles.size(), triangles.data(), GL_DYNAMIC_DRAW);
	glad_glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triangle_buffer);
}

void GUI::WORKSPACE::Viewport_Realtime::selectObject(const dvec2& uv) { // TODO fix slight missalignment
	CLASS::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
	camera->f_compile(FILE->active_scene->ptr, FILE->default_camera);
	const VIEWPORT_REALTIME::Ray ray = VIEWPORT_REALTIME::Ray(
		d_to_f(FILE->default_camera->transform.position),
		d_to_f(normalize(
			camera->projection_center
			+ (camera->projection_u * uv.x)
			+ (camera->projection_v * uv.y)
			- FILE->default_camera->transform.position
		))
	);

	dvec1 t_dist = MAX_DIST;
	dvec1 closest_dist = MAX_DIST;
	CLASS::Object* closest = nullptr;
	for (const auto& [object, triangles] : triangle_map) {
		for (const VIEWPORT_REALTIME::Triangle& tri : triangles) {
			if (VIEWPORT_REALTIME::f_rayTriangleIntersection(ray, tri, t_dist)) {
				if (t_dist < closest_dist && t_dist > EPSILON) {
					closest_dist = t_dist;
					closest = object;
				}
			}
		}
	}
	if (closest) {
		FILE->active_object->set(closest);
	}
	else {
		FILE->active_object->set(nullptr);
	}
}

void GUI::WORKSPACE::Viewport_Realtime::displayLoop() {
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

	GLuint compute_program = computeShaderProgram("Realtime_Shader");
	GLuint post_program = fragmentShaderProgram("Realtime_Shader");

	const uvec3 compute_layout = uvec3(
		d_to_u(ceil(u_to_d(render_resolution.x) / 32.0)),
		d_to_u(ceil(u_to_d(render_resolution.y) / 32.0)),
		1U
	);

	dataTransfer();

	// Compute Output
	GLuint raw_render_layer = renderLayer(render_resolution, GL_RGBA8);

	glad_glBindVertexArray(VAO);
	while (!glfwWindowShouldClose(window)) {
		current_time = glfwGetTime();
		frame_time = current_time - last_time;
		last_time = current_time;
		window_time += frame_time;

		renderTick();
		CLASS::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
		camera->f_compile(FILE->active_scene->ptr, FILE->default_camera);

		glad_glUseProgram(compute_program);
		glad_glUniform3dv(glad_glGetUniformLocation(compute_program, "camera_pos" ), 1, value_ptr(FILE->default_camera->transform.position));
		glad_glUniform3dv(glad_glGetUniformLocation(compute_program, "camera_p_uv"), 1, value_ptr(camera->projection_center ));
		glad_glUniform3dv(glad_glGetUniformLocation(compute_program, "camera_p_u" ), 1, value_ptr(camera->projection_u ));
		glad_glUniform3dv(glad_glGetUniformLocation(compute_program, "camera_p_v" ), 1, value_ptr(camera->projection_v ));

		glad_glBindImageTexture(0, raw_render_layer         , 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

		glad_glDispatchCompute(compute_layout.x, compute_layout.y, compute_layout.z);

		glad_glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glad_glUseProgram(post_program);
		glad_glClear(GL_COLOR_BUFFER_BIT);
		glad_glUniform1f(glad_glGetUniformLocation(post_program, "display_aspect_ratio"), d_to_f(display_aspect_ratio));
		glad_glUniform1f(glad_glGetUniformLocation(post_program, "render_aspect_ratio"), d_to_f(render_aspect_ratio));
		bindRenderLayer(post_program, 0, raw_render_layer , "raw_render_layer");
		glad_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		frame_counter++;
		runframe++;

		if (window_time > 1.0) {
			frame_count = frame_counter;
			window_time -= 1.0;
			frame_counter = 0;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void GUI::WORKSPACE::Viewport_Realtime::framebufferSize(GLFWwindow* window, int width, int height) {
	Viewport_Realtime* instance = static_cast<Viewport_Realtime*>(glfwGetWindowUserPointer(window));
	glad_glViewport(0, 0, width, height);
	instance->display_resolution.x = width;
	instance->display_resolution.y = height;
	instance->display_aspect_ratio = u_to_d(instance->display_resolution.x) / u_to_d(instance->display_resolution.y);
	instance->runframe = 0;
}

bool GUI::WORKSPACE::VIEWPORT_REALTIME::f_rayTriangleIntersection(const Ray& ray, const Triangle& tri, dvec1& ray_length) {
	const dvec3 v1v0 = tri.v1 - tri.v0;
	const dvec3 v2v0 = tri.v2 - tri.v0;
	const dvec3 rov0 = ray.origin - tri.v0;

	const dvec3 n = cross(v1v0, v2v0);
	const dvec3 q = cross(rov0, ray.direction);
	const dvec1 d = 1.0 / dot(ray.direction, n);
	const dvec1 u = d * dot(-q, v2v0);
	const dvec1 v = d * dot(q, v1v0);
	const dvec1 t = d * dot(-n, rov0);

	if (u < 0.0 || v < 0.0 || (u + v) > 1.0) return false;
	ray_length = t;
	return true;
}