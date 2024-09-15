#include "Rendering/Rasterizer.hpp"

#include "Rendering/Renderer.hpp"

KL::Rasterizer::Rasterizer(Renderer* renderer) :
	renderer(renderer)
{}

void KL::Rasterizer::f_initialize() {
	d_resolution = renderer->display_resolution;
	d_aspect_ratio = d_to_f(renderer->display_aspect_ratio);

	r_resolution = renderer->f_res();
	r_aspect_ratio = d_to_f(renderer->f_aspectRatio());

	data["display_program"] = 0;
	data["raster_program"] = 0;
	data["FBT"] = 0;
	data["FBO"] = 0;
	data["RBO"] = 0;

	data["VAO"] = 0;
	data["VBO"] = 0;
	data["EBO"] = 0;

	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(-1.0, -1.0);

	cout << endl << endl << r_aspect_ratio << endl << d_aspect_ratio << endl << endl;

	{
		auto confirmation = fragmentShaderProgram("Rasterizer");
		if (confirmation) {
			data["raster_program"] = confirmation.data;
		}
	}
	{
		auto confirmation = fragmentShaderProgram("Raster_Display");
		if (confirmation) {
			data["display_program"] = confirmation.data;
		}
	}

	// Fullscreen Quad
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

	glCreateVertexArrays(1, &data["VAO"]);
	glCreateBuffers(1, &data["VBO"]);
	glCreateBuffers(1, &data["EBO"]);
	GLuint Array_Object = data["VAO"];
	GLuint Buffer_Object = data["VBO"];
	GLuint EBO = data["EBO"];

	glNamedBufferData(Buffer_Object, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(Array_Object, 0);
	glVertexArrayAttribBinding(Array_Object, 0, 0);
	glVertexArrayAttribFormat(Array_Object, 0, 2, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(Array_Object, 1);
	glVertexArrayAttribBinding(Array_Object, 1, 0);
	glVertexArrayAttribFormat(Array_Object, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(Array_Object, 0, Buffer_Object, 0, 4 * sizeof(GLfloat));
	glVertexArrayElementBuffer(Array_Object, EBO);

	glBindVertexArray(0);

	// FBO
	glGenFramebuffers(1, &data["FBO"]);
	const GLuint fbo = data["FBO"];
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &data["FBT"]);
	const GLuint fbt = data["FBT"];
	glBindTexture(GL_TEXTURE_2D, fbt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, r_resolution.x, r_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, renderer->display_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, renderer->display_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbt, 0);

	glGenRenderbuffers(1, &data["RBO"]);
	const GLuint rbo = data["RBO"];
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, r_resolution.x, r_resolution.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void KL::Rasterizer::f_tickUpdate() {
}

void KL::Rasterizer::f_recompile() {
	{
		auto confirmation = fragmentShaderProgram("Rasterizer");
		if (confirmation) {
			data["raster_program"] = confirmation.data;
		}
	}
	{
		auto confirmation = fragmentShaderProgram("Raster_Display");
		if (confirmation) {
			data["display_program"] = confirmation.data;
		}
	}
}

void KL::Rasterizer::f_cleanup() {
	glDeleteVertexArrays(1, &data["VAO"]);
	glDeleteBuffers(1, &data["VBO"]);
	glDeleteBuffers(1, &data["EBO"]);

	glDeleteProgram(data["display_program"]);
	glDeleteProgram(data["raster_program"]);

	glDeleteRenderbuffers(1, &data["RBO"]);
	glDeleteFramebuffers(1, &data["FBO"]);
	glDeleteTextures(1, &data["FBT"]);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (KL::Object* object : FILE->active_scene->pointer->objects) {
		auto vao = &gl_data[object]["VAO"];
		auto vbo = &gl_data[object]["VBO"];
		glDeleteVertexArrays(1, vao);
		glDeleteBuffers(1, vbo);
	}

	gl_triangle_cache.clear();
	gl_data.clear();

	glBindVertexArray(0);
	glUseProgram(0);

	for (KL::Object* object : FILE->active_scene->pointer->objects) {
		object->cpu_update = true;
		if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
			object->data->getMesh()->cpu_update = true;
		}
	}
}

void KL::Rasterizer::f_resize() {
	d_resolution = renderer->display_resolution;
	d_aspect_ratio = d_to_f(renderer->display_aspect_ratio);

	r_resolution = renderer->f_res();
	r_aspect_ratio = d_to_f(renderer->f_aspectRatio());

	if (true) {
		glDeleteRenderbuffers(1, &data["RBO"]);
		glDeleteFramebuffers(1, &data["FBO"]);
		glDeleteTextures(1, &data["FBT"]);

		glGenFramebuffers(1, &data["FBO"]);
		const GLuint fbo = data["FBO"];
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glGenTextures(1, &data["FBT"]);
		const GLuint fbt = data["FBT"];
		glBindTexture(GL_TEXTURE_2D, fbt);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, r_resolution.x, r_resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, renderer->display_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, renderer->display_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbt, 0);

		glGenRenderbuffers(1, &data["RBO"]);
		const GLuint rbo = data["RBO"];
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, r_resolution.x, r_resolution.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void KL::Rasterizer::f_render() {
	const GLuint raster_program = data["raster_program"];
	const GLuint display_program = data["display_program"];

	glViewport(0, 0, r_resolution.x, r_resolution.y);
	glBindFramebuffer(GL_FRAMEBUFFER, data["FBO"]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(raster_program);

	KL::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
	glUniform3fv(glGetUniformLocation(raster_program, "camera_pos" ), 1, value_ptr(d_to_f(FILE->default_camera->transform.position)));
	glUniformMatrix4fv(glGetUniformLocation(raster_program, "view_matrix"), 1, GL_FALSE, value_ptr(d_to_f(camera->glViewMatrix(FILE->default_camera))));
	glUniformMatrix4fv(glGetUniformLocation(raster_program, "projection_matrix"), 1, GL_FALSE, value_ptr(d_to_f(camera->glProjectionMatrix(r_aspect_ratio))));

	for (KL::Object* object : FILE->active_scene->pointer->objects) {
		if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
			f_renderMesh(raster_program, object);
		}
		else if (object->data->type == KL::OBJECT::DATA::Type::GROUP) {
			f_renderGroup(raster_program, object);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, d_resolution.x, d_resolution.y);
	glUseProgram(display_program);

	glUniform1f (glGetUniformLocation(display_program, "display_aspect_ratio"), d_aspect_ratio);
	glUniform1f (glGetUniformLocation(display_program, "render_aspect_ratio"), r_aspect_ratio);
	bindRenderLayer(display_program, 0, data["FBT"], "render");

	glBindVertexArray(data["VAO"]);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void KL::Rasterizer::f_renderMesh(const GLuint raster_program, KL::Object* object) {
	auto vao = &gl_data[object]["VAO"];
	auto vbo = &gl_data[object]["VBO"];
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
	// Mesh
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniformMatrix4fv(glGetUniformLocation(raster_program, "model_matrix"), 1, GL_FALSE, value_ptr(d_to_f(object->transform_matrix)));
	glUniform1ui(glGetUniformLocation(raster_program, "wireframe"), 0);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);
	// Wireframe

	glUniform1ui(glGetUniformLocation(raster_program, "wireframe"), 1);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);

	glBindVertexArray(0);
}

void KL::Rasterizer::f_renderGroup(const GLuint raster_program, KL::Object* object) {
	for (KL::Object* sub_object : object->data->getGroup()->objects) {
		sub_object->f_compileMatrix();
		sub_object->transform_matrix *= object->transform_matrix;

		f_renderMesh(raster_program, sub_object);
	}
}