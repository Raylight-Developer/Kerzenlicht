#pragma once

#include "Include.hpp"
#include "Ops.hpp"

GLuint fragmentShaderProgram(const string& file_path);
GLuint computeShaderProgram(const string& file_path);
GLuint renderLayer(const uvec2& resolution, const GLuint& format);
void   bindRenderLayer(const GLuint& program_id, const GLuint& unit, const GLuint& id, const string& name);