#include "Rendering/PathTracer.hpp"

#include "Rendering/Renderer.hpp"

KL::PathTracer::PathTracer(Renderer* renderer) :
	renderer(renderer)
{
	gpu_data = new GPU_Scene();
}

void KL::PathTracer::f_initialize() {
	reset = false;
	debug = false;

	data["VAO"] = 0;
	data["VBO"] = 0;
	data["EBO"] = 0;

	data["compute_program"] = 0;
	data["display_program"] = 0;

	data["compute_layout.x"] = 0;
	data["compute_layout.y"] = 0;

	data["view_layer"] = 0;
	data["accumulation_render_layer"] = 0;
	data["normal_render_layer      "] = 0;
	data["bvh_render_layer         "] = 0;
	data["raw_render_layer         "] = 0;

	data["ssbo 5"] = 0;
	data["ssbo 6"] = 0;
	data["ssbo 7"] = 0;
	data["ssbo 8"] = 0;

	glViewport(0, 0, renderer->display_resolution.x, renderer->display_resolution.y);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

	{
		auto confirmation = computeShaderProgram("Compute");
		if (confirmation) {
			data["compute_program"] =  confirmation.data;
		}
	}
	{
		auto confirmation = fragmentShaderProgram("Display");
		if (confirmation) {
			data["display_program"] =  confirmation.data;
		}
	}

	data["compute_layout.x"] = d_to_u(ceil(u_to_d(renderer->render_resolution.x) / 32.0));
	data["compute_layout.y"] = d_to_u(ceil(u_to_d(renderer->render_resolution.y) / 32.0));

	// Compute Output
	data["accumulation_render_layer"] = renderLayer(renderer->render_resolution);
	data["normal_render_layer      "] = renderLayer(renderer->render_resolution);
	data["bvh_render_layer         "] = renderLayer(renderer->render_resolution);
	data["raw_render_layer         "] = renderLayer(renderer->render_resolution);

	gpu_data->f_init();
	gpu_data->updateTextures();
	gpu_data->f_tickUpdate();
	gpu_data->printInfo();
}

void KL::PathTracer::f_tickUpdate() {
	gpu_data->f_tickUpdate();

	GLint ssboMaxSize;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &ssboMaxSize);

	glDeleteBuffers(1, &data["ssbo 5"]);
	glDeleteBuffers(1, &data["ssbo 6"]);
	glDeleteBuffers(1, &data["ssbo 7"]);
	glDeleteBuffers(1, &data["ssbo 8"]);

	data["ssbo 5"] = ssboBinding(5, ul_to_u(gpu_data->trianglesSize())  , gpu_data->triangles.data());
	data["ssbo 6"] = ssboBinding(6, ul_to_u(gpu_data->bvhNodesSize())   , gpu_data->bvh_nodes.data());
	data["ssbo 7"] = ssboBinding(7, ul_to_u(gpu_data->texturesSize())   , gpu_data->textures.data());
	data["ssbo 8"] = ssboBinding(8, ul_to_u(gpu_data->textureDataSize()), gpu_data->texture_data.data());
}

void KL::PathTracer::f_recompile() {
	{
		auto confirmation = computeShaderProgram("Compute");
		if (confirmation) {
			glDeleteProgram(data["compute_program"]);
			data["compute_program"] =  confirmation.data;
		}
	}
	{
		auto confirmation = fragmentShaderProgram("Display");
		if (confirmation) {
			glDeleteProgram(data["display_program"]);
			data["display_program"] = confirmation.data;
		}
	}
}

