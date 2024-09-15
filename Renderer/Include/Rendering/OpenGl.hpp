#pragma once

#include "Include.hpp"
#include "Ops.hpp"

namespace GL {
	struct Data;
	struct SSBO;
	struct Render_Layer;
	struct Array_Object;
	struct Buffer_Object;
}

namespace GL {
	struct Data {
		GLuint ID;

		Data() { ID = 0; }
		virtual void f_clean() = 0;
	};

	struct Array_Object : Data {
		Array_Object();

		void f_clean() override;

		void f_linkAttribute(Buffer_Object& Buffer_Object, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		void f_bind();
		void f_unbind();
	};

	struct Buffer_Object : Data {
		Buffer_Object();

		void f_clean() override;

		template <typename T>
		void f_init(const vector<T>& data, const GLuint& draw_mode = GL_DYNAMIC_DRAW) {
			f_clean();
			glGenBuffers(1, &ID);
			f_bind();
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), draw_mode);
			f_unbind();
		}
		void f_bind();
		void f_unbind();
	};

	struct SSBO : Data {
		GLuint binding_index;

		SSBO();

		void f_clean() override;

		void f_bind();
		void f_unbind();

		template <typename T>
		void f_init(const GLuint& binding_index, const vector<T>& data) {
			f_clean();
			this->binding_index = binding_index;
			glGenBuffers(1, &ID);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ID);
			glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_index, ID);
		}
	};

	struct Render_Layer : Data {
		GLuint binding_index;

		Render_Layer();

		void f_clean() override;

		void f_init(const uvec2& resolution, const GLuint& format = GL_RGBA32F);
		void f_bind(const GLuint& program_id, const GLuint& binding_index, const string& sampler_name);
		void f_unbind();
	};
}

tuple<bool, GLuint> fragmentShaderProgram(const string& file_path);
tuple<bool, GLuint> computeShaderProgram (const string& file_path);
GLuint renderLayer(const uvec2& resolution);
void   bindRenderLayer(const GLuint& program_id, const GLuint& unit, const GLuint& id, const string& name);

bool checkShaderCompilation(const GLuint& shader, const string& shader_code);
bool checkProgramLinking(const GLuint& program);
void printShaderErrorWithContext(const string& shaderSource, const string& errorLog);

template <typename T>
GLuint ssboBinding(const GLuint& binding, const GLuint& size, const T& data) {
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
	return buffer;
}