﻿#include "Workspaces/Viewport.hpp"

#include "Workspaces/Manager.hpp"

//#define PATH_TRACING true

GUI::WORKSPACE::Timeline::Timeline(Workspace_Viewport* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::LeftToRight),
	parent(parent)
{
	slider = new GUI::Slider(this);
	slider->setRange(0, 360);

	info = new GUI::Label(this);
	info->setText("FPS: 0 | Frame: 0 | Delta: 0.016ms");

	GUI::Value_Input* current_frame = new GUI::Value_Input(this);
	current_frame->setText("0");
	QIntValidator* validator = new QIntValidator(slider->minimum(), slider->maximum(), current_frame);
	current_frame->setValidator(validator);

	connect(slider, &GUI::Slider::valueChanged, [this, current_frame](int value) {
		current_frame->setText(QString::number(value));
		FILE->active_scene->pointer->current_frame = value;

		if (this->parent->viewport) this->parent->viewport->f_tickUpdate();
	});
	connect(current_frame, &GUI::Value_Input::returnPressed, [this, current_frame]() {
		slider->setValue(current_frame->text().toInt());
	});
	connect(current_frame, &GUI::Value_Input::textChanged, [this, current_frame](const QString& text) {
		if (text.toDouble() > slider->maximum())
			current_frame->setText(QString::number(slider->maximum()));
		else if (text.toDouble() < slider->minimum())
			current_frame->setText(QString::number(slider->minimum()));
	});

	addWidget(info);
	addWidget(current_frame);
	addWidget(slider);
}

GUI::WORKSPACE::Workspace_Viewport::Workspace_Viewport(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	f_systemInfo();

	viewport = new Viewport(this);
	QWidget* container = QWidget::createWindowContainer(viewport);

	container->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
	container->setContentsMargins(0, 0, 0, 0);

	timeline = new Timeline(this);

	GUI::Splitter* splitter = new GUI::Splitter(this, true);
	splitter->addWidget(container);
	splitter->addWidget(timeline);
	addWidget(splitter);
}

GUI::WORKSPACE::Workspace_Viewport::~Workspace_Viewport() {
}

void GUI::WORKSPACE::Workspace_Viewport::f_systemInfo() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);

	LOG ENDL ANSI_B << "[System]" ANSI_RESET;
	LOG ENDL ANSI_B << "  [RAM]" ANSI_RESET;
	LOG ENDL << "    " << d_to_u((status.ullTotalPhys / (1024 * 1024 * 1024))) << " GB - " << status.ullTotalPhys / (1024 * 1024) << " MB";
	LOG ENDL ANSI_B << "  [CPU]" ANSI_RESET;
	LOG ENDL << "    " << thread::hardware_concurrency() << " Threads";
	LOG ENDL ANSI_B << "  [GPU]" ANSI_RESET;
	FLUSH;
}


GUI::WORKSPACE::Viewport::Viewport(Workspace_Viewport* parent) :
	QOpenGLWindow(),
	parent(parent),
	gpu_data(new KL::GPU_Scene()),

	frame_counter(0),
	frame_count(0),
	runframe(0),

	display_resolution(uvec2(3840U, 2160U)),
	display_aspect_ratio(u_to_d(display_resolution.x) / u_to_d(display_resolution.y)),

	render_scale(1.0),

	render_resolution(d_to_u(u_to_d(display_resolution)* render_scale)),
	render_aspect_ratio(u_to_d(render_resolution.x) / u_to_d(render_resolution.y)),

	reset(false),
	debug(false),

	gl_data({}),

	window_time(0.0),
	frame_time(FPS_60),

	view_layer(0)
{
	setObjectName("Viewport_Realtime");
}

