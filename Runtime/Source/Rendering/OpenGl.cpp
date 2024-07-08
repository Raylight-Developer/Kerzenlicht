#include "Rendering/OpenGl.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image.h"

Image::Image() {
	data = 0;
	width = 0;
	height = 0;
	channel_fromat = 0;
	data_type = GL_UNSIGNED_BYTE;
}

bool Image::init(const string& i_file_path) {
		
	int t_width, t_height, t_nrChannels;
	data = stbi_load(i_file_path.c_str(), &t_width, &t_height, &t_nrChannels, STBI_rgb_alpha);
	if (data) {
		channel_fromat = GL_RGBA;
		width = t_width;
		height = t_height;
		data_type = GL_UNSIGNED_BYTE;
		return true;
	}
	return false;
}

//void GPU_Texture::init(const string& i_image_path) {
//	Image texture = Image();
//	if (texture.init(i_image_path)) {
//		glGenTextures(1, &ID);
//		glBindTexture(GL_TEXTURE_2D, ID);
//		glTexImage2D(GL_TEXTURE_2D, 0, texture.channel_fromat, texture.width, texture.height, 0, texture.channel_fromat, texture.data_type, texture.data);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glBindTexture(GL_TEXTURE_2D, 0);
//	}
//}

GLuint fragmentShaderProgram(const string& file_path) {
	GLuint shader_program = glCreateShader(GL_VERTEX_SHADER);

	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	const string vertex_code = loadFromFile("./Resources/Shaders/" + file_path + ".vert");
	const char* vertex_code_cstr = vertex_code.c_str();
	glShaderSource(vert, 1, &vertex_code_cstr, NULL);
	glCompileShader(vert);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	const string fragment_code = loadFromFile("./Resources/Shaders/" + file_path + ".frag");
	const char* fragment_code_cstr = fragment_code.c_str();
	glShaderSource(frag, 1, &fragment_code_cstr, NULL);
	glCompileShader(frag);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vert);
	glAttachShader(shader_program, frag);
	glLinkProgram(shader_program);

	glDeleteShader(vert);
	glDeleteShader(frag);

	return shader_program;
}

GLuint computeShaderProgram(const string& file_path) {
	GLuint compute_program;

	const string compute_code = preprocessShader("./Resources/Shaders/" + file_path + ".comp");
	ofstream outFile;
	outFile.open("./Resources/Shaders/" + file_path + "_Compiled.comp");
	outFile << compute_code;
	outFile.close();
	const char* compute_code_cstr = compute_code.c_str();
	GLuint comp_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(comp_shader, 1, &compute_code_cstr, NULL);
	glCompileShader(comp_shader);

	compute_program = glCreateProgram();
	glAttachShader(compute_program, comp_shader);
	glLinkProgram(compute_program);

	return compute_program;
}

GLuint renderLayer(const uvec2& resolution) {
	GLuint ID;
	glCreateTextures(GL_TEXTURE_2D, 1, &ID);
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D (ID, 1, GL_RGBA32F, resolution.x,resolution.y);
	return ID;
}

void bindRenderLayer(const GLuint& program_id, const GLuint& unit, const GLuint& id, const string& name) {
	glUniform1i(glGetUniformLocation(program_id, name.c_str()), unit);
	glBindTextureUnit(unit, id);
}

vector<uint> loadRgba8Texture(const string& file_path, uvec2& resolution) {

	int width, height, channels;
	unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!data) {
		std::cerr << "Failed to load image: " << file_path << std::endl;
	}

	vector<uint> imageData(width * height);
	for (uint i = 0; i < i_to_u(width * height); ++i) {
		uint r = data[i * 4 + 0];
		uint g = data[i * 4 + 1];
		uint b = data[i * 4 + 2];
		uint a = data[i * 4 + 3];
		imageData[i] = (r << 24) | (g << 16) | (b << 8) | a;
	}

	resolution = uvec2(i_to_u(width), i_to_u(height));
	return imageData;
}

vector<uint> loadRgba16fTexture(const string& file_path, uvec2& resolution) {

	int width, height, channels;
	float* data = stbi_loadf(file_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!data) {
		std::cerr << "Failed to load image: " << file_path << std::endl;
	}

	vector<uint> imageData(width * height);
	//TODO

	resolution = uvec2(i_to_u(width), i_to_u(height));
	return imageData;
}

vector<uint> loadRgba32fTexture(const string& file_path, uvec2& resolution) {

	int width, height, channels;
	float* data = stbi_loadf(file_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!data) {
		std::cerr << "Failed to load image: " << file_path << std::endl;
	}

	vector<uint> imageData(width * height);
	for (uint i = 0; i < i_to_u(width * height); i++) {
		imageData[i] = data[i];
	}

	resolution = uvec2(i_to_u(width), i_to_u(height));
	return imageData;
}