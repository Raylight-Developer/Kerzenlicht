#pragma once

#include "Core.hpp"

#include "Nodes/Shader_Nodes.hpp"

// FWD DECL OTHER
namespace KL {
	struct File;
}

// FWD DECL THIS

// DECL
namespace KL {
	namespace SHADER {
		enum struct Type: uint16 {
			NONE,
			CODE,
			NODES
		};
		namespace TEXTURE {
			enum struct Format: uint16 {
				NONE,
				RGBA8u,
				RGBA16f,
				RGBA32f,
			};

			enum struct Filtering: uint16 {
				RAW,
				BILINEAR
			};
		}
		struct Texture {
			string name;
			string file_path;
			uvec2 resolution;
			TEXTURE::Format format;
			TEXTURE::Filtering filtering;
			vector<uint> data;

			Texture();

			bool loadFromFile(const string& file_path);
			static Texture fromFile(const string& file_path);

			vector<uint> toPackedRgba8Texture() const;
			vector<unsigned char> toRgba8Texture() const;
		};
	}

	struct Shader {
		string name;

		SHADER::Type type;
		string shader_code;
		vector<Prop> inputs;
		KL::Node_Tree* node_tree;

		Shader();

		static string f_compileShaders(const string& code);
	};
	namespace SHADER_CMD {
		enum struct Type {
			NONE,
			CODE
		};
		struct Shader_Code : History_Command {
			Shader* ptr;
			string cmd_new, cmd_old;

			Shader_Code(Shader* ptr, const string& cmd_new);

			~Shader_Code() {};
			void execute() override;
			History_Command* undo() const override;
			void serialize(Lace& lace) const override;
			void deserialize(const string& value) override;
		};
	}
}