#include "Rendering/OpenGl.hpp"

void EBO::f_init(GLuint* i_indices, GLsizeiptr i_size) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_size, i_indices, GL_STATIC_DRAW);
}

void EBO::f_bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::f_unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::f_delete() {
	glDeleteBuffers(1, &ID);
}

void FBO::f_init() {
	glGenFramebuffers(1, &ID);
}

void FBO::f_bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FBO::f_unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::f_delete() {
	glDeleteFramebuffers(1, &ID);
}

void FBT::f_init(const uvec2& i_size) {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_size.x, i_size.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FBT::f_resize(const uvec2& i_size) {
	GLuint new_ID;

	glGenTextures(1, &new_ID);
	glBindTexture(GL_TEXTURE_2D, new_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_size.x, i_size.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, new_ID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	ID = new_ID;
}

void FBT::f_bind(const GLenum& i_texture_id) {
	glActiveTexture(i_texture_id);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void FBT::f_unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FBT::f_delete() {
	glDeleteTextures(1, &ID);
}

void VAO::f_init() {
	glGenVertexArrays(1, &ID);
}

void VAO::f_linkVBO(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset) {
	VBO.f_bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.f_unbind();
}

void VAO::f_bind() {
	glBindVertexArray(ID);
}

void VAO::f_unbind() {
	glBindVertexArray(0);
}

void VAO::f_delete() {
	glDeleteVertexArrays(1, &ID);
}

void VBO::f_init(GLfloat* vertices, GLsizeiptr size) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::f_bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::f_unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::f_delete() {
	glDeleteBuffers(1, &ID);
}

void Shader_Program::f_init(const string& i_fragmentFile) {
	source = i_fragmentFile;
	f_compile();
}

void Shader_Program::f_compile() {
	glDeleteProgram(ID);

	string vertexCode = f_loadFromFile("./resources/Shaders/vert.glsl");
	string fragmentCode = f_loadFromFile(source);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	f_checkCompilation(vertexShader, "VERTEX " + program_name);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	f_checkCompilation(fragmentShader, "FRAGMENT " + program_name);

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	f_checkCompilation(ID, "PROGRAM " + program_name);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

string Shader_Program::f_loadFromFile(const string& i_filename) {
	ifstream in(i_filename, ios::binary);
	if (in) {
		string contents;
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

void Shader_Program::f_activate() {
	glUseProgram(ID);
}

void Shader_Program::f_delete() {
	glDeleteProgram(ID);
}

void Shader_Program::f_checkCompilation(const GLuint& i_shader, const string& i_shader_name) {
	GLint hasCompiled;
	char infoLog[1024];
	if (i_shader_name != "PROGRAM") {
		glGetShaderiv(i_shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) {
			glGetShaderInfoLog(i_shader, 1024, NULL, infoLog);
			cout << "SHADER_COMPILATION_ERROR for:" << i_shader_name << "\n" << infoLog << endl;
		}
	}
	else {
		glGetProgramiv(i_shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) {
			glGetProgramInfoLog(i_shader, 1024, NULL, infoLog);
			cout << "SHADER_LINKING_ERROR for:" << i_shader_name << "\n" << infoLog << endl;
		}
	}
}

Image::Image() {
}

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image.h"
bool Image::f_load(const string& i_file_path) {
	int t_width, t_height, t_nrChannels;
	data = stbi_load(i_file_path.c_str(), &t_width, &t_height, &t_nrChannels, 0);
	if (data) {
		if (t_nrChannels == 1)
			channel_fromat = GL_RED;
		else if (t_nrChannels == 3)
			channel_fromat = GL_RGB;
		else if (t_nrChannels == 4)
			channel_fromat = GL_RGBA;
		width = t_width;
		height = t_height;
		data_type = GL_UNSIGNED_BYTE;
		return true;
	}
	return false;
}

void Texture::f_init(const string& i_image_path) {
	Image texture = Image();
	if (texture.f_load(i_image_path)) {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, texture.channel_fromat, texture.width, texture.height, 0, texture.channel_fromat, texture.data_type, texture.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Texture::f_bind(const GLenum& i_texture_id) {
	glActiveTexture(i_texture_id);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::f_unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::f_delete() {
	glDeleteTextures(1, &ID);
}