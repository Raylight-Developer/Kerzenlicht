#pragma once

#include "Include.hpp"
#include "Ops.hpp"

#include "Gpu_Scene.hpp"

KL::Confirm<GLuint> fragmentShaderProgram(const string& file_path);
KL::Confirm<GLuint> computeShaderProgram (const string& file_path);
GLuint renderLayer(const uvec2& resolution, const GLuint& filter = GL_NEAREST);
void   bindRenderLayer(const GLuint& program_id, const GLuint& unit, const GLuint& id, const string& name);

bool checkShaderCompilation(const GLuint& shader, const string& shader_code);
bool checkProgramLinking(const GLuint& program);
void printShaderErrorWithContext(const string& shaderSource, const string& errorLog);

template <typename T>
GLuint ssboBinding(const GLuint& binding, const GLuint& size, const vector<T>& data);