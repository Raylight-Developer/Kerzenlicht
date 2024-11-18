#include "Shader/Shader.hpp"

#include "Utils/Session.hpp"

KL::SHADER::Texture::Texture() :
	name("New Texture")
{
	format = TEXTURE::Format::RGBAUINT8;
	file_path = "";
	resolution = uvec2(0, 0);
	data = {};
}

KL::SHADER::Texture KL::SHADER::Texture::fromFile(const string& file_path) {
	Texture tex;
	tex.loadFromFile(file_path);
	return tex;
}

vector<uint> KL::SHADER::Texture::toRgba8Texture() const {
	vector<uint> packedData;
	for (uint i = 0; i < resolution.x * resolution.y; i++) {
		uint r = data[i * 4 + 0];
		uint g = data[i * 4 + 1];
		uint b = data[i * 4 + 2];
		uint a = data[i * 4 + 3];
		uint rgba = (r << 24) | (g << 16) | (b << 8) | a;
		packedData.push_back(rgba);
	}
	return packedData;
}

KL::Shader::Shader() :
	name("New Material"),
	node_tree(nullptr),
	type(KL::SHADER::Type::NONE)
{}

string KL::Shader::f_compileShaders(const string & code) {
	const string input = "<inputs>[";

	string hooked_code = code;
	vector<string> material_code;

	uint64 id = 0;
	for (KL::Shader* shader : FILE->shaders) {
		string process_shader = shader->shader_code;

		uint64 input_pos = process_shader.find(input);
		while (input_pos != string::npos) {
			uint64 end_pos = process_shader.find("]", input_pos);
			string index_str = process_shader.substr(input_pos + input.length(), end_pos - (input_pos + input.length()));
			uint64 index = str_to_ul(index_str);

			switch (shader->inputs[index].type) {
				case (PROP::Type::TEXTURE) : {
					const string section = "textures[";
					process_shader.replace(input_pos, section.length(), section);
					auto exists = f_getVectorIndex(FILE->textures, shader->inputs[index].getTexture());
					if (exists) {
						process_shader.replace(input_pos + section.length(), index_str.length(), to_string(exists.data));
					}
					break;
				}
			}
			input_pos = process_shader.find("<inputs>[");
		}

		Lace shader_code;
		shader_code ENDL << Lace_TAB() << "else if (hit_data.material == " << id++ << ") {";
		shader_code ENDL << f_addLinesToLine(process_shader, "\t\t");
		shader_code << Lace_TAB() << "}";

		material_code.push_back(shader_code.str());
	}

	const string comp_material_code = f_join(material_code);

	const string toFind = "//---INSERT-SHADERS---//";
	size_t startPos = 0;

	while ((startPos = hooked_code.find(toFind, startPos)) != string::npos) {
		hooked_code.replace(startPos, toFind.length(), comp_material_code);
		startPos += comp_material_code.length();
	}
	return hooked_code;
}

KL::SHADER_CMD::Shader_Code::Shader_Code(Shader* ptr, const string& cmd_new) :
	ptr(ptr),
	cmd_new(cmd_new),
	cmd_old(ptr->shader_code)
{
	type = HISTORY::Type::SHADER;
	sub_type = e_to_us(SHADER_CMD::Type::CODE);
	info << "Shader Code for [" << ptr->name << "] changed";
	details HTML_R << "From" HTML_RESET HTML_ENDL << ptr->shader_code HTML_ENDL HTML_G << "To" HTML_RESET HTML_ENDL << cmd_new;
}

void KL::SHADER_CMD::Shader_Code::execute() {
	ptr->shader_code = cmd_new;
}

KL::History_Command*  KL::SHADER_CMD::Shader_Code::undo() const {
	return new Shader_Code(ptr, cmd_old);
}

void KL::SHADER_CMD::Shader_Code::serialize(Lace& lace) const {
	lace NL << "┌Before";
	lace++;
	lace NL << cmd_old;
	lace--;
	lace NL << "└Before";
	lace NL << "┌After";
	lace++;
	lace NL << cmd_new;
	lace--;
	lace NL << "└After";
}

void KL::SHADER_CMD::Shader_Code::deserialize(const string& value) {
}

#undef FILE
#undef NL
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool KL::SHADER::Texture::loadFromFile(const string & file_path) { // TODO handle different formats
	this->file_path = file_path;
	int width, height, channels;
	unsigned char* tex_data = stbi_load(file_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (tex_data == nullptr) {
		LOG ENDL ANSI_R << "[Texture]" ANSI_RESET << " Failed to load image: " << file_path << " " << stbi_failure_reason();
		return false;
	}
	uint64 totalPixels = width * height * 4;
	data = vector<uint>(totalPixels);
	for (uint64 i = 0; i < totalPixels; ++i) {
		data[i] = static_cast<uint>(tex_data[i]);
	}

	resolution = uvec2((i_to_u(width), (i_to_u(height))));
	stbi_image_free(tex_data);
	return true;
}