void GUI::WORKSPACE::Viewport::f_pipeline() {
	#ifdef PATH_TRACING
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
		gl_data["vao"] = 0;
		GLuint VBO, EBO;
		glCreateVertexArrays(1, &gl_data["vao"]);
		glCreateBuffers(1, &VBO);
		glCreateBuffers(1, &EBO);

		glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

		GLuint VAO = gl_data["vao"];
		glEnableVertexArrayAttrib (VAO, 0);
		glVertexArrayAttribBinding(VAO, 0, 0);
		glVertexArrayAttribFormat (VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);

		glEnableVertexArrayAttrib (VAO, 1);
		glVertexArrayAttribBinding(VAO, 1, 0);
		glVertexArrayAttribFormat (VAO, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

		glVertexArrayVertexBuffer (VAO, 0, VBO, 0, 4 * sizeof(GLfloat));
		glVertexArrayElementBuffer(VAO, EBO);

		// Compute Output
		gl_data["accumulation_render_layer"] = renderLayer(render_resolution);
		gl_data["normal_render_layer"]       = renderLayer(render_resolution);
		gl_data["bvh_render_layer"]          = renderLayer(render_resolution);
		gl_data["raw_render_layer"]          = renderLayer(render_resolution);

		{
			auto [compiled, id] = computeShaderProgram("Compute");
			gl_data["compute_program"] = id;
		}
		{
			auto [compiled, id] = fragmentShaderProgram("Display");
			gl_data["display_program"] = id;
		}

		gl_data["compute_layout_x"] = d_to_u(ceil(u_to_d(render_resolution.x) / 32.0));
		gl_data["compute_layout_y"] = d_to_u(ceil(u_to_d(render_resolution.y) / 32.0));
		gl_data["compute_layout_z"] = 1U;

		glBindVertexArray(VAO);
		gpu_data->updateTextures();
		gpu_data->printInfo();
	#else
		glEnable(GL_DEPTH_TEST);
		FILE->default_camera->data->getCamera()->updateFocalAngle();
		auto [compiled, id] = fragmentShaderProgram("Rasterizer");
		gl_data["raster_program"] = id;
	#endif
}

void GUI::WORKSPACE::Viewport::f_uploadData() {
	#ifdef PATH_TRACING
		triangles.clear();
		triangle_map.clear();
		for (KL::Object* object : FILE->active_scene->pointer->objects) {
			object->f_compileMatrix();
			vector <VIEWPORT_REALTIME::Triangle>  temp;
			if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
				const KL::OBJECT::DATA::Mesh* mesh = object->data->getMesh();
				for (const KL::OBJECT::DATA::MESH::Face* face : mesh->faces) {
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
			else if (object->data->type == KL::OBJECT::DATA::Type::GROUP) {
				const KL::OBJECT::DATA::Group* group = object->data->getGroup();
				for (KL::Object* sub_object : group->objects) {
					sub_object->f_compileMatrix();
					sub_object->transform_matrix *= object->transform_matrix;
					if (sub_object->data->type == KL::OBJECT::DATA::Type::MESH) {
						const KL::OBJECT::DATA::Mesh* mesh = sub_object->data->getMesh();
						for (const KL::OBJECT::DATA::MESH::Face* face : mesh->faces) {
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
	#else
	#endif
}

void GUI::WORKSPACE::Viewport::f_tickUpdate() {
	if (FILE->active_scene->pointer) {
		for (KL::Object* object : FILE->active_scene->pointer->objects) {
			if (object and object->node_tree) {
				object->node_tree->exec(&frame_time);
				object->cpu_update = true;
			}
		}
	}

	#ifdef PATH_TRACING
		gpu_data->f_tickUpdate();

		GLint ssboMaxSize;
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &ssboMaxSize);

		glDeleteBuffers(1, &gl_data["ssbo 5"]);
		glDeleteBuffers(1, &gl_data["ssbo 6"]);
		glDeleteBuffers(1, &gl_data["ssbo 7"]);
		glDeleteBuffers(1, &gl_data["ssbo 8"]);

		gl_data["ssbo 5"] = ssboBinding(5, ul_to_u(gpu_data->trianglesSize()  ), gpu_data->triangles.data());
		gl_data["ssbo 6"] = ssboBinding(6, ul_to_u(gpu_data->bvhNodesSize()   ), gpu_data->bvh_nodes.data());
		gl_data["ssbo 7"] = ssboBinding(7, ul_to_u(gpu_data->texturesSize()   ), gpu_data->textures.data());
		gl_data["ssbo 8"] = ssboBinding(8, ul_to_u(gpu_data->textureDataSize()), gpu_data->texture_data.data());
	#else
	{
		triangle_map.clear();
		for (KL::Object* object : FILE->active_scene->pointer->objects) {
			if (object->cpu_update) {
				object->cpu_update = false;
				object->f_compileMatrix();
				vector<vec1>* location = &gl_triangle_cache[uptr(object)];
				location->clear();

				vector <VIEWPORT_REALTIME::Triangle>  temp;

				if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
					KL::OBJECT::DATA::Mesh* mesh = object->data->getMesh();
					mat3 normal_matrix = mat3(glm::transpose(glm::inverse(object->transform_matrix)));
					for (KL::OBJECT::DATA::MESH::Face* face : mesh->faces) {
						auto tri = KL::OBJECT::DATA::Mesh::faceToArray(face, mesh, object->transform_matrix, normal_matrix);
						location->insert(location->end(), tri.begin(), tri.end());

						const dvec4 vert4_a = object->transform_matrix * dvec4(face->vertices[0]->position, 1.0);
						const dvec4 vert4_b = object->transform_matrix * dvec4(face->vertices[1]->position, 1.0);
						const dvec4 vert4_c = object->transform_matrix * dvec4(face->vertices[2]->position, 1.0);
						const dvec3 vert_a = dvec3(vert4_a.x, vert4_a.y, vert4_a.z) / vert4_a.w;
						const dvec3 vert_b = dvec3(vert4_b.x, vert4_b.y, vert4_b.z) / vert4_b.w;
						const dvec3 vert_c = dvec3(vert4_c.x, vert4_c.y, vert4_c.z) / vert4_c.w;
						temp.push_back(VIEWPORT_REALTIME::Triangle(vert_a, vert_b, vert_c));
					}
				}
				else if (object->data->type == KL::OBJECT::DATA::Type::GROUP) {
					const KL::OBJECT::DATA::Group* group = object->data->getGroup();
					for (KL::Object* sub_object : group->objects) {
						sub_object->f_compileMatrix();
						sub_object->transform_matrix *= object->transform_matrix;
						mat3 normal_matrix = mat3(glm::transpose(glm::inverse(sub_object->transform_matrix)));
						if (sub_object->data->type == KL::OBJECT::DATA::Type::MESH) {
							KL::OBJECT::DATA::Mesh* mesh = sub_object->data->getMesh();
							for (KL::OBJECT::DATA::MESH::Face* face : mesh->faces) {
								auto tri = KL::OBJECT::DATA::Mesh::faceToArray(face, mesh, sub_object->transform_matrix, normal_matrix);
								location->insert(location->end(), tri.begin(), tri.end());

								const dvec4 vert4_a = sub_object->transform_matrix * dvec4(face->vertices[0]->position, 1.0);
								const dvec4 vert4_b = sub_object->transform_matrix * dvec4(face->vertices[1]->position, 1.0);
								const dvec4 vert4_c = sub_object->transform_matrix * dvec4(face->vertices[2]->position, 1.0);
								const dvec3 vert_a = dvec3(vert4_a.x, vert4_a.y, vert4_a.z) / vert4_a.w;
								const dvec3 vert_b = dvec3(vert4_b.x, vert4_b.y, vert4_b.z) / vert4_b.w;
								const dvec3 vert_c = dvec3(vert4_c.x, vert4_c.y, vert4_c.z) / vert4_c.w;
								temp.push_back(VIEWPORT_REALTIME::Triangle(vert_a, vert_b, vert_c));
							}
						}
					}
				}
				triangle_map[object] = temp;
			}
		}
		gl_triangles = f_flattenMap(gl_triangle_cache);

		glDeleteVertexArrays(1, &gl_data["vao"]);
		glDeleteBuffers(1, &gl_data["vbo"]);

		gl_data["vao"] = 0;
		gl_data["vbo"] = 0;

		glGenVertexArrays(1, &gl_data["vao"]);
		glGenBuffers(1, &gl_data["vbo"]);

		glBindVertexArray(gl_data["vao"]);
		glBindBuffer(GL_ARRAY_BUFFER, gl_data["vbo"]);

		glBufferData(GL_ARRAY_BUFFER, gl_triangles.size() * sizeof(vec1), gl_triangles.data(), GL_DYNAMIC_DRAW);

		// Vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(vec1), (void*)0);
		glEnableVertexAttribArray(0);

		// Vertex Normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(vec1), (void*)(3 * sizeof(vec1)));
		glEnableVertexAttribArray(1);
		//
		//// Vertex UVs
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(vec1), (void*)(6 * sizeof(vec1)));
		glEnableVertexAttribArray(2);
	}
	#endif
}

void GUI::WORKSPACE::Viewport::f_selectObject(const dvec2& uv) { // TODO fix slight missalignment
	#ifdef PATH_TRACING
		KL::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
		camera->compile(FILE->active_scene->pointer, FILE->default_camera);
		const VIEWPORT_REALTIME::Ray ray = VIEWPORT_REALTIME::Ray(
			d_to_f(FILE->default_camera->transform.position),
			d_to_f(normalize(
				camera->projection_uv
				+ (camera->projection_u * uv.x)
				+ (camera->projection_v * uv.y)
				- FILE->default_camera->transform.position
			))
		);

		dvec1 t_dist = MAX_DIST;
		dvec1 closest_dist = MAX_DIST;
		KL::Object* closest = nullptr;
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
	#else
	#endif
}

void GUI::WORKSPACE::Viewport::f_recompileShaders() {
	#ifdef PATH_TRACING
	{
		{
			auto [compiled, id] = computeShaderProgram("Compute");
			if (compiled) {
				glDeleteProgram(gl_data["compute_program"]);
				gl_data["compute_program"] = id;
			}
		}
		{
			auto [compiled, id] = fragmentShaderProgram("Display");
			if (compiled) {
				glDeleteProgram(gl_data["display_program"]);
				gl_data["display_program"] = id;
			}
		}
	}
	#else
	{
		auto [compiled, id] = fragmentShaderProgram("Rasterizer");
		if (compiled) {
			glDeleteProgram(gl_data["raster_program"]);
			gl_data["raster_program"] = id;
		}
	}
	#endif
}

void GUI::WORKSPACE::Viewport::initializeGL() {
	initializeOpenGLFunctions();
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glViewport(0, 0, render_resolution.x, render_resolution.y);

	f_pipeline();
	start_time = chrono::high_resolution_clock::now();
}

void GUI::WORKSPACE::Viewport::paintGL() {

	current_time = chrono::high_resolution_clock::now();
	frame_time = chrono::duration<double>(current_time - last_time).count();
	last_time = current_time;
	window_time += frame_time;

	f_tickUpdate();

	#ifdef PATH_TRACING
	{
		glClear(GL_COLOR_BUFFER_BIT);
		const GLuint compute_program = gl_data["compute_program"];
		glUseProgram(compute_program);
		glUniform1ui(glGetUniformLocation(compute_program, "frame_count" ), ul_to_u(runframe));
		glUniform1f (glGetUniformLocation(compute_program, "aspect_ratio"), d_to_f(render_aspect_ratio));
		glUniform1f (glGetUniformLocation(compute_program, "current_time"), d_to_f(chrono::duration<double>(current_time - start_time).count()));
		glUniform2ui(glGetUniformLocation(compute_program, "resolution"  ), render_resolution.x, render_resolution.y);
		glUniform1ui(glGetUniformLocation(compute_program, "reset"), static_cast<GLuint>(reset));
		glUniform1ui(glGetUniformLocation(compute_program, "debug"), static_cast<GLuint>(debug));

		glUniform1ui(glGetUniformLocation(compute_program, "ray_bounces"), 1);
		glUniform1ui(glGetUniformLocation(compute_program, "samples_per_pixel"), 1);

		KL::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
		camera->compile(FILE->active_scene->pointer, FILE->default_camera);
		glUniform3fv(glGetUniformLocation(compute_program, "camera_pos" ), 1, value_ptr(d_to_f(FILE->default_camera->transform.position)));
		glUniform3fv(glGetUniformLocation(compute_program, "camera_p_uv"), 1, value_ptr(d_to_f(camera->projection_uv)));
		glUniform3fv(glGetUniformLocation(compute_program, "camera_p_u" ), 1, value_ptr(d_to_f(camera->projection_u)));
		glUniform3fv(glGetUniformLocation(compute_program, "camera_p_v" ), 1, value_ptr(d_to_f(camera->projection_v)));
		glBindImageTexture(0, gl_data["accumulation_render_layer"], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, gl_data["raw_render_layer"         ], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(2, gl_data["bvh_render_layer"         ], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(3, gl_data["normal_render_layer"      ], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glDispatchCompute(gl_data["compute_layout_x"], gl_data["compute_layout_y"], gl_data["compute_layout_z"]);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		const GLuint display_program = gl_data["display_program"];
		glUseProgram(display_program);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniform1f(glGetUniformLocation(display_program, "display_aspect_ratio"), d_to_f(display_aspect_ratio));
		glUniform1f(glGetUniformLocation(display_program, "render_aspect_ratio"), d_to_f(render_aspect_ratio));
		glUniform1ui(glGetUniformLocation(display_program, "view_layer"), view_layer);
		glUniform1ui(glGetUniformLocation(display_program, "debug"), static_cast<GLuint>(debug));
		bindRenderLayer(display_program, 0, gl_data["accumulation_render_layer"], "accumulation_render_layer");
		bindRenderLayer(display_program, 1, gl_data["raw_render_layer"]         , "raw_render_layer"         );
		bindRenderLayer(display_program, 2, gl_data["bvh_render_layer"]         , "bvh_render_layer"         );
		bindRenderLayer(display_program, 3, gl_data["normal_render_layer"]      , "normal_render_layer"      );
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	#else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const GLuint raster_program = gl_data["raster_program"];
		const GLuint vao = gl_data["vao"];
		glUseProgram(raster_program);

		KL::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
		glUniform3fv(glGetUniformLocation(raster_program, "camera_pos" ), 1, value_ptr(d_to_f(FILE->default_camera->transform.position)));
		glUniformMatrix4fv(glGetUniformLocation(raster_program, "view_mat"), 1, GL_FALSE, value_ptr(camera->glViewMatrix(FILE->default_camera, render_aspect_ratio)));

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, ul_to_u(gl_triangles.size() / 8));
	}
	#endif

	frame_counter++;
	runframe++;
	if (reset) reset = false;

	if (window_time > 1.0) {
		frame_count = frame_counter;
		window_time = 0.0;
		frame_counter = 0;

		string selected = "None";
		if (FILE->active_object->pointer) {
			selected = FILE->active_object->pointer->name;
		}
		parent->timeline->info->setText("FPS: " + QString::number(frame_count) + " | Frame: 0 | Delta: " + QString::number(frame_time) + "ms | Selected: " + QString::fromStdString(selected));
	}
	requestUpdate();
}

void GUI::WORKSPACE::Viewport::resizeGL(int w, int h) {
	display_resolution = uvec2(i_to_u(w), i_to_u(h));
	display_aspect_ratio = u_to_d(display_resolution.x) / u_to_d(display_resolution.y);

	render_resolution = d_to_u(u_to_d(display_resolution) * render_scale);
	render_aspect_ratio = u_to_d(render_resolution.x) / u_to_d(render_resolution.y);

	glViewport(0, 0, render_resolution.x, render_resolution.y);
	#ifdef PATH_TRACING
	{
		// Compute Output
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &gl_data["accumulation_render_layer"]);
		glDeleteTextures(1, &gl_data["normal_render_layer"]);
		glDeleteTextures(1, &gl_data["bvh_render_layer"]);
		glDeleteTextures(1, &gl_data["raw_render_layer"]); // TODO Fix all other potential OpenGL memory Leaks

		gl_data["accumulation_render_layer"] = renderLayer(render_resolution);
		gl_data["normal_render_layer"]       = renderLayer(render_resolution);
		gl_data["bvh_render_layer"]          = renderLayer(render_resolution);
		gl_data["raw_render_layer"]          = renderLayer(render_resolution);

		gl_data["compute_layout_x"] = d_to_u(ceil(u_to_d(render_resolution.x) / 32.0));
		gl_data["compute_layout_y"] = d_to_u(ceil(u_to_d(render_resolution.y) / 32.0));

	}
	#endif
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

GLuint GUI::WORKSPACE::Viewport::renderLayer(const uvec2& resolution) {
	GLuint ID;
	glCreateTextures(GL_TEXTURE_2D, 1, &ID);
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(ID, 1, GL_RGBA32F, resolution.x, resolution.y);
	return ID;
}

tuple<bool, GLuint> GUI::WORKSPACE::Viewport::fragmentShaderProgram(const string& file_path) {
	GLuint shader_program = glCreateShader(GL_VERTEX_SHADER);

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	const string vertex_code = loadFromFile("./Resources/Shaders/" + file_path + ".vert");
	const char* vertex_code_cstr = vertex_code.c_str();
	glShaderSource(vert_shader, 1, &vertex_code_cstr, NULL);
	glCompileShader(vert_shader);

	if (!checkShaderCompilation(vert_shader, vertex_code)) {
		return tuple(false, 0);
	}

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const string fragment_code = loadFromFile("./Resources/Shaders/" + file_path + ".frag");
	const char* fragment_code_cstr = fragment_code.c_str();
	glShaderSource(frag_shader, 1, &fragment_code_cstr, NULL);
	glCompileShader(frag_shader);

	if (!checkShaderCompilation(frag_shader, fragment_code)) {
		return tuple(false, 0);
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	checkProgramLinking(shader_program);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return tuple(true, shader_program);
}

tuple<bool, GLuint> GUI::WORKSPACE::Viewport::computeShaderProgram(const string& file_path) {
	GLuint shader_program;
	string compute_code = preprocessShader("./Resources/Shaders/" + file_path + ".comp");
	compute_code = KL::Shader::f_compileShaders(compute_code);
	writeToFile("./Resources/Shaders/" + file_path + "_Compiled.comp", compute_code);
	const char* compute_code_cstr = compute_code.c_str();
	GLuint comp_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp_shader, 1, &compute_code_cstr, NULL);
	glCompileShader(comp_shader);

	if (!checkShaderCompilation(comp_shader, compute_code)) {
		return tuple(false, 0);
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, comp_shader);
	glLinkProgram(shader_program);

	checkProgramLinking(shader_program);

	glDeleteShader(comp_shader);

	return tuple(true, shader_program);
}

void GUI::WORKSPACE::Viewport::bindRenderLayer(const GLuint& program_id, const GLuint& unit, const GLuint& id, const string& name) {
	glUniform1i(glGetUniformLocation(program_id, name.c_str()), unit);
	glBindTextureUnit(unit, id);
}

bool GUI::WORKSPACE::Viewport::checkShaderCompilation(const GLuint& shader, const string& shader_code) {
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
		LOG ENDL ENDL ANSI_R << "[OpenGL]" ANSI_RESET << " Shader Compilation Failed: "; FLUSH;
		printShaderErrorWithContext(shader_code, infoLog);
		return false;
	}
	return true;
}

bool GUI::WORKSPACE::Viewport::checkProgramLinking(const GLuint& program) {
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
		LOG ENDL ENDL ANSI_R << "[OpenGL]" ANSI_RESET << " Program Linking Failed: " << infoLog; FLUSH;
		return false;
	}
	return true;
}

void GUI::WORKSPACE::Viewport::printShaderErrorWithContext(const string& shaderSource, const string& errorLog) {
	LOG += 1;
	size_t pos = errorLog.find('(');
	if (pos == string::npos) {
		LOG ENDL << "Error: Unable to parse error log."; FLUSH;
		LOG -= 1;
		return;
	}

	size_t endPos = errorLog.find(')', pos);
	if (endPos == string::npos) {
		LOG ENDL << "Error: Unable to parse error log."; FLUSH;
		LOG -= 1;
		return;
	}

	uint64 lineNumber = str_to_ul(errorLog.substr(pos + 1, endPos - pos - 1));

	Tokens lines = f_split(shaderSource, "\n");

	uint64 startLine = max(0ULL, lineNumber - 4);
	uint64 endLine = min(lines.size(), lineNumber + 3);

	for (uint64 i = startLine; i < endLine; ++i) {
		LOG ENDL << (i + 1) << ": " << lines[i];
		if (i == lineNumber - 1) {
			LOG ENDL ANSI_R << "^-- Error here: " ANSI_RESET << errorLog;
		}
	}
	LOG -= 1;
	LOG ENDL ENDL;
	FLUSH;
}