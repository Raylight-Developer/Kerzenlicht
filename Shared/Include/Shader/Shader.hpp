#pragma once

#include "Include.hpp"
#include "Ops.hpp"
#include "Data_Property.hpp"

#include "History.hpp"

#include "Nodes/Shader_Nodes.hpp"

// FWD DECL OTHER
namespace KL {
	struct File;
}

// FWD DECL THIS

// DECL
namespace KL {
	namespace SHADER {
		enum struct Type {
			NONE,
			CODE,
			NODES
		};
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
			string file_path;
			uvec2 resolution;
			TEXTURE::Format format;
			vector<uint> data;

			Texture();

			bool loadFromFile(const string& file_path);
			static Texture fromFile(const string& file_path);

			vector<uint> toRgba8Texture() const;
		};
	}

	struct Shader {
		string name;

		SHADER::Type type;
		string shader_code;
		vector<Data> inputs;
		KL::Node_Tree* node_tree;

		Shader();

		static string f_compileShaders(const string& code);
	};
	namespace SHADER_CMD {
		struct Shader_Code : History_Command {
			Shader_Code(Shader* ptr, const string& cmd_new);

			~Shader_Code() {};
			void execute() override;
			unique_ptr<History_Command> invert() const override;

		private:
			Shader* ptr;
			string cmd_new, cmd_old;
		};
	}
}