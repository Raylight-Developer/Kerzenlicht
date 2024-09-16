#include "Workspaces/Viewport.hpp"

#include "Workspaces/Manager.hpp"

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

	container->setFocusPolicy(Qt::StrongFocus);
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

	renderer_data({}),

	window_time(0.0),
	frame_time(FPS_60),

	panning(false),

	view_layer(0)
{
	setObjectName("Viewport_Realtime");
}

void GUI::WORKSPACE::Viewport::f_pipeline() {
	glEnable(GL_DEPTH_TEST);
	auto [compiled, id] = fragmentShaderProgram("Rasterizer");
	renderer_data["raster_program"] = id;
}

void GUI::WORKSPACE::Viewport::f_uploadData() {

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
}

void GUI::WORKSPACE::Viewport::f_selectObject(const dvec2& uv) { // TODO fix slight missalignment

}

void GUI::WORKSPACE::Viewport::f_recompileShaders() {
	auto [compiled, id] = fragmentShaderProgram("Rasterizer");
	if (compiled) {
		glDeleteProgram(renderer_data["raster_program"]);
		renderer_data["raster_program"] = id;
	}
}

void GUI::WORKSPACE::Viewport::initializeGL() {
	initializeOpenGLFunctions();
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	const GLuint raster_program = renderer_data["raster_program"];
	glUseProgram(raster_program);

	KL::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
	glUniform3fv(glGetUniformLocation(raster_program, "camera_pos" ), 1, value_ptr(d_to_f(FILE->default_camera->transform.position)));
	glUniformMatrix4fv(glGetUniformLocation(raster_program, "view_matrix"), 1, GL_FALSE, value_ptr(d_to_f(camera->glViewMatrix(FILE->default_camera))));
	glUniformMatrix4fv(glGetUniformLocation(raster_program, "projection_matrix"), 1, GL_FALSE, value_ptr(d_to_f(camera->glProjectionMatrix(display_aspect_ratio))));

	for (KL::Object* object : FILE->active_scene->pointer->objects) {
		if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
			f_renderMesh(raster_program, object);
		}
		else if (object->data->type == KL::OBJECT::DATA::Type::GROUP) {
			f_renderGroup(raster_program, object);
		}
	}

	frame_counter++;
	runframe++;

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

	glViewport(0, 0, render_resolution.x, render_resolution.y);
}

void GUI::WORKSPACE::Viewport::mouseReleaseEvent(QMouseEvent* event) {
	panning = false;
}

void GUI::WORKSPACE::Viewport::mousePressEvent(QMouseEvent* event) {
	if (event->modifiers() == Qt::KeyboardModifier::AltModifier) {
		panning = true;
		last_mouse = p_to_d(event->pos());
	}
	else {
		f_selectObject(dvec2(1.0, -1.0) * (dvec2(event->pos().x(), event->pos().y()) - 1.0 - dvec2(width(), height()) / 2.0) / max(i_to_d(width()), i_to_d(height())));
	}
}

void GUI::WORKSPACE::Viewport::mouseMoveEvent(QMouseEvent* event) {
	if (panning) {
		dvec2 delta = (p_to_d(event->pos()) - last_mouse) * 0.25;

		FILE->default_camera->transform.orbit(dvec3(0), dvec2(-delta.y, -delta.x));
		last_mouse = p_to_d(event->pos());
	}
}

void GUI::WORKSPACE::Viewport::keyPressEvent(QKeyEvent* event) {
	if (event->key() == Qt::Key::Key_R) {
		f_recompileShaders();
	}
}

void GUI::WORKSPACE::Viewport::wheelEvent(QWheelEvent*) {
}

void GUI::WORKSPACE::Viewport::f_renderMesh(const GLuint raster_program, KL::Object* object) {
	auto vao = &gl_data[object]["vao"];
	auto vbo = &gl_data[object]["vbo"];
	vector<vec1>* cached_triangles = &gl_triangle_cache[uptr(object)];
	auto mesh = object->data->getMesh();

	if (object->cpu_update) {
		object->cpu_update = false;
		object->f_compileMatrix();
	}
	if (mesh->cpu_update) {
		mesh->cpu_update = false;
		cached_triangles->clear();

		mat3 normal_matrix = mat3(glm::transpose(glm::inverse(object->transform_matrix)));
		for (KL::OBJECT::DATA::MESH::Face* face : mesh->faces) {
			auto tri = KL::OBJECT::DATA::Mesh::faceToArray(face, mesh, object->transform_matrix, normal_matrix);
			cached_triangles->insert(cached_triangles->end(), tri.begin(), tri.end());
		}

		glDeleteVertexArrays(1, vao);
		glDeleteBuffers(1, vbo);

		glGenVertexArrays(1, vao);
		glGenBuffers(1, vbo);

		glBindVertexArray(*vao);
		glBindBuffer(GL_ARRAY_BUFFER, *vbo);

		glBufferData(GL_ARRAY_BUFFER, cached_triangles->size() * sizeof(vec1), cached_triangles->data(), GL_DYNAMIC_DRAW);

		// Vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(vec1), (void*)0);
		glEnableVertexAttribArray(0);

		// Vertex Normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(vec1), (void*)(3 * sizeof(vec1)));
		glEnableVertexAttribArray(1);

		// Vertex UVs
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(vec1), (void*)(6 * sizeof(vec1)));
		glEnableVertexAttribArray(2);
	}
	const GLuint vertex_count = ul_to_u(cached_triangles->size() / 8);

	glBindVertexArray(*vao);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 255);
	glStencilMask(255);

	// Mesh
	glUniformMatrix4fv(glGetUniformLocation(raster_program, "model_matrix"), 1, GL_FALSE, value_ptr(d_to_f(object->transform_matrix)));
	glUniform1ui(glGetUniformLocation(raster_program, "wireframe"), 0);
	glUniform1ui(glGetUniformLocation(raster_program, "stencil"), 0);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);

	// Stencil
	//glDisable(GL_POLYGON_OFFSET_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//
	//glStencilFunc(GL_NOTEQUAL, 1, 255);
	//glStencilMask(255);
	//
	//glUniformMatrix4fv(glGetUniformLocation(raster_program, "model_matrix"), 1, GL_FALSE, value_ptr(glm::scale(d_to_f(object->transform_matrix), vec3(1.15f))));
	//glUniform1ui(glGetUniformLocation(raster_program, "stencil"), 1);
	//glDrawArrays(GL_TRIANGLES, 0, vertex_count);
	//
	//glDisable(GL_STENCIL_TEST);

	// Wireframe
	glUniform1ui(glGetUniformLocation(raster_program, "wireframe"), 1);
	glUniform1ui(glGetUniformLocation(raster_program, "stencil"), 0);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.0, -1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);


	glBindVertexArray(0);
}

void GUI::WORKSPACE::Viewport::f_renderGroup(const GLuint raster_program, KL::Object* object) {
	for (KL::Object* sub_object : object->data->getGroup()->objects) {
		sub_object->f_compileMatrix();
		sub_object->transform_matrix *= object->transform_matrix;

		f_renderMesh(raster_program, sub_object);
	}
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