void KL::PathTracer::f_cleanup() {
	glDeleteVertexArrays(1, &data["VAO"]);
	glDeleteBuffers(1, &data["VBO"]);
	glDeleteBuffers(1, &data["EBO"]);

	glDeleteBuffers(1, &data["ssbo 5"]);
	glDeleteBuffers(1, &data["ssbo 6"]);
	glDeleteBuffers(1, &data["ssbo 7"]);
	glDeleteBuffers(1, &data["ssbo 8"]);

	glDeleteTextures(1, &data["accumulation_render_layer"]);
	glDeleteTextures(1, &data["normal_render_layer      "]);
	glDeleteTextures(1, &data["bvh_render_layer         "]);
	glDeleteTextures(1, &data["raw_render_layer         "]);

	glDeleteProgram(data["compute_program"]);
	glDeleteProgram(data["display_program"]);

	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(3, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glBindTextureUnit(0, 0);
	glBindTextureUnit(1, 0);
	glBindTextureUnit(2, 0);
	glBindTextureUnit(3, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	for (KL::Object* object : FILE->active_scene->pointer->objects) {
		object->cpu_update = true;
		if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
			object->data->getMesh()->cpu_update = true;
		}
	}
}

void KL::PathTracer::f_render() {
	const GLuint compute_program = data["compute_program"];
	const GLuint display_program = data["display_program"];
	glBindVertexArray(data["VAO"]);

	glUseProgram(compute_program);
	glUniform1ui(glGetUniformLocation(compute_program, "frame_count"), ul_to_u(renderer->runframe));
	glUniform1f (glGetUniformLocation(compute_program, "aspect_ratio"), d_to_f(renderer->render_aspect_ratio));
	glUniform1f (glGetUniformLocation(compute_program, "current_time"), d_to_f(renderer->current_time));
	glUniform2ui(glGetUniformLocation(compute_program, "resolution"), renderer->render_resolution.x, renderer->render_resolution.y);
	glUniform1ui(glGetUniformLocation(compute_program, "reset"), static_cast<GLuint>(reset));
	glUniform1ui(glGetUniformLocation(compute_program, "debug"), static_cast<GLuint>(debug));

	glUniform1ui(glGetUniformLocation(compute_program, "ray_bounces"), 1);
	glUniform1ui(glGetUniformLocation(compute_program, "samples_per_pixel"), 1);

	KL::OBJECT::DATA::Camera* camera = FILE->default_camera->data->getCamera();
	camera->compile(FILE->active_scene->pointer, FILE->default_camera);
	glUniform3fv(glGetUniformLocation(compute_program, "camera_pos"),  1, value_ptr(d_to_f(FILE->default_camera->transform.position)));
	glUniform3fv(glGetUniformLocation(compute_program, "camera_p_uv"), 1, value_ptr(d_to_f(camera->projection_uv)));
	glUniform3fv(glGetUniformLocation(compute_program, "camera_p_u"),  1, value_ptr(d_to_f(camera->projection_u)));
	glUniform3fv(glGetUniformLocation(compute_program, "camera_p_v"),  1, value_ptr(d_to_f(camera->projection_v)));

	glBindImageTexture(0, data["accumulation_render_layer"], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, data["raw_render_layer         "], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(2, data["bvh_render_layer         "], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(3, data["normal_render_layer      "], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(data["compute_layout.x"], data["compute_layout.y"], 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUseProgram(display_program);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniform1f (glGetUniformLocation(display_program, "display_aspect_ratio"), d_to_f(renderer->display_aspect_ratio));
	glUniform1f (glGetUniformLocation(display_program, "render_aspect_ratio"), d_to_f(renderer->render_aspect_ratio));
	glUniform1ui(glGetUniformLocation(display_program, "view_layer"), data["view_layer"] );
	glUniform1ui(glGetUniformLocation(display_program, "debug"), static_cast<GLuint>(debug));
	bindRenderLayer(display_program, 0, data["accumulation_render_layer"], "accumulation_render_layer");
	bindRenderLayer(display_program, 1, data["raw_render_layer         "], "raw_render_layer"         );
	bindRenderLayer(display_program, 2, data["bvh_render_layer         "], "bvh_render_layer"         );
	bindRenderLayer(display_program, 3, data["normal_render_layer      "], "normal_render_layer"      );
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}