#pragma once

#include "Include.hpp"

#include "Data_Property.hpp"
#include "Session.hpp"
#include "Scene.hpp"
#include "Lace.hpp"
#include "Ops.hpp"

#include "Nodes.hpp"

#include "Object/Object.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

// FWD DECL OTHER
struct Scene;
struct Lace;
struct Bin_Lace;

// FWD DECL THIS
namespace KL {
	struct File;
}

// DECL
namespace KL {
	enum struct Parse_Type {
		NONE,
		HEADER,
		TEXTURE,
		SHADER,
		NODE_TREE,
		PROP,
		OBJECT,
		SCENE,
		BUILD_STEPS
	};

	struct File {
		//TODO store pointer map as old if there are no duplicated pointers for conflict and to avoid re-writing files for versioning systems
		BiMap<uint64, uint64> pointer_map; // hash - pointer_address

		vector<SHADER::Texture*> textures;
		vector<KL::Shader*> shaders;

		vector<OBJECT::Data*> object_data;
		vector<Node_Tree*> node_trees;
		vector<Object*> objects;
		vector<Scene*> scenes;

		Observable_Ptr<Object>* active_object;
		Observable_Ptr<Scene>* active_scene;
		vector<Object*> selected_objects;
		Object* active_camera;

		string version;
		string type;

		File();

		void f_loadAsciiFile   (const string& file_path);
		void f_saveAsciiFile   (const string& file_path);

		void f_loadBinaryFile  (const string& file_path);
		void f_saveBinaryFile  (const string& file_path);

		void f_importAsciiFile (const string& file_path);
		void f_importBinaryFile(const string& file_path); // TODO

		static File f_getAsciiFile(const string& file_path);
		static File f_getBinaryFile(const string& file_path);

		string f_printFile();
		void   f_fileStats() const;

		uint64 f_ptrVal(const void* key) const;
		uint64 f_ptrKey(const void* val) const;

		virtual void              f_loadAscii           (const Token_Array& token_data, const Tokens& line_data);
		virtual void              f_loadAsciiHeader     (const Token_Array& token_data, const Tokens& line_data);
		virtual KL::Node_Tree*    f_loadAsciiNodeTree   (const Token_Array& token_data, const Tokens& line_data);
		virtual SHADER::Texture*  f_loadAsciiTexture    (const Token_Array& token_data, const Tokens& line_data);
		virtual KL::Shader*       f_loadAsciiShader     (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiData       (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiAtmosphere (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiPrimitive  (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiSkeleton   (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiCamera     (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiVolume     (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiCurve      (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiEmpty      (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiForce      (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiGroup      (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiLight      (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiMesh       (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiSfx        (const Token_Array& token_data, const Tokens& line_data);
		virtual OBJECT::Data*     f_loadAsciiVfx        (const Token_Array& token_data, const Tokens& line_data);
		virtual Object*           f_loadAsciiObject     (const Token_Array& token_data, const Tokens& line_data);
		virtual Scene*            f_loadAsciiScene      (const Token_Array& token_data, const Tokens& line_data);
		virtual void              f_loadAsciiHistory    (const Token_Array& token_data, const Tokens& line_Data); // TODO
		virtual void              f_loadAsciiBuild      (const Token_Array& token_data, const Tokens& line_data);

		virtual void              f_loadBinary          (const vector<Byte>& byte_data);

		virtual void              f_importAscii         (const Token_Array& token_data, const Tokens& line_data);

		virtual void f_saveAscii           (Lace& lace);
		virtual void f_saveAsciiHeader     (Lace& lace);
		virtual void f_saveAsciiNodeTree   (Lace& lace, const Node_Tree*        data, const uint64& i);
		virtual void f_saveAsciiTexture    (Lace& lace, const SHADER::Texture*  data, const uint64& i);
		virtual void f_saveAsciiShader     (Lace& lace, const KL::Shader*       data, const uint64& i);
		virtual void f_saveAsciiData       (Lace& lace);
		virtual void f_saveAsciiAtmosphere (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiPrimitive  (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiSkeleton   (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiCamera     (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiVolume     (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiCurve      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiEmpty      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiForce      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiGroup      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiLight      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiMesh       (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiSfx        (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiVfx        (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		virtual void f_saveAsciiObject     (Lace& lace, const Object*           data, const uint64& i);
		virtual void f_saveAsciiScene      (Lace& lace, const Scene*            data, const uint64& i);
		virtual void f_saveAsciiHistory    (Lace& lace); //TODO
		virtual void f_saveAsciiBuild      (Lace& lace);

		virtual void f_saveBinary           (Bin_Lace& bin);
		virtual void f_saveBinaryHeader     (Bin_Lace& bin);
		virtual void f_saveBinaryNodeTree   (Bin_Lace& bin, Node_Tree*            data);
		virtual void f_saveBinaryMaterial   (Bin_Lace& bin, const KL::Shader*   data, const uint64& i) {};
		virtual void f_saveBinaryData       (Bin_Lace& bin) {};
		virtual void f_saveBinaryAtmosphere (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryPrimitive  (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinarySkeleton   (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryCamera     (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryVolume     (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryCurve      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryEmpty      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryForce      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryGroup      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryLight      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryMesh       (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinarySfx        (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryVfx        (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i) {};
		virtual void f_saveBinaryObject     (Bin_Lace& bin, const Object*           data, const uint64& i) {};
		virtual void f_saveBinaryScene      (Bin_Lace& bin, const Scene*            data, const uint64& i) {};
		virtual void f_saveBinaryHistory    (Bin_Lace& bin) {};
		virtual void f_saveBinaryBuild      (Bin_Lace& bin) {};
	};
}