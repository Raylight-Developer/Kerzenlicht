#pragma once

#include "Include.hpp"
#include "Ops.hpp"

GLuint f_fragmentShaderProgram(const string& file_path);
GLuint f_computeShaderProgram(const string& file_path);

struct Image {
	uint16_t width;
	uint16_t height;
	unsigned char* data;
	int channel_fromat;
	int data_type;

	Image();
	bool f_load(const string& i_file_path);
};