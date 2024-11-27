#include "Rendering/PathTracer.hpp"

#include "Rendering/Renderer.hpp"

KL::PathTracer::PathTracer(Renderer* renderer) :
	renderer(renderer)
{
	gpu_data = new GPU::Scene();

	debug = false;

	current_sample = 0;

	resolution = uvec2(0);
	aspect_ratio = 1.0;

	r_resolution = uvec2(0);
	r_aspect_ratio = 1.0;
}

void KL::PathTracer::f_initialize() {
	debug = false;

	current_sample = 0;

	resolution = renderer->resolution;
	aspect_ratio = d_to_f(renderer->aspect_ratio);

	r_resolution = renderer->f_res();
	r_aspect_ratio = d_to_f(renderer->f_aspectRatio());

	data["VAO"] = MAX_UINT32;
	data["VBO"] = MAX_UINT32;
	data["EBO"] = MAX_UINT32;

	data["compute_program"] = MAX_UINT32;
	data["display_program"] = MAX_UINT32;

	data["compute_layout.x"] = MAX_UINT32;
	data["compute_layout.y"] = MAX_UINT32;

	data["view_layer"] = 1;
	data["accumulation_render_layer"] = MAX_UINT32;
	data["normal_render_layer      "] = MAX_UINT32;
	data["bvh_render_layer         "] = MAX_UINT32;
	data["raw_render_layer         "] = MAX_UINT32;

	data["mesh_instances"]  = MAX_UINT32;
	data["mesh_triangles"]  = MAX_UINT32;
	data["mesh_blas"]  = MAX_UINT32;
	data["textures"]  = MAX_UINT32;
	data["texture_data"]  = MAX_UINT32;
	data["camera_lenses"]  = MAX_UINT32;
	data["point_lights"] = MAX_UINT32;
	data["directional_lights"] = MAX_UINT32;

	glViewport(0, 0, resolution.x, resolution.y);
	glClearColor(0, 0, 0, 0);

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
	glCreateVertexArrays(1, &data["VAO"]);
	glCreateBuffers(1, &data["VBO"]);
	glCreateBuffers(1, &data["EBO"]);
	GLuint VAO = data["VAO"];
	GLuint VBO = data["VBO"];
	GLuint EBO = data["EBO"];

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

	glBindVertexArray(data["VAO"]);

	f_recompile();

	data["compute_layout.x"] = d_to_u(ceil(u_to_d(r_resolution.x) / 8.0));
	data["compute_layout.y"] = d_to_u(ceil(u_to_d(r_resolution.y) / 8.0));

	// Compute Output
	data["accumulation_render_layer"] = renderLayer(r_resolution, renderer->display_filter);
	data["normal_render_layer      "] = renderLayer(r_resolution, renderer->display_filter);
	data["bvh_render_layer         "] = renderLayer(r_resolution, renderer->display_filter);
	data["raw_render_layer         "] = renderLayer(r_resolution, renderer->display_filter);

	gpu_data->f_init();
	gpu_data->updateTextures();
	gpu_data->f_update();
	gpu_data->printInfo();
	data["mesh_instances"] = ssboData(gpu_data->mesh_instances);
	data["mesh_triangles"] = ssboData(gpu_data->mesh_triangles);
	data["mesh_blas"] = ssboData(gpu_data->mesh_blas);
	data["textures"]  = ssboData(gpu_data->textures);
	data["texture_data"]  = ssboData(gpu_data->texture_data);
	data["camera_lenses"]  = ssboData(gpu_data->camera_lenses);
	data["point_lights"] = ssboData(gpu_data->point_lights);
	data["directional_lights"] = ssboData(gpu_data->directional_lights);
}

