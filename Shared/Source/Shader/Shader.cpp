#include "Shader/Shader.hpp"

#ifdef COMPILE_EDITOR
#include "Core/Editor_File.hpp"
#elif COMPILE_RENDERER
#include "Core/Render_File.hpp"
#else
#include "File.hpp"
#endif

#include "Session.hpp"

KL::SHADER::Texture::Texture() :
	name("New Texture")
{
	format = TEXTURE::Format::RGBAUINT8;
}

#define STB_IMAGE_IMPLEMENTATION
#undef FILE
#include "stb_image.h"

bool KL::SHADER::Texture::loadFromFile(const string & file_path) {
	int width, height, channels;
	unsigned char* tex_data = stbi_load(file_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (tex_data == nullptr) {
		LOG << ENDL << ANSI_R << "[Texture]" << ANSI_RESET << " Failed to load image: " << file_path << " " << stbi_failure_reason();
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

#undef STB_IMAGE_IMPLEMENTATION
#undef FILE
#define FILE Session::getInstance().getFile()

KL::Shader::Shader() :
	name("New Material")
{}

string KL::Shader::compileMaterials(const string & code) {
	string hooked_code = code;
	vector<string> material_code;

	uint64 id = 0;
	for (KL::Shader* material : FILE->materials) {
		Lace shader_code;
		shader_code << ENDL << Lace_TAB() << "else if (hit_data.material == " << id++ << ") {";
		shader_code << ENDL << f_prependToLine(material->shader_code, "\t\t");
		shader_code << Lace_TAB() << "}";
		cout << material->shader_code;

		material_code.push_back(shader_code.str());
	}

	const string comp_material_code = f_join(material_code);

	const string toFind = "//---INSERT-SHADERS---//";
	size_t startPos = 0;

	while ((startPos = hooked_code.find(toFind, startPos)) != string::npos) {
		hooked_code.replace(startPos, toFind.length(), comp_material_code);
		startPos += comp_material_code.length();
	}

	LOG << ENDL << comp_material_code; FLUSH;
	return hooked_code;
}