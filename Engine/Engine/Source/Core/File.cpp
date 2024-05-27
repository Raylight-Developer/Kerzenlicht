#include "Core/File.hpp"

CLASS::Scene::Scene() {
	objects = vector<CLASS::Object*>();
	active_camera = nullptr;
	current_frame = 0;
}

CLASS::Scene::~Scene() {
}

CLASS::File::File() {
	nodes = vector<CLASS::Node_Tree*>();
	object_data = vector<CLASS::OBJECT::Data*>();
	materials = vector<CLASS::Material*>();
	objects = vector<CLASS::Object*>();
	scenes = vector<CLASS::Scene*>();

	active_object = new Observable_Ptr<Object>();
	active_scene = new Observable_Ptr<Scene>();

	default_camera = new Object();
	default_camera->transform.position = dvec3(2.5, 1.5, 2.5);
	default_camera->transform.euler_rotation = dvec3(15, 225, 0);
	default_camera->data = new OBJECT::Data(OBJECT::DATA::Type::CAMERA, new OBJECT::DATA::Camera());

	version = "ERROR";
}

void CLASS::File::f_loadFile(const string& file_path) {
	ifstream file(file_path, ios::binary);

	if (file.is_open()) {
		vector<vector<string>> data = vector<vector<string>>();
		string line;

		Parse_Type is_processing = Parse_Type::NONE;
		while (getline(file, line)) {
			vector<string> tokens = f_split(line);
			if (!tokens.empty()) {
				if (is_processing == Parse_Type::NONE) {
					if      (tokens[0] == "┌Build-Steps")
						is_processing = Parse_Type::BUILD_STEPS;
					else if (tokens[0] == "┌Node-Tree")
						is_processing = Parse_Type::NODE_TREE;
					else if (tokens[0] == "┌Material")
						is_processing = Parse_Type::MATERIAL;
					else if (tokens[0] == "┌Header")
						is_processing = Parse_Type::HEADER;
					else if (tokens[0] == "┌Object")
						is_processing = Parse_Type::OBJECT;
					else if (tokens[0] == "┌Scene")
						is_processing = Parse_Type::SCENE;
					else if (tokens[0] == "┌Data")
						is_processing = Parse_Type::DATA;
					data.clear();
					data.push_back(tokens);
				}
				else {
					if      (is_processing == Parse_Type::BUILD_STEPS and tokens[0] == "└Build-Steps") {
						is_processing = Parse_Type::NONE;
						f_loadBuild(data);
						cout << ANSI_GREEN << "[Loading]" << ANSI_RESET <<" Build Complete" << endl;
					}
					else if (is_processing == Parse_Type::NODE_TREE and tokens[0] == "└Node-Tree") {
						is_processing = Parse_Type::NONE;
						nodes.push_back(f_loadNodeTree(data));
					}
					else if (is_processing == Parse_Type::MATERIAL and tokens[0] == "└Material") {
						is_processing = Parse_Type::NONE;
						materials.push_back(f_loadMaterial(data));
					}
					else if (is_processing == Parse_Type::HEADER and tokens[0] == "└Header") {
						is_processing = Parse_Type::NONE;
						cout << ANSI_CYAN << "[Loading]" << ANSI_RESET << " Loading: " << file_path << "..." << endl;
						f_loadHeader(data);
					}
					else if (is_processing == Parse_Type::OBJECT and tokens[0] == "└Object") {
						is_processing = Parse_Type::NONE;
						objects.push_back(f_loadObject(data));
					}
					else if (is_processing == Parse_Type::SCENE and tokens[0] == "└Scene") {
						is_processing = Parse_Type::NONE;
						scenes.push_back(f_loadScene(data));
					}
					else if (is_processing == Parse_Type::DATA and tokens[0] == "└Data") {
						is_processing = Parse_Type::NONE;
						object_data.push_back(f_loadData(data));
					}
					else {
						data.push_back(tokens);
					}
				}
			}
		}
	}
	else cout << ANSI_RED << ANSI_BOLD << "[Loading]" << ANSI_RESET << " Error Opening File: " << file_path << endl;
}

void CLASS::File::f_loadHeader(const vector<vector<string>>& token_data) {
}

