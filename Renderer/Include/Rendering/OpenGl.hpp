#pragma once

#include "Shared.hpp"

#include "Shader/Shader.hpp"

KL::Confirm<GLuint> fragmentShaderProgram(const string& vert_file_path, const string& frag_file_path);
KL::Confirm<GLuint> computeShaderProgram (const string& file_path);
GLuint renderLayer(const uvec2& resolution, const GLuint& filter = GL_NEAREST);
void   bindRenderLayer(const GLuint& program_id, const GLuint& unit, const GLuint& id, const string& name);

bool checkShaderCompilation(const GLuint& shader, const string& shader_code);
bool checkProgramLinking(const GLuint& program);
void printShaderErrorWithContext(const string& shaderSource, const string& errorLog);

template <typename T>
GLuint ssboData(const vector<T>& data) {
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ul_to_u(sizeof(T) * data.size()), data.data(), GL_DYNAMIC_DRAW);
	return buffer;
}