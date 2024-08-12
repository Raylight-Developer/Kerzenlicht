#pragma once

#include "Include.hpp"
#include "Ops.hpp"
#include "Data_Property.hpp"

#include "Nodes/Shader_Nodes.hpp"

// FWD DECL OTHER
namespace CLASS {
	struct File;
}

// FWD DECL THIS

// DECL
namespace SHADER {
	namespace TEXTURE {
		enum struct Format {
			NONE,
			RGBUINT8,
			RGBUINT10,
			RGBFLOAT16,
			RGBFLOAT32,
			RGBAUINT8,
			RGBAUINT10,
			RGBAFLOAT16,
			RGBAFLOAT32,
			MONOUINT8,
			MONOUINT10,
			MONOFLOAT16,
			MONOFLOAT32
		};
	}
	struct Texture {
		string name;
		uvec2 resolution;
		TEXTURE::Format format;
		vector<uint> data;

		Texture();

		bool loadFromFile(const string& file_path);
		static Texture fromFile(const string& file_path);

		vector<uint> toRgba8Texture() const;
	};

	namespace MATERIAL {
		enum struct Type {
			NONE,
			CODE,
			NODES
		};
	}
	struct Material {
		string name;

		MATERIAL::Type type;
		string shader_code;
		CLASS::Node_Tree* node_tree;

		Material();

		static string compileMaterials(const string& code);
	};
}