CLASS::Node_Tree* CLASS::File::f_loadNodeTree(const vector<vector<string>>& token_data) {
	auto node_tree = new CLASS::Node_Tree();
	auto gui_node_tree = new GUI::NODE::Node_Tree();

	bool is_processing = false;
	vector<vector<string>> read_data = vector<vector<string>>();
	for (const vector<string>& tokens : token_data) {
		if (tokens[0] == "┌Node") {
			is_processing = true;
			read_data.clear();
			read_data.push_back(tokens);
		}
		else if (tokens[0] == "└Node") {
			is_processing = false;
			//CLASS::Node* node;
			//GUI::NODE::Node* gui_node;
			//for (const vector<string>& sub_tokens : read_data) {
			//	if (sub_tokens[0] == "┌Node") {
			//		if      (sub_tokens[2] == "CONSTRAINT") {
			//		}
			//		else if (sub_tokens[2] == "GENERATE") {
			//		}
			//		else if (sub_tokens[2] == "PHYSICS") {
			//		}
			//		else if (sub_tokens[2] == "MODIFY") {
			//		}
			//		else if (sub_tokens[2] == "EXEC") {
			//		}
			//		else if (sub_tokens[2] == "LINK") {
			//		}
			//		else if (sub_tokens[2] == "MATH") {
			//		}
			//		else if (sub_tokens[2] == "UTIL") {
			//		}
			//	}
			//}
			//node_tree->nodes.push_back(node);
		}
		else if (tokens[0] == "#Node") {
			CLASS::Node* node;
			GUI::NODE::Node* gui_node;
			if      (tokens[2] == "CONSTRAINT") {
			}
			else if (tokens[2] == "GENERATE") {
			}
			else if (tokens[2] == "PHYSICS") {
			}
			else if (tokens[2] == "MODIFY") {
			}
			else if (tokens[2] == "EXEC") {
				if (tokens[4] == "Tick") {
					node = new NODE::EXEC::Tick();
					gui_node = new GUI::NODE::EXEC::Tick(str_to_i(tokens[9], tokens[10]));
					node_tree->tick = static_cast<NODE::EXEC::Tick*>(node);
				}
			}
			else if (tokens[2] == "LINK") {
			}
			else if (tokens[2] == "MATH") {
				if      (tokens[4] == "Add") {
					node = new NODE::MATH::Add();
					gui_node = new GUI::NODE::MATH::Add(str_to_i(tokens[9], tokens[10]));
				}
				else if (tokens[4] == "Sub") {
					node = new NODE::MATH::Sub();
					gui_node = new GUI::NODE::MATH::Sub(str_to_i(tokens[9], tokens[10]));
				}
				else if (tokens[4] == "Mul") {
					node = new NODE::MATH::Mul();
					gui_node = new GUI::NODE::MATH::Mul(str_to_i(tokens[9], tokens[10]));
				}
				else if (tokens[4] == "Div") {
					node = new NODE::MATH::Div();
					gui_node = new GUI::NODE::MATH::Div(str_to_i(tokens[9], tokens[10]));
				}
			}
			else if (tokens[2] == "UTIL") {
			}
			node_tree->nodes.push_back(node);
			gui_node_tree->nodes.push_back(gui_node);
		}
		else if (tokens[0] == "┌Build-E") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Build-E") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				auto port_l = static_cast<NODE::PORT::Exec_O_Port*>(
					node_tree->nodes[str_to_ul(sub_tokens[0])]->outputs[str_to_ul(sub_tokens[1])]
				);
				auto port_r = static_cast<NODE::PORT::Exec_I_Port*>(
					node_tree->nodes[str_to_ul(sub_tokens[2])]->inputs[str_to_ul(sub_tokens[3])]
				);
				port_l->connection = port_r;
				port_r->incoming_connections.push_back(port_l);

				auto gui_port_l = static_cast<GUI::NODE::PORT::Exec_O_Port*>(
					gui_node_tree->nodes[str_to_ul(sub_tokens[0])]->outputs[str_to_ul(sub_tokens[1])]
				);
				auto gui_port_r = static_cast<GUI::NODE::PORT::Exec_I_Port*>(
					gui_node_tree->nodes[str_to_ul(sub_tokens[2])]->inputs[str_to_ul(sub_tokens[3])]
				);
				gui_port_l->connection = new GUI::NODE::Connection(gui_port_l, gui_port_r);
				gui_port_r->incoming_connections.push_back(gui_port_l->connection);
			}
		}
		else if (tokens[0] == "┌Build-D") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Build-D") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				auto port_l = static_cast<NODE::PORT::Data_O_Port*>(
					node_tree->nodes[str_to_ul(sub_tokens[0])]->outputs[str_to_ul(sub_tokens[1])]
				);
				auto port_r = static_cast<NODE::PORT::Data_I_Port*>(
					node_tree->nodes[str_to_ul(sub_tokens[2])]->inputs[str_to_ul(sub_tokens[3])]
				);
				port_r->connection = port_l;
				port_l->outgoing_connections.push_back(port_r);
				
				auto gui_port_l = static_cast<GUI::NODE::PORT::Data_O_Port*>(
					gui_node_tree->nodes[str_to_ul(sub_tokens[0])]->outputs[str_to_ul(sub_tokens[1])]
				);
				auto gui_port_r = static_cast<GUI::NODE::PORT::Data_I_Port*>(
					gui_node_tree->nodes[str_to_ul(sub_tokens[2])]->inputs[str_to_ul(sub_tokens[3])]
				);
				gui_port_r->connection = new GUI::NODE::Connection(gui_port_l, gui_port_r);
				gui_port_l->outgoing_connections.push_back(gui_port_r->connection);
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
	node_map[node_tree] = gui_node_tree;
	return node_tree;
}

