#pragma once

#include "Include.hpp"
#include "Ops.hpp"

GLuint fragmentShaderProgram(const string& file_path);
GLuint computeShaderProgram(const string& file_path);
GLuint renderLayer(const uvec2& resolution);
void   bindRenderLayer(const GLuint& program_id, const GLuint& unit, const GLuint& id, const string& name);

struct Image {
	uint16_t width;
	uint16_t height;
	unsigned char* data;
	int channel_fromat;
	int data_type;

	Image();
	bool init(const string& i_file_path);
};

vector<uint> loadRgba8Texture(const string& file_path, uvec2& resolution);

//struct GPU_Texture {
//	GLuint ID;
//
//	GPU_Texture() { ID = 0; };
//	void init(const string& i_image_path);
//};