void KL::PathTracer::f_tickUpdate() {
	//gpu_data->f_update();
	//
	//glDeleteBuffers(1, &data["mesh_instances"]);
	//glDeleteBuffers(1, &data["mesh_triangles"]);
	//glDeleteBuffers(1, &data["mesh_blas"]);
	//data["mesh_instances"] = ssboData(gpu_data->mesh_instances);
	//data["mesh_triangles"] = ssboData(gpu_data->mesh_triangles);
	//data["mesh_blas"] = ssboData(gpu_data->mesh_bvh);
	//
	//glDeleteBuffers(1, &data["textures"]);
	//glDeleteBuffers(1, &data["texture_data"]);
	//glDeleteBuffers(1, &data["camera_lenses"]);
	//glDeleteBuffers(1, &data["point_lights"]);
	//glDeleteBuffers(1, &data["directional_lights"]);
	//data["textures"]  = ssboData(gpu_data->textures);
	//data["texture_data"]  = ssboData(gpu_data->texture_data);
	//data["camera_lenses"]  = ssboData(gpu_data->camera_lenses);
	//data["point_lights"] = ssboData(gpu_data->point_lights);
	//data["directional_lights"] = ssboData(gpu_data->directional_lights);
}

void KL::PathTracer::f_recompile() {
	//delete renderer->file;
	//renderer->file = new KL::Render_File();
	//renderer->file->f_loadAsciiFile("../Shared/Resources/Assets/Ganyu.krz");
	//Session::getInstance().setFile(renderer->file);
	{
		auto confirmation = computeShaderProgram("Compute/Compute");
		if (confirmation) {
			glDeleteProgram(data["compute_program"]);
			data["compute_program"] =  confirmation.data;
		}
	}
	{
		auto confirmation = fragmentShaderProgram("Compute/Display", "Compute/Display");
		if (confirmation) {
			glDeleteProgram(data["display_program"]);
			data["display_program"] = confirmation.data;
		}
	}
	current_sample = 0;
}

void KL::PathTracer::f_cleanup() {
	glDeleteVertexArrays(1, &data["VAO"]);
	glDeleteBuffers(1, &data["VBO"]);
	glDeleteBuffers(1, &data["EBO"]);

	glDeleteProgram(data["compute_program"]);
	glDeleteProgram(data["display_program"]);

	glDeleteBuffers(1, &data["mesh_instances"]);
	glDeleteBuffers(1, &data["mesh_triangles"]);
	glDeleteBuffers(1, &data["mesh_blas"]);
	glDeleteBuffers(1, &data["mesh_tlas"]);

	glDeleteBuffers(1, &data["textures"]);
	glDeleteBuffers(1, &data["texture_data"]);

	glDeleteBuffers(1, &data["camera_lenses"]);
	glDeleteBuffers(1, &data["point_lights"]);
	glDeleteBuffers(1, &data["directional_lights"]);

	glDeleteTextures(1, &data["accumulation_render_layer"]);
	glDeleteTextures(1, &data["normal_render_layer      "]);
	glDeleteTextures(1, &data["bvh_render_layer         "]);
	glDeleteTextures(1, &data["raw_render_layer         "]);

	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(3, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glBindTextureUnit(0, 0);
	glBindTextureUnit(1, 0);
	glBindTextureUnit(2, 0);
	glBindTextureUnit(3, 0);

	glUseProgram(0);

	for (KL::Object* object : FILE->active_scene.pointer->objects) {
		object->cpu_update = true;
		if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
			object->getMesh()->cpu_update = true;
		}
	}
}

void KL::PathTracer::f_resize() {
	auto prev_res = r_resolution;
	resolution = renderer->resolution;
	aspect_ratio = d_to_f(renderer->aspect_ratio);

	r_resolution = renderer->f_res();
	r_aspect_ratio = d_to_f(renderer->f_aspectRatio());

	current_sample = 0;
	glViewport(0, 0, resolution.x, resolution.y);

	if (prev_res != r_resolution) { 

		data["compute_layout.x"] = d_to_u(ceil(u_to_d(r_resolution.x) / 8.0));
		data["compute_layout.y"] = d_to_u(ceil(u_to_d(r_resolution.y) / 8.0));

		glDeleteTextures(1, &data["accumulation_render_layer"]);
		glDeleteTextures(1, &data["normal_render_layer      "]);
		glDeleteTextures(1, &data["bvh_render_layer         "]);
		glDeleteTextures(1, &data["raw_render_layer         "]);
		data["accumulation_render_layer"] = renderLayer(r_resolution, renderer->display_filter);
		data["normal_render_layer      "] = renderLayer(r_resolution, renderer->display_filter);
		data["bvh_render_layer         "] = renderLayer(r_resolution, renderer->display_filter);
		data["raw_render_layer         "] = renderLayer(r_resolution, renderer->display_filter);
	}
}