CLASS::Material* CLASS::File::f_loadMaterial(const vector<vector<string>>& token_data) {
	CLASS::Material* material = new CLASS::Material();
	material->name = f_join(token_data[0], " ", 4);
	return material;
}

CLASS::OBJECT::Data* CLASS::File::f_loadData(const vector<vector<string>>& token_data) {
	if      (token_data[0][2] == "Atmosphere")
		return f_loadAtmosphere(token_data);
	else if (token_data[0][2] == "Primitive")
		return f_loadPrimitive (token_data);
	else if (token_data[0][2] == "Skeleton")
		return f_loadSkeleton  (token_data);
	else if (token_data[0][2] == "Camera")
		return f_loadCamera    (token_data);
	else if (token_data[0][2] == "Volume")
		return f_loadVolume    (token_data);
	else if (token_data[0][2] == "Curve")
		return f_loadCurve     (token_data);
	else if (token_data[0][2] == "Empty")
		return f_loadEmpty     (token_data);
	else if (token_data[0][2] == "Force")
		return f_loadForce     (token_data);
	else if (token_data[0][2] == "Group")
		return f_loadGroup     (token_data);
	else if (token_data[0][2] == "Light")
		return f_loadLight     (token_data);
	else if (token_data[0][2] == "Mesh")
		return f_loadMesh      (token_data);
	else if (token_data[0][2] == "Sfx")
		return f_loadSfx       (token_data);
	else if (token_data[0][2] == "Vfx")
		return f_loadVfx       (token_data);
	return new CLASS::OBJECT::Data();
}

