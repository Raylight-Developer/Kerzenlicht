#include "Workspaces/Viewport.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Timeline::Timeline(Workspace_Viewport* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::LeftToRight),
	parent(parent)
{
	slider = new GUI::Slider(this);
	slider->setRange(0, 240);

	info = new GUI::Label(this);
	info->setText("FPS: 0 Frame: ");
	info->setFixedWidth(64);

	GUI::Value_Input* current_frame = new GUI::Value_Input(this);
	current_frame->setText("0");
	QIntValidator* validator = new QIntValidator(slider->minimum(), slider->maximum(), current_frame);
	current_frame->setValidator(validator);

	connect(slider, &GUI::Slider::valueChanged, [this, current_frame](int value) {
		current_frame->setText(QString::number(value));
		FILE->active_scene->ptr->current_frame = value;

		if (this->parent->viewport_gpu_renderer) this->parent->viewport_gpu_renderer->f_updateFrame();
		if (this->parent->viewport_realtime) this->parent->viewport_realtime->f_updateFrame();
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

	addWidget(slider);
	addWidget(current_frame);
	addWidget(info);
}

GUI::WORKSPACE::Workspace_Viewport::Workspace_Viewport(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	f_systemInfo();

	const Type type = Type::REALTIME;
	switch (type) {
		case Type::REALTIME: {
			viewport_realtime = new Viewport_Realtime(this);
			container = QWidget::createWindowContainer(viewport_realtime);
			break;
		}
		case Type::CPU: {
			viewport_cpu_renderer = new Viewport_CPU_Renderer(this);
			container = QWidget::createWindowContainer(viewport_cpu_renderer);
			break;
		}
		case Type::GPU: {
			viewport_gpu_renderer = new Viewport_GPU_Renderer(this);
			container = QWidget::createWindowContainer(viewport_gpu_renderer);
			break;
		}
	}
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

	*LOG
	<< ENDL << "┌"
	<< ENDL << "│  System"
	<< ENDL << "│    Total physical RAM: " << uint(ceil((double)(status.ullTotalPhys / (1024.0 * 1024.0 * 1024.0)))) << "GB - " << status.ullTotalPhys / (1024 * 1024) << "MB"
	<< ENDL << "│  CPU"
	<< ENDL << "│    Threads: " << thread::hardware_concurrency()
	<< ENDL << "│  GPU"
	<< ENDL << "└";
}

GUI::WORKSPACE::Viewport_CPU_Renderer::Viewport_CPU_Renderer(Workspace_Viewport* parent) :
	QOpenGLWindow(),
	parent(parent),
	display_resolution(uvec2(3840U, 2160U)),
	render_resolution(uvec2(3840U, 2160U)),
	display_aspect_ratio(16.0 / 9.0),
	render_aspect_ratio(16.0 / 9.0),
	cpu_renderer(nullptr)
{
	display_shader_program = 0U;
	cpu_render_result = 0U;
	fullscreen_quad_VAO = 0U;
	fullscreen_quad_VBO = 0U;
	fullscreen_quad_EBO = 0U;
}

void GUI::WORKSPACE::Viewport_CPU_Renderer::f_pipeline() {
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
	glCreateTextures(GL_TEXTURE_2D, 1, &cpu_render_result);
	glTextureParameteri(cpu_render_result, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for raw pixels
	glTextureParameteri(cpu_render_result, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(cpu_render_result, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(cpu_render_result, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(cpu_render_result, 1, GL_RGBA32F, render_resolution.x, render_resolution.y);
	glBindImageTexture(0, cpu_render_result, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	GLuint display_vert_shader = glCreateShader(GL_VERTEX_SHADER);
	const string vertex_code = f_loadFromFile("./Resources/Shaders/CPU_Shader.vert");
	const char* vertex_code_cstr = vertex_code.c_str();
	glShaderSource(display_vert_shader, 1, &vertex_code_cstr, NULL);
	glCompileShader(display_vert_shader);
	GLuint display_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const string fragment_code = f_loadFromFile("./Resources/Shaders/CPU_Shader.frag");
	const char* fragment_code_cstr = fragment_code.c_str();
	glShaderSource(display_frag_shader, 1, &fragment_code_cstr, NULL);
	glCompileShader(display_frag_shader);

	display_shader_program = glCreateProgram();
	glAttachShader(display_shader_program, display_vert_shader);
	glAttachShader(display_shader_program, display_frag_shader);
	glLinkProgram(display_shader_program);

	glDeleteShader(display_vert_shader);
	glDeleteShader(display_frag_shader);
}

void GUI::WORKSPACE::Viewport_CPU_Renderer::f_cpuDisplayRender() {
	// Display Render
	glUseProgram(display_shader_program);

	glBindTextureUnit(0, cpu_render_result);

	glUniform1f(glGetUniformLocation(display_shader_program, "display_aspect_ratio"), d_to_f(display_aspect_ratio));
	glUniform1f(glGetUniformLocation(display_shader_program, "render_aspect_ratio"),  d_to_f(render_aspect_ratio));
	glUniform1i(glGetUniformLocation(display_shader_program, "render"), 0);

	glBindVertexArray(fullscreen_quad_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void GUI::WORKSPACE::Viewport_CPU_Renderer::f_cpuUpdateRender(const float* display_pixmap) {
	glGenTextures(1, &cpu_render_result);
	glBindTexture(GL_TEXTURE_2D, cpu_render_result);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, render_resolution.x, render_resolution.y, 0, GL_RGBA, GL_FLOAT, display_pixmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	update();
}

void GUI::WORKSPACE::Viewport_CPU_Renderer::initializeGL() {
	initializeOpenGLFunctions();
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glViewport(0, 0, display_resolution.x, display_resolution.y);

	f_pipeline();

	cpu_renderer = new CPU_Renderer(this);
	connect(cpu_renderer, &CPU_Renderer::f_cpuUpdateRender, this, &Viewport_CPU_Renderer::f_cpuUpdateRender, Qt::ConnectionType::QueuedConnection);
	cpu_renderer->start();
}

void GUI::WORKSPACE::Viewport_CPU_Renderer::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT);
	f_cpuDisplayRender();
}

void GUI::WORKSPACE::Viewport_CPU_Renderer::resizeGL(int w, int h) {
	display_resolution = uvec2(w, h);
	display_aspect_ratio = d_to_f(u_to_d(display_resolution.x) / u_to_d(display_resolution.y));
	glViewport(0, 0, display_resolution.x, display_resolution.y);
}

GUI::WORKSPACE::Viewport_GPU_Renderer::Viewport_GPU_Renderer(Workspace_Viewport* parent) :
	QOpenGLWindow(),
	parent(parent),
	downsampling(2.0),
	display_resolution(uvec2(3840U, 2160U)),
	render_resolution(d_to_u(u_to_d(display_resolution) / downsampling)),
	display_aspect_ratio(16.0 / 9.0),
	render_aspect_ratio(16.0 / 9.0)
{
	compute_shader_program = 0U;
	compute_layout = uvec3(0U);
	compute_render = 0U;

	display_shader_program = 0U;
	fullscreen_quad_VAO = 0U;
	fullscreen_quad_VBO = 0U;
	fullscreen_quad_EBO = 0U;

	triangles = vector<GPU_Triangle>();
}

void GUI::WORKSPACE::Viewport_GPU_Renderer::f_pipeline() {
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
	glTextureStorage2D (compute_render, 1, GL_RGBA8, render_resolution.x, render_resolution.y);
	glBindImageTexture(0, compute_render, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	GLuint display_vert_shader = glCreateShader(GL_VERTEX_SHADER);
	const string vertex_code = f_loadFromFile("./Resources/Shaders/GPU_Shader.vert");
	const char* vertex_code_cstr = vertex_code.c_str();
	glShaderSource(display_vert_shader, 1, &vertex_code_cstr, NULL);
	glCompileShader(display_vert_shader);
	GLuint display_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const string fragment_code = f_loadFromFile("./Resources/Shaders/GPU_Shader.frag");
	const char* fragment_code_cstr = fragment_code.c_str();
	glShaderSource(display_frag_shader, 1, &fragment_code_cstr, NULL);
	glCompileShader(display_frag_shader);

	display_shader_program = glCreateProgram();
	glAttachShader(display_shader_program, display_vert_shader);
	glAttachShader(display_shader_program, display_frag_shader);
	glLinkProgram(display_shader_program);

	glDeleteShader(display_vert_shader);
	glDeleteShader(display_frag_shader);

	const string compute_code = f_loadFromFile("./Resources/Shaders/GPU_Shader.comp");
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
		d_to_u(ceil(u_to_d(render_resolution.x) / 32.0)),
		d_to_u(ceil(u_to_d(render_resolution.y) / 32.0)),
		1U
	);
}

void GUI::WORKSPACE::Viewport_GPU_Renderer::f_uploadData() {
	triangles.clear();
	//for (CLASS::Object* object : file->objects) {
	//	object->f_compileMatrix(file->active_scene->ptr);
	//	if (static_cast<CLASS::OBJECT::Data*>(object->object_store->getObjectStore()->data->getData(file->active_scene->ptr, object))->type == CLASS::OBJECT::DATA::Type::MESH) {
	//		const CLASS::OBJECT::DATA::Mesh* mesh = static_cast<const CLASS::OBJECT::DATA::Mesh*>(static_cast<CLASS::OBJECT::Data*>(object->object_store->getObjectStore()->data->getData(file->active_scene->ptr, object))->data);
	//		for (const CLASS::OBJECT::DATA::MESH::Face* face : mesh->faces) {
	//			const dvec4 vert4_a = object->transform_matrix * dvec4(face->vertices[0]->position, 1.0);
	//			const dvec4 vert4_b = object->transform_matrix * dvec4(face->vertices[1]->position, 1.0);
	//			const dvec4 vert4_c = object->transform_matrix * dvec4(face->vertices[2]->position, 1.0);
	//			const dvec3 vert_a = dvec3(vert4_a.x, vert4_a.y, vert4_a.z) / vert4_a.w;
	//			const dvec3 vert_b = dvec3(vert4_b.x, vert4_b.y, vert4_b.z) / vert4_b.w;
	//			const dvec3 vert_c = dvec3(vert4_c.x, vert4_c.y, vert4_c.z) / vert4_c.w;
	//			triangles.push_back(GPU_Triangle(vert_a, vert_b, vert_c));
	//		}
	//	}
	//}

	GLuint triangle_buffer;
	glGenBuffers(1, &triangle_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangle_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPU_Triangle) * triangles.size(), triangles.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triangle_buffer);
}

void GUI::WORKSPACE::Viewport_GPU_Renderer::f_updateFrame() {
	f_updateNodes();
	f_uploadData();
	update();
}

void GUI::WORKSPACE::Viewport_GPU_Renderer::f_updateNodes() {
	//for (CLASS::Object* object : file->active_scene->ptr->objects) {
	//	if (object->object_store) {
	//		const CLASS::Transform keyframed_transform = *static_cast<CLASS::Transform*>(object->object_store->inputs[0]->getData(file->active_scene->ptr, object));
	//
	//		static_cast<CLASS::Transform*>(object->object_store->getObjectStore()->transform->getData(file->active_scene->ptr, object))->position = keyframed_transform.position;
	//		static_cast<CLASS::Transform*>(object->object_store->getObjectStore()->transform->getData(file->active_scene->ptr, object))->euler_rotation = keyframed_transform.euler_rotation;
	//		static_cast<CLASS::Transform*>(object->object_store->getObjectStore()->transform->getData(file->active_scene->ptr, object))->scale = keyframed_transform.scale;
	//	}
	//}
}

void GUI::WORKSPACE::Viewport_GPU_Renderer::initializeGL() {
	initializeOpenGLFunctions();
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glViewport(0, 0, display_resolution.x, display_resolution.y);

	f_pipeline();
	f_uploadData();
}

void GUI::WORKSPACE::Viewport_GPU_Renderer::paintGL() {
	//glClear(GL_COLOR_BUFFER_BIT);
	//
	//CLASS::OBJECT::DATA::Camera* camera = static_cast<CLASS::OBJECT::Data*>(file->active_scene->ptr->active_camera->object_store->getObjectStore()->data->getData(file->active_scene->ptr, file->active_scene->ptr->active_camera))->getCamera();
	//camera->f_compile(file->active_scene->ptr, file->active_scene->ptr->active_camera);
	//
	//glUseProgram(compute_shader_program);
	//glUniform3fv(glGetUniformLocation(compute_shader_program, "camera_pos" ), 1, value_ptr(d_to_f(static_cast<CLASS::Transform*>(file->active_scene->ptr->active_camera->object_store->getObjectStore()->transform->getData(file->active_scene->ptr, file->active_scene->ptr->active_camera))->position)));
	//glUniform3fv(glGetUniformLocation(compute_shader_program, "camera_p_uv"), 1, value_ptr(d_to_f(camera->projection_center)));
	//glUniform3fv(glGetUniformLocation(compute_shader_program, "camera_p_u") , 1, value_ptr(d_to_f(camera->projection_u)));
	//glUniform3fv(glGetUniformLocation(compute_shader_program, "camera_p_v") , 1, value_ptr(d_to_f(camera->projection_v)));
	//
	//glDispatchCompute(compute_layout.x, compute_layout.y, compute_layout.z);
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);
	//
	//// Display Render
	//glUseProgram(display_shader_program);
	//
	//glBindTextureUnit(0, compute_render);
	//glUniform1f(glGetUniformLocation(display_shader_program, "display_aspect_ratio"), d_to_f(display_aspect_ratio));
	//glUniform1f(glGetUniformLocation(display_shader_program, "render_aspect_ratio"),  d_to_f(render_aspect_ratio));
	//glUniform1i(glGetUniformLocation(display_shader_program, "render"), 0);
	//
	//glBindVertexArray(fullscreen_quad_VAO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void GUI::WORKSPACE::Viewport_GPU_Renderer::resizeGL(int w, int h) {
	display_resolution = uvec2(w, h);
	render_resolution = d_to_u(u_to_d(display_resolution) / downsampling);
	display_aspect_ratio = u_to_d(display_resolution.x) / u_to_d(display_resolution.y);
	render_aspect_ratio = u_to_d(render_resolution.x) / u_to_d(render_resolution.y);

	GLuint new_compute;
	glCreateTextures(GL_TEXTURE_2D, 1, &new_compute);
	glTextureParameteri(new_compute, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST for raw pixels
	glTextureParameteri(new_compute, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(new_compute, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(new_compute, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(new_compute, 1, GL_RGBA8, render_resolution.x, render_resolution.y);
	glBindImageTexture(0, new_compute, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	compute_render = new_compute;

	glViewport(0, 0, display_resolution.x, display_resolution.y);
}