void KL::PathTracer::f_render() {
	const GLuint compute_program = data["compute_program"];
	const GLuint display_program = data["display_program"];

	glUseProgram(compute_program);

	mat3 vectors;
	mat3 projections;
	KL::Object* camera_object = FILE->f_activeCamera();
	KL::OBJECT::DATA::Camera* camera = camera_object->getCamera();
	camera->glProperties(FILE->active_scene.pointer, camera_object, vectors, projections);

	// Uniforms
	{
		glUniform1ui(glGetUniformLocation(compute_program, "frame_count"      ), ul_to_u(renderer->runframe));
		glUniform1f (glGetUniformLocation(compute_program, "aspect_ratio"     ), r_aspect_ratio);
		glUniform1f (glGetUniformLocation(compute_program, "current_time"     ), d_to_f(renderer->current_time));
		glUniform2ui(glGetUniformLocation(compute_program, "resolution"       ), r_resolution.x, r_resolution.y);
		glUniform1ui(glGetUniformLocation(compute_program, "debug"            ), static_cast<GLuint>(debug));
		glUniform1ui(glGetUniformLocation(compute_program, "current_sample"   ), current_sample);
		glUniform1ui(glGetUniformLocation(compute_program, "ray_bounces"      ), 3);
		glUniform1ui(glGetUniformLocation(compute_program, "samples_per_pixel"), 1);

		glUniform3fv(glGetUniformLocation(compute_program, "camera.position"      ), 1, value_ptr(d_to_f(camera_object->transform.position)));
		glUniform3fv(glGetUniformLocation(compute_program, "camera.x_vec"         ), 1, value_ptr(vectors[0]));
		glUniform3fv(glGetUniformLocation(compute_program, "camera.y_vec"         ), 1, value_ptr(vectors[1]));
		glUniform3fv(glGetUniformLocation(compute_program, "camera.z_vec"         ), 1, value_ptr(vectors[2]));
		glUniform3fv(glGetUniformLocation(compute_program, "camera.p_uv"          ), 1, value_ptr(projections[0]));
		glUniform3fv(glGetUniformLocation(compute_program, "camera.p_u"           ), 1, value_ptr(projections[1]));
		glUniform3fv(glGetUniformLocation(compute_program, "camera.p_v"           ), 1, value_ptr(projections[2]));
		glUniform1f (glGetUniformLocation(compute_program, "camera.focal_distance"), d_to_f(camera->focal_distance));
		glUniform1f (glGetUniformLocation(compute_program, "camera.aperture"      ), d_to_f(camera->aperture));
		glUniform1f (glGetUniformLocation(compute_program, "camera.fov"           ), d_to_f(camera->view_angle));
	}
	// Render Layers
	{
		glBindImageTexture(0, data["accumulation_render_layer"], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, data["raw_render_layer         "], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(2, data["bvh_render_layer         "], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glBindImageTexture(3, data["normal_render_layer      "], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}
	// SSBOs
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, data["mesh_instances"]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, data["mesh_triangles"]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, data["mesh_blas"]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, data["mesh_tlas"]);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, data["textures"]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, data["texture_data"]);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, data["camera_lenses"]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, data["point_lights"]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, data["directional_lights"]);
	}

	glDispatchCompute(data["compute_layout.x"], data["compute_layout.y"], 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glUseProgram(display_program);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniform1f (glGetUniformLocation(display_program, "display_aspect_ratio"), aspect_ratio);
	glUniform1f (glGetUniformLocation(display_program, "render_aspect_ratio"), r_aspect_ratio);
	glUniform1ui(glGetUniformLocation(display_program, "view_layer"), data["view_layer"] );
	glUniform1ui(glGetUniformLocation(display_program, "debug"), static_cast<GLuint>(debug));
	glUniform1ui(glGetUniformLocation(display_program, "current_sample"), current_sample);
	bindRenderLayer(display_program, 0, data["accumulation_render_layer"], "accumulation_render_layer");
	bindRenderLayer(display_program, 1, data["raw_render_layer         "], "raw_render_layer"         );
	bindRenderLayer(display_program, 2, data["bvh_render_layer         "], "bvh_render_layer"         );
	bindRenderLayer(display_program, 3, data["normal_render_layer      "], "normal_render_layer"      );
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	current_sample++;
}