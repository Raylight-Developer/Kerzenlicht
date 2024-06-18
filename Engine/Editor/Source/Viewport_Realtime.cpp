#include "Viewport_Realtime.hpp"

#include "Workspaces/Viewport.hpp"
#include "Workspaces/Manager.hpp"
#include "Main_Window.hpp"

GUI::WORKSPACE::Viewport_Realtime::Viewport_Realtime(Workspace_Viewport* parent) :
	QOpenGLWindow(),
	parent(parent),
	resolution(uvec2(3840U, 2160U)),
	aspect_ratio(16.0 / 9.0)
{
	compute_shader_program = 0U;
	compute_layout = uvec3(0U);
	compute_render = 0U;

	display_shader_program = 0U;
	fullscreen_quad_VAO = 0U;
	fullscreen_quad_VBO = 0U;
	fullscreen_quad_EBO = 0U;

	triangles = vector<VIEWPORT_REALTIME::GPU_Triangle>();
	triangle_map = map<CLASS::Object*, vector<VIEWPORT_REALTIME::Triangle>>();

	fps_counter = 0;
	fps_measure = chrono::steady_clock::now();

	frame_counter = 0;
	last_delta = fps_measure;
	delta = FPS_60;

	setObjectName("Viewport_Realtime");
}

void GUI::WORKSPACE::Viewport_Realtime::f_pipeline() {
	// Pipeline Setup
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
	glCreateVertexArrays(1, &fullscreen_quad_VAO);
	glCreateBuffers(1, &fullscreen_quad_VBO);
	glCreateBuffers(1, &fullscreen_quad_EBO);

	glNamedBufferData(fullscreen_quad_VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(fullscreen_quad_EBO, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(fullscreen_quad_VAO, 0);
	glVertexArrayAttribBinding(fullscreen_quad_VAO, 0, 0);
	glVertexArrayAttribFormat(fullscreen_quad_VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(fullscreen_quad_VAO, 1);
	glVertexArrayAttribBinding(fullscreen_quad_VAO, 1, 0);
	glVertexArrayAttribFormat(fullscreen_quad_VAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(fullscreen_quad_VAO, 0, fullscreen_quad_VBO, 0, 4 * sizeof(GLfloat));
	glVertexArrayElementBuffer(fullscreen_quad_VAO, fullscreen_quad_EBO);

	// Compute Output
	glCreateTextures(GL_TEXTURE_2D, 1, &compute_render);
	glTextureParameteri(compute_render, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for raw pixels
	glTextureParameteri(compute_render, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(compute_render, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(compute_render, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(compute_render, 1, GL_RGBA8, resolution.x, resolution.y);
	glBindImageTexture(0, compute_render, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	GLuint display_vert_shader = glCreateShader(GL_VERTEX_SHADER);
	const string vertex_code = f_loadFromFile("./Resources/Shaders/Realtime_Shader.vert");
	const char* vertex_code_cstr = vertex_code.c_str();
	glShaderSource(display_vert_shader, 1, &vertex_code_cstr, NULL);
	glCompileShader(display_vert_shader);
	GLuint display_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const string fragment_code = f_loadFromFile("./Resources/Shaders/Realtime_Shader.frag");
	const char* fragment_code_cstr = fragment_code.c_str();
	glShaderSource(display_frag_shader, 1, &fragment_code_cstr, NULL);
	glCompileShader(display_frag_shader);

	display_shader_program = glCreateProgram();
	glAttachShader(display_shader_program, display_vert_shader);
	glAttachShader(display_shader_program, display_frag_shader);
	glLinkProgram(display_shader_program);

	glDeleteShader(display_vert_shader);
	glDeleteShader(display_frag_shader);

	const string compute_code = f_loadFromFile("./Resources/Shaders/Realtime_Shader.comp");
	//cerr << compute_code;
	const char* compute_code_cstr = compute_code.c_str();
	GLuint comp_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp_shader, 1, &compute_code_cstr, NULL);
	glCompileShader(comp_shader);

	compute_shader_program = glCreateProgram();
	glAttachShader(compute_shader_program, comp_shader);
	glLinkProgram(compute_shader_program);

	glDeleteShader(comp_shader);

	compute_layout = uvec3(
		d_to_u(ceil(u_to_d(resolution.x) / 32.0)),
		d_to_u(ceil(u_to_d(resolution.y) / 32.0)),
		1U
	);
}

void GUI::WORKSPACE::Viewport_Realtime::f_uploadData() {
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
	glGenBuffers(1, &triangle_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangle_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VIEWPORT_REALTIME::GPU_Triangle) * triangles.size(), triangles.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triangle_buffer);
}

void GUI::WORKSPACE::Viewport_Realtime::f_updateTick() {
	if (++fps_counter >= 60) {
		parent->timeline->info->setText("Fps: " + QString::number(60.0 / (chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now() - fps_measure).count() / 1000.0)));
		fps_measure = chrono::steady_clock::now();
		fps_counter = 0;
	}
	if (frame_counter != 0) delta = chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now() - last_delta).count() / 1000.0;
	last_delta = chrono::steady_clock::now();
	for (const CLASS::Object* object : FILE->active_scene->ptr->objects)
		if (object->nodes)
			object->nodes->exec(&delta);
	//for (auto [key, workspace] : parent->parent->parent->workspaces) // TODO optimize with callbacks or other method
	//	if (workspace->type == Workspace_Type::NODE_EDITOR)
	//		static_cast<Workspace_Node_Editor*>(workspace->workspace)->viewport->update();
	frame_counter++;
	f_updateFrame();
}

void GUI::WORKSPACE::Viewport_Realtime::f_updateFrame() {
	f_uploadData();
	update();
}

void GUI::WORKSPACE::Viewport_Realtime::f_selectObject(const dvec2& uv) {
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

void GUI::WORKSPACE::Viewport_Realtime::initializeGL() {
	initializeOpenGLFunctions();
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glViewport(0, 0, resolution.x, resolution.y);

	f_pipeline();
	last_delta = chrono::steady_clock::now();
}

void GUI::WORKSPACE::Viewport_Realtime::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT);

	f_updateTick();
	CLASS::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
	camera->f_compile(FILE->active_scene->ptr, FILE->default_camera);

	glUseProgram(compute_shader_program);
	glUniform3dv(glGetUniformLocation(compute_shader_program, "camera_pos"),  1, value_ptr(FILE->default_camera->transform.position));
	glUniform3dv(glGetUniformLocation(compute_shader_program, "camera_p_uv"), 1, value_ptr(camera->projection_center));
	glUniform3dv(glGetUniformLocation(compute_shader_program, "camera_p_u"),  1, value_ptr(camera->projection_u));
	glUniform3dv(glGetUniformLocation(compute_shader_program, "camera_p_v"),  1, value_ptr(camera->projection_v));

	glDispatchCompute(compute_layout.x, compute_layout.y, compute_layout.z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// Display Render
	glUseProgram(display_shader_program);

	glBindTextureUnit(0, compute_render);
	glUniform1d(glGetUniformLocation(display_shader_program, "aspect_ratio"), aspect_ratio);
	glUniform1i(glGetUniformLocation(display_shader_program, "render"), 0);

	glBindVertexArray(fullscreen_quad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	requestUpdate();
}

void GUI::WORKSPACE::Viewport_Realtime::resizeGL(int w, int h) {
	resolution = uvec2(w, h);
	aspect_ratio = u_to_d(resolution.x) / u_to_d(resolution.y);

	GLuint new_compute;
	glCreateTextures(GL_TEXTURE_2D, 1, &new_compute);
	glTextureParameteri(new_compute, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for raw pixels
	glTextureParameteri(new_compute, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(new_compute, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(new_compute, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(new_compute, 1, GL_RGBA8, resolution.x, resolution.y);
	glBindImageTexture(0, new_compute, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	compute_render = new_compute;

	compute_layout = uvec3(
		d_to_u(ceil(u_to_d(resolution.x) / 32.0)),
		d_to_u(ceil(u_to_d(resolution.y) / 32.0)),
		1U
	);

	glViewport(0, 0, resolution.x, resolution.y);
	//update();
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