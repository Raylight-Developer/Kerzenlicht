#include "Rendering/Rasterizer.hpp"

#include "Rendering/Renderer.hpp"

KL::Rasterizer::Rasterizer(Renderer* renderer) :
	renderer(renderer)
{}

void KL::Rasterizer::f_initialize() {
	glEnable(GL_DEPTH_TEST);
	FILE->default_camera->data->getCamera()->updateFocalAngle();
	auto [compiled, id] = fragmentShaderProgram("Rasterizer");
	if (compiled) {
		data["raster_program"] = id;
	}
}

void KL::Rasterizer::f_tickUpdate() {
}

void KL::Rasterizer::f_recompile() {
	auto [compiled, id] = fragmentShaderProgram("Rasterizer");
	if (compiled) {
		glDeleteProgram(data["raster_program"]);
		data["raster_program"] = id;
	}
}

void KL::Rasterizer::f_cleanup() {
	glDeleteProgram(data["raster_program"]);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_POLYGON_OFFSET_LINE);

	for (KL::Object* object : FILE->active_scene->pointer->objects) {
		auto vao = &gl_data[object]["vao"];
		auto vbo = &gl_data[object]["vbo"];
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

void KL::Rasterizer::f_render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const GLuint raster_program = data["raster_program"];
	glUseProgram(raster_program);

	KL::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
	glUniform3fv(glGetUniformLocation(raster_program, "camera_pos" ), 1, value_ptr(d_to_f(FILE->default_camera->transform.position)));
	glUniformMatrix4fv(glGetUniformLocation(raster_program, "view_matrix"), 1, GL_FALSE, value_ptr(camera->glViewMatrix(FILE->default_camera)));
	glUniformMatrix4fv(glGetUniformLocation(raster_program, "projection_matrix"), 1, GL_FALSE, value_ptr(camera->glProjectionMatrix(renderer->render_aspect_ratio)));

	for (KL::Object* object : FILE->active_scene->pointer->objects) {
		if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
			f_renderMesh(raster_program, object);
		}
		else if (object->data->type == KL::OBJECT::DATA::Type::GROUP) {
			f_renderGroup(raster_program, object);
		}
	}
}


void KL::Rasterizer::f_renderMesh(const GLuint raster_program, KL::Object* object) {
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
	// Mesh
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniformMatrix4fv(glGetUniformLocation(raster_program, "model_matrix"), 1, GL_FALSE, value_ptr(d_to_f(object->transform_matrix)));
	glUniform1ui(glGetUniformLocation(raster_program, "wireframe"), 0);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);
	// Wireframe

	glUniform1ui(glGetUniformLocation(raster_program, "wireframe"), 1);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.0, -1.0);
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