#pragma once

#include "Shared.hpp"

#include "Nodes.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

#include "GUI_Nodes.hpp"
#include "GUI_Object_Nodes.hpp"

#include "Object/Object.hpp"

// FWD DECL OTHER
struct Log_Console;

struct Scene; // Contains All Data For The Engine

// FWD DECL THIS
namespace CLASS {
	struct File;  // Contains All Data For The Editor
}

// DECL
namespace CLASS {
	enum struct Parse_Type {
		NONE,
		BUILD_STEPS,
		NODE_TREE,
		MATERIAL,
		HEADER,
		OBJECT,
		SCENE,
		DATA
	};

	struct File {
		map<uint64, void*> pointer_map;

		vector<SHADER::Shader*> materials;
		vector<OBJECT::Data*> object_data;
		vector<Node_Tree*> node_trees;
		vector<Object*> objects;
		vector<Scene*> scenes;

		unordered_map<CLASS::Node_Tree*, GUI::NODE::Node_Tree*> nodetree_map;
		unordered_map<CLASS::Node*, GUI::NODE::Node*> node_map;
		Observable_Ptr<Object>* active_object;
		Observable_Ptr<Scene>* active_scene;
		vector<Object*> selected_objects;
		Object* default_camera;

		string version;
		string type;

		File();

		void f_loadAsciiFile(const string& file_path);
		void f_saveAsciiFile(const string& file_path);
		void f_loadBinaryFile(const string& file_path);
		void f_saveBinaryFile(const string& file_path);

		static File f_getAsciiFile(const string& file_path);
		static File f_getBinaryFile(const string& file_path);

		string f_printFile();
		void   f_fileStats() const;

	private:
		void              f_loadAscii           (const vector<vector<string>>& token_data);
		void              f_loadAsciiHeader     (const vector<vector<string>>& token_data);
		CLASS::Node_Tree* f_loadAsciiNodeTree   (const vector<vector<string>>& token_data);
		SHADER::Shader*   f_loadAsciiMaterial   (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiData       (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiAtmosphere (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiPrimitive  (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiSkeleton   (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiCamera     (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiVolume     (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiCurve      (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiEmpty      (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiForce      (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiGroup      (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiLight      (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiMesh       (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiSfx        (const vector<vector<string>>& token_data);
		OBJECT::Data*     f_loadAsciiVfx        (const vector<vector<string>>& token_data);
		Object*           f_loadAsciiObject     (const vector<vector<string>>& token_data);
		Scene*            f_loadAsciiScene      (const vector<vector<string>>& token_data);
		void              f_loadAsciiBuild      (const vector<vector<string>>& token_data);

		void              f_loadBinary          (const vector<Byte>& byte_data);

		void f_saveAscii           (Lace& lace);
		void f_saveAsciiHeader     (Lace& lace);
		void f_saveAsciiNodeTree   (Lace& lace, Node_Tree*              data, const uint64& i);
		void f_saveAsciiMaterial   (Lace& lace, const SHADER::Shader*   data, const uint64& i);
		void f_saveAsciiData       (Lace& lace);
		void f_saveAsciiAtmosphere (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiPrimitive  (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiSkeleton   (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiCamera     (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiVolume     (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiCurve      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiEmpty      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiForce      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiGroup      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiLight      (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiMesh       (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiSfx        (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiVfx        (Lace& lace, const OBJECT::Data*     data, const uint64& i);
		void f_saveAsciiObject     (Lace& lace, const Object*           data, const uint64& i);
		void f_saveAsciiScene      (Lace& lace, const Scene*            data, const uint64& i);
		void f_saveAsciiBuild      (Lace& lace);

		void f_saveBinary           (Bin_Lace& bin);
		void f_saveBinaryHeader     (Bin_Lace& bin);
		void f_saveBinaryNodeTree   (Bin_Lace& bin, Node_Tree*              data);
		void f_saveBinaryMaterial   (Bin_Lace& bin, const SHADER::Shader*   data, const uint64& i);
		void f_saveBinaryData       (Bin_Lace& bin);
		void f_saveBinaryAtmosphere (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryPrimitive  (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinarySkeleton   (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryCamera     (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryVolume     (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryCurve      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryEmpty      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryForce      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryGroup      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryLight      (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryMesh       (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinarySfx        (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryVfx        (Bin_Lace& bin, const OBJECT::Data*     data, const uint64& i);
		void f_saveBinaryObject     (Bin_Lace& bin, const Object*           data, const uint64& i);
		void f_saveBinaryScene      (Bin_Lace& bin, const Scene*            data, const uint64& i);
		void f_saveBinaryBuild      (Bin_Lace& bin);
	};
}