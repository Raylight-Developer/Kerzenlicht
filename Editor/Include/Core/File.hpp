#pragma once

#include "Shared.hpp"

#include "Node/Nodes.hpp"
#include "Node/Node_Def.hpp"

#include "Node_GUI.hpp"
#include "Node_GUI_Def.hpp"

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
		vector<OBJECT::Data*> object_data;
		vector<Material*> materials;
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

		File();

		void   f_loadFile(const string& file_path);
		void   f_saveFile(const string& file_path);
		string f_printFile();

		void              f_loadHeader     (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		CLASS::Node_Tree* f_loadNodeTree   (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		Material*         f_loadMaterial   (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadData       (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadAtmosphere (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadPrimitive  (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadSkeleton   (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadCamera     (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadVolume     (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadCurve      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadEmpty      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadForce      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadGroup      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadLight      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadMesh       (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadSfx        (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		OBJECT::Data*     f_loadVfx        (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		Object*           f_loadObject     (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		Scene*            f_loadScene      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);
		void              f_loadBuild      (const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map);

		void f_saveHeader     (Lace& lace);
		void f_saveNodeTree   (Lace& lace, Node_Tree*          data     , const uint64& i);
		void f_saveMaterial   (Lace& lace, const Material*     data     , const uint64& i);
		void f_saveData       (Lace& lace);
		void f_saveAtmosphere (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_savePrimitive  (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveSkeleton   (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveCamera     (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveVolume     (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveCurve      (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveEmpty      (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveForce      (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveGroup      (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveLight      (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveMesh       (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveSfx        (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveVfx        (Lace& lace, const OBJECT::Data* data     , const uint64& i);
		void f_saveObject     (Lace& lace, const Object*       data     , const uint64& i);
		void f_saveScene      (Lace& lace, const Scene*        data     , const uint64& i);
		void f_saveBuild      (Lace& lace);
	};
}

template <typename T>
string ptr_to_str(const T pointer) {
	return "* " + to_string(reinterpret_cast<uint64>(pointer));
}