CLASS::OBJECT::Data* CLASS::File::f_loadAtmosphere(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::ATMOSPHERE;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadPrimitive(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::PRIMITIVE;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadSkeleton(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::SKELETON;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadCamera(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	CLASS::OBJECT::DATA::Camera* camera = new CLASS::OBJECT::DATA::Camera();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::CAMERA;
	data->data = camera;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadVolume(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::VOLUME;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadCurve(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::CURVE;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadEmpty(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::EMPTY;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadForce(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::FORCE;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadGroup(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	CLASS::OBJECT::DATA::Group* group = new CLASS::OBJECT::DATA::Group();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::GROUP;
	data->data = group;

	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadLight(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::LIGHT;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadMesh(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	CLASS::OBJECT::DATA::Mesh * mesh = new CLASS::OBJECT::DATA::Mesh();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::MESH;
	data->data = mesh;

	bool is_processing = false;
	vector<vector<string>> read_data = vector<vector<string>>();
	for (const vector<string>& tokens : token_data) {
		if (
			tokens[0] == "┌Vertices(" or
			tokens[0] == "┌Faces("
		) {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Vertices") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				mesh->vertices.push_back(new CLASS::OBJECT::DATA::MESH::Vertex(str_to_d(token_data[2], token_data[3], token_data[4])));
			}
		}
		else if (tokens[0] == "└Faces") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				CLASS::OBJECT::DATA::MESH::Face* face = new CLASS::OBJECT::DATA::MESH::Face();
				face->vertices.push_back(mesh->vertices[str_to_ul(token_data[2])]);
				face->vertices.push_back(mesh->vertices[str_to_ul(token_data[3])]);
				face->vertices.push_back(mesh->vertices[str_to_ul(token_data[4])]);
				mesh->faces.push_back(face);
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}

	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadSfx(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::SFX;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadVfx(const vector<vector<string>>& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::VFX;
	return data;
}

CLASS::Object* CLASS::File::f_loadObject(const vector<vector<string>>& token_data) {
	CLASS::Object* object = new CLASS::Object();
	object->name = f_join(token_data[0], " ", 4);
	CLASS::Transform transform;

	bool is_processing = false;
	vector<vector<string>> read_data = vector<vector<string>>();
	for (const vector<string>& tokens : token_data) {
		if (tokens[0] == "Position") {
			transform.position = str_to_d(tokens[1], tokens[2], tokens[3]);
		}
		else if (tokens[0] == "Rotation") {
			transform.euler_rotation = str_to_d(tokens[1], tokens[2], tokens[3]);
		}
		else if (tokens[0] == "Scale") {
			transform.scale = str_to_d(tokens[1], tokens[2], tokens[3]);
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
	object->transform = transform;
	return object;
};

CLASS::Scene* CLASS::File::f_loadScene(const vector<vector<string>>& token_data) {
	CLASS::Scene* scene = new CLASS::Scene();

	bool is_processing = false;
	vector<vector<string>> read_data = vector<vector<string>>();
	for (const vector<string>& tokens : token_data) {
		if (tokens[0] == "Active-Camera") {
			scene->active_camera = objects[str_to_ul(tokens[1])];
		}
		else if (tokens[0] == "┌Objects") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Objects") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				scene->objects.push_back(objects[str_to_ul(sub_tokens[0])]);
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
	return scene;
}

void CLASS::File::f_loadBuild(const vector<vector<string>>& token_data) {
	bool is_processing = false;
	vector<vector<string>> read_data = vector<vector<string>>();
	for (const vector<string>& tokens : token_data) {
		if (tokens[0] == "Active-Scene") {
			active_scene->set(scenes[str_to_ul(tokens[1])]);
		}
		else if (tokens[0] == "┌Object-Group") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Object-Group") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				object_data[str_to_ul(sub_tokens[0])]->getGroup()->objects.push_back(objects[str_to_ul(sub_tokens[1])]);
			}
		}
		else if (tokens[0] == "┌Object-Data") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Object-Data") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				objects[str_to_ul(sub_tokens[0])]->data = object_data[str_to_ul(sub_tokens[1])];
			}
		}
		else if (tokens[0] == "┌Object-Node") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Object-Node") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				objects[str_to_ul(sub_tokens[0])]->nodes = nodes[str_to_ul(sub_tokens[1])];
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
}

void CLASS::File::f_saveFile(const string& file_path) {
	ofstream file(file_path);
	if (file.is_open()) {
		file << f_printFile();
		file.close();
	}
}

#define NL << NL()
#define SP << S() <<
#define A += 1;
#define R -= 1;

string CLASS::File::f_printFile() {
	Lace lace = Lace(" ");
	uint64 count = 0;
	f_saveHeader(lace);

	count = 0;
	lace NL << "┌Materials( " << materials.size() << " )";
	lace A
	for (const CLASS::Material* material : materials)
		f_saveMaterial(lace, material, count++);
	lace R
	lace NL << "└Materials";

	count = 0;
	lace NL << "┌Node-Trees( " << nodes.size() << " )";
	lace A
	for (CLASS::Node_Tree* node : nodes)
		f_saveNodeTree(lace, node, count++);
	lace R
	lace NL << "└Node-Trees";

	lace NL << "┌Data( " << object_data.size() << " )";
	lace A
	f_saveData(lace);
	lace R
	lace NL << "└Data";

	count = 0;
	lace NL << "┌Objects( " << objects.size() << " )";
	lace A
	for (const CLASS::Object* object : objects)
		f_saveObject(lace, object, count++);
	lace R
	lace NL << "└Objects";

	count = 0;
	lace NL << "┌Scenes( " << objects.size() << " )";
	lace A
	for (const CLASS::Scene* scene : scenes)
		f_saveScene(lace, scene, count++);
	lace R
	lace NL << "└Scenes";

	f_saveBuild(lace);

	return lace.str();
}

void CLASS::File::f_saveHeader(Lace& lace) {
	lace NL << "┌Header";
	lace A
		lace NL << "Version 1.0.0";
	lace R;
	lace NL << "└Header";
}

void CLASS::File::f_saveNodeTree(Lace& lace, CLASS::Node_Tree* data, const uint64& i) {
	lace NL << "┌Node-Tree [ " << i << " ]";
	lace A
	lace NL << "┌Nodes";
	lace A
	for (const GUI::NODE::Node* node : node_map[data]->nodes) {
		switch (node->type) {
			case NODE::Type::CONSTRAINT: {
				switch (static_cast<NODE::CONSTRAINT::Type>(node->sub_type)) {
					case NODE::CONSTRAINT::Type::PARENT: {
						break;
					}
				}
				break;
			}
			case NODE::Type::EXEC: {
				switch (static_cast<NODE::EXEC::Type>(node->sub_type)) {
					case NODE::EXEC::Type::TICK: {
						lace NL << "#Node :: EXEC :: Tick [ " << i << " ] ( " << node->rect.topLeft() << " )";
						break;
					}
					case NODE::EXEC::Type::COUNTER: {
						lace NL << "#Node :: EXEC :: Counter [ " << i << " ] ( " << node->rect.topLeft() << " )";
						break;
					}
				}
				break;
			}
			case NODE::Type::MATH: {
				switch (static_cast<NODE::MATH::Type>(node->sub_type)) {
					case NODE::MATH::Type::ADD: {
						lace NL << "#Node :: MATH :: Add [ " << i << " ] ( " << node->rect.topLeft() << " )";
						break;
					}
					case NODE::MATH::Type::SUB: {
						lace NL << "#Node :: MATH :: Sub [ " << i << " ] ( " << node->rect.topLeft() << " )";
						break;
					}
					case NODE::MATH::Type::MUL: {
						lace NL << "#Node :: MATH :: Mul [ " << i << " ] ( " << node->rect.topLeft() << " )";
						break;
					}
					case NODE::MATH::Type::DIV: {
						lace NL << "#Node :: MATH :: Div [ " << i << " ] ( " << node->rect.topLeft() << " )";
						break;
					}
				}
				break;
			}
			case NODE::Type::UTIL: {
				switch (static_cast<NODE::UTIL::Type>(node->sub_type)) {
					case NODE::UTIL::Type::VIEW: {
						lace NL << "#Node :: UTIL :: View [ " << i << " ] ( " << node->rect.topLeft() << " )";
						break;
					}
				}
				break;
			}
		}
	}
	lace R
	lace NL << "└Nodes";
	lace NL << "┌Build-E";
	lace A
	for (uint node_l_id = 0; node_l_id < data->nodes.size(); node_l_id++) {
		const auto node = data->nodes[node_l_id];
		for (const auto& port : node->outputs) {
			if (const auto port_l = dynamic_cast<CLASS::NODE::PORT::Exec_O_Port*>(port)) {
				if (port_l->connection) {
					const auto port_r = port_l->connection;
					const auto node_r = port_r->node;
					const uint port_l_id = distance(node->outputs.begin(), find(node->outputs.begin(), node->outputs.end(), port_l));
					const uint node_r_id = distance(data->nodes.begin(), find(data->nodes.begin(), data->nodes.end(), node_r));
					const uint port_r_id = distance(node_r->inputs.begin(), find(node_r->inputs.begin(), node_r->inputs.end(), port_r));
					lace NL << node_l_id SP port_l_id SP node_r_id SP port_r_id;
				}
			}
		}
	}
	lace R
	lace NL << "└Build-E";
	lace NL << "┌Build-D";
	lace A
	for (uint node_r_id = 0; node_r_id < data->nodes.size(); node_r_id++) {
		const auto node = data->nodes[node_r_id];
		for (const auto& port : node->inputs) {
			if (const auto port_r = dynamic_cast<CLASS::NODE::PORT::Data_I_Port*>(port)) {
				if (port_r->connection) {
					const auto port_l = port_r->connection;
					const auto node_l = port_l->node;
					const uint port_l_id = distance(node_l->outputs.begin(), find(node_l->outputs.begin(), node_l->outputs.end(), port_l));
					const uint node_l_id = distance(data->nodes.begin(), find(data->nodes.begin(), data->nodes.end(), node_l));
					const uint port_r_id = distance(node->inputs.begin(), find(node->inputs.begin(), node->inputs.end(), port_r));
					lace NL << node_l_id SP port_l_id SP node_r_id SP port_r_id;
				}
			}
		}
	}
	lace R
	lace NL << "└Build-D";
	lace R
	lace NL << "└Node-Tree";
}

void CLASS::File::f_saveMaterial(Lace& lace, const CLASS::Material* data, const uint64& i) {
	lace NL << "┌Material [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Material";
}

void CLASS::File::f_saveData(Lace& lace) {
	for (uint64 i = 0; i < object_data.size(); i++) {
		switch (object_data[i]->type) {
			case CLASS::OBJECT::DATA::Type::ATMOSPHERE: f_saveAtmosphere(lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::PRIMITIVE:  f_savePrimitive (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::SKELETON:   f_saveSkeleton  (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::CAMERA:     f_saveCamera    (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::VOLUME:     f_saveVolume    (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::CURVE:      f_saveCurve     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::EMPTY:      f_saveEmpty     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::FORCE:      f_saveForce     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::GROUP:      f_saveGroup     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::LIGHT:      f_saveLight     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::MESH:       f_saveMesh      (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::SFX:        f_saveSfx       (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::VFX:        f_saveVfx       (lace, object_data[i], i); break;
		}
	}
}

void CLASS::File::f_saveAtmosphere(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Atmosphere [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Atmosphere";
}

void CLASS::File::f_savePrimitive(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Primitve [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Primitive";
}

void CLASS::File::f_saveSkeleton(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Skeleton [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Skeleton";
}

void CLASS::File::f_saveCamera(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	const CLASS::OBJECT::DATA::Camera* camera = static_cast<CLASS::OBJECT::DATA::Camera*>(data->data);
	lace NL << "┌Data :: Camera [ " << i << " ] " << data->name;
	lace A
	lace NL << "Fov " << camera->focal_angle;
	lace R
	lace NL << "└Data :: Camera";
}

void CLASS::File::f_saveVolume(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Volume [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Volume";
}

void CLASS::File::f_saveCurve(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Curve [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Curve";
}

void CLASS::File::f_saveEmpty(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Empty [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Empty";
}

void CLASS::File::f_saveForce(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Force [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Force";
}

void CLASS::File::f_saveGroup(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Group [ " << i << " ] " << data->name;
	lace NL << "└Data :: Group";
}

void CLASS::File::f_saveLight(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Light [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Light";
}

void CLASS::File::f_saveMesh(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	const CLASS::OBJECT::DATA::Mesh* mesh = static_cast<CLASS::OBJECT::DATA::Mesh*>(data->data);
	lace NL << "┌Data :: Mesh [ " << i << " ] " << data->name;
	lace A
	lace NL << "┌Vertices( " << mesh->vertices.size() << " )";
	lace A
	for (uint64 i = 0; i < mesh->vertices.size(); i++) {
		lace NL << i << " ( " << mesh->vertices[i]->position << " )";
	}
	lace R
	lace NL << "└Vertices";
	lace NL << "┌Faces( " << mesh->faces.size() << " )";
	lace A
	for (uint64 i = 0; i < mesh->faces.size(); i++) {
		lace NL << i << " [ ";
		for (uint64 j = 0; j < mesh->faces[i]->vertices.size(); j++)
			for (uint64 k = 0; k < mesh->vertices.size(); k++)
				if (mesh->faces[i]->vertices[j] == mesh->vertices[k])
					lace << k << " ";
		lace << "]";
	}
	lace R
	lace NL << "└Faces";
	lace R
	lace NL << "└Data :: Mesh";
}

void CLASS::File::f_saveSfx(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Sfx [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Sfx";
}

void CLASS::File::f_saveVfx(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL << "┌Data :: Vfx [ " << i << " ] " << data->name;
	lace A
	lace NL << "";
	lace R
	lace NL << "└Data :: Vfx";
}

void CLASS::File::f_saveObject(Lace& lace, const CLASS::Object* data, const uint64& i) {
	lace NL << "┌Object [ " << i << " ] " << data->name;
	lace A
	lace NL << "Position " << data->transform.position;
	lace NL << "Rotation " << data->transform.euler_rotation;
	lace NL << "Scale " << data->transform.scale;
	switch (data->data->type) {
		case OBJECT::DATA::Type::ATMOSPHERE: lace NL << "Type " << "Atmosphere"; break;
		case OBJECT::DATA::Type::PRIMITIVE : lace NL << "Type " << "Primitive";  break;
		case OBJECT::DATA::Type::SKELETON  : lace NL << "Type " << "Skeleton";   break;
		case OBJECT::DATA::Type::CAMERA    : lace NL << "Type " << "Camera";     break;
		case OBJECT::DATA::Type::VOLUME    : lace NL << "Type " << "Volume";     break;
		case OBJECT::DATA::Type::CURVE     : lace NL << "Type " << "Curve";      break;
		case OBJECT::DATA::Type::EMPTY     : lace NL << "Type " << "Empty";      break;
		case OBJECT::DATA::Type::FORCE     : lace NL << "Type " << "Froce";      break;
		case OBJECT::DATA::Type::GROUP     : lace NL << "Type " << "Group";      break;
		case OBJECT::DATA::Type::LIGHT     : lace NL << "Type " << "Light";      break;
		case OBJECT::DATA::Type::MESH      : lace NL << "Type " << "Mesh";       break;
		case OBJECT::DATA::Type::SFX       : lace NL << "Type " << "Sfx";        break;
		case OBJECT::DATA::Type::VFX       : lace NL << "Type " << "Vfx";        break;
		case OBJECT::DATA::Type::NONE      : lace NL << "Type " << "NONE";       break;
	}
	lace R
	lace NL << "└Object";
}

void CLASS::File::f_saveScene(Lace& lace, const CLASS::Scene* data, const uint64& i) {
	lace NL << "┌Scene [ " << i << " ]";
	lace A
	lace NL << "┌Objects";
	lace A
	for (uint64 i = 0; i < data->objects.size(); i++)
		for (uint64 j = 0; j < objects.size(); j++)
			if (objects[j] == data->objects[i])
				lace NL << j << " " << objects[j]->name;
	lace R
	lace NL << "└Objects";
	lace R
	lace NL << "└Scene";
}

void CLASS::File::f_saveBuild(Lace& lace) {
	lace NL << "┌Build-Steps";
	lace A
	const uint scene_id = distance(scenes.begin(), find(scenes.begin(), scenes.end(), active_scene->ptr));
	lace NL << "Active-Scene " << scene_id;
	lace NL << "┌Object-Group";
	lace A
	for (uint64 obj_group_id = 0; obj_group_id < objects.size(); obj_group_id++) {
		if (objects[obj_group_id]->data->type == OBJECT::DATA::Type::GROUP) {
			for (const auto object : objects[obj_group_id]->data->getGroup()->objects) {
				const uint obj_id = distance(objects.begin(), find(objects.begin(), objects.end(), object));
				lace NL << obj_group_id SP obj_id;
			}
		}
	}
	lace R
	lace NL << "└Object-Group";
	lace NL << "┌Object-Data";
	lace A
	for (uint64 obj_id = 0; obj_id < objects.size(); obj_id++) {
		if (objects[obj_id]->data->type != OBJECT::DATA::Type::NONE and objects[obj_id]->data) {
			const uint data_id = distance(object_data.begin(), find(object_data.begin(), object_data.end(), objects[obj_id]->data));
			lace NL << obj_id SP data_id;
		}
	}
	lace R
	lace NL << "└Object-Data";
	lace NL << "┌Object-Node";
	lace A
	for (uint64 obj_id = 0; obj_id < objects.size(); obj_id++) {
		if (objects[obj_id]->data->type != OBJECT::DATA::Type::NONE and objects[obj_id]->nodes) {
			const uint node_id = distance(nodes.begin(), find(nodes.begin(), nodes.end(), objects[obj_id]->nodes));
			lace NL << obj_id SP node_id;
		}
	}
	lace R
	lace NL << "└Object-Node";
	lace R
	lace NL << "└Build-Steps";
}