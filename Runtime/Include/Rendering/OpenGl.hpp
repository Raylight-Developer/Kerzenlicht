#pragma once

#include "Include.hpp"

struct EBO;
struct FBO;
struct FBT;
struct VAO;
struct VBO;

struct EBO {
	GLuint ID;

	EBO() { ID = 0; };

	void f_init(GLuint* i_indices, GLsizeiptr i_size);
	void f_bind();
	void f_unbind();
	void f_delete();
};

struct FBO {
	GLuint ID;

	FBO() { ID = 0; };

	void f_init();
	void f_bind();
	void f_unbind();
	void f_delete();
};

struct FBT {
	GLuint ID;

	FBT() { ID = 0; };

	void f_resize(const uvec2& i_size);

	void f_init(const uvec2& i_size);
	void f_bind(const GLenum& i_texture_id);
	void f_unbind();
	void f_delete();
};

struct VAO {
	GLuint ID;

	VAO() { ID = 0; };

	void f_linkVBO(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset);

	void f_init();
	void f_bind();
	void f_unbind();
	void f_delete();
};

struct VBO {
	GLuint ID;

	VBO() { ID = 0; };

	void f_init(GLfloat* vertices, GLsizeiptr size);
	void f_bind();
	void f_unbind();
	void f_delete();
};

struct Shader_Program {
	GLuint ID;
	string frag_source;
	string program_name;

	Shader_Program(const string& i_name = "") { ID = 0; program_name = i_name; };

	void f_compile();
	void f_checkCompilation(const GLuint& i_shader, const string& i_shader_name);

	string f_loadFromFile(const string& i_filename);

	void f_init(const char* i_fragmentFile);
	void f_activate();
	void f_delete();
};

struct Image {
	uint16_t width;
	uint16_t height;
	unsigned char* data;
	int channel_fromat;
	int data_type;

	Image();
	bool f_load(const string& i_file_path);
};

struct Texture {
	GLuint ID;

	Texture() { ID = 0; };

	void f_init(const string& i_image_path);
	void f_bind(const GLenum& i_texture_id);
	void f_unbind();
	void f_delete();
};
