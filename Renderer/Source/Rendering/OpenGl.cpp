#include "Rendering/OpenGl.hpp"

#include "Session.hpp"
#include "Shader/Shader.hpp"

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
	string compute_code = preprocessShader("./Resources/Shaders/" + file_path + ".comp");
	compute_code = SHADER::Material::compileMaterials(compute_code);
	LOG << ENDL << compute_code;
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