#include "Core/File.hpp"

#include "Log_Console.hpp"
#include "Core/Session.hpp"

CLASS::File::File() {
	node_trees = vector<CLASS::Node_Tree*>();
	object_data = vector<CLASS::OBJECT::Data*>();
	materials = vector<SHADER::Material*>();
	objects = vector<CLASS::Object*>();
	scenes = vector<CLASS::Scene*>();

	active_object = new Observable_Ptr<Object>();
	active_scene = new Observable_Ptr<Scene>();

	default_camera = new Object();
	default_camera->transform.position = dvec3(2.5, 1.5, 2.5);
	default_camera->transform.euler_rotation = dvec3(-20, 45, 0);
	default_camera->data = new OBJECT::Data(OBJECT::DATA::Type::CAMERA, new OBJECT::DATA::Camera());

	version = "ERROR";
}

void CLASS::File::f_loadFile(const string& file_path) {
	ifstream file(file_path, ios::binary);
	map<uint64, void*> pointer_map;

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
						*LOG << ENDL << HTML_GREEN << "[File]" << HTML_RESET << " Reading: Build..."; FLUSH;
						is_processing = Parse_Type::NONE;
						f_loadBuild(data, pointer_map);
					}
					else if (is_processing == Parse_Type::NODE_TREE and tokens[0] == "└Node-Tree") {
						*LOG << ENDL << HTML_BLUE << "[File]" << HTML_RESET << " Reading: Node-Tree..."; FLUSH;
						is_processing = Parse_Type::NONE;
						node_trees.push_back(f_loadNodeTree(data, pointer_map));
					}
					else if (is_processing == Parse_Type::MATERIAL and tokens[0] == "└Material") {
						*LOG << ENDL << HTML_BLUE << "[File]" << HTML_RESET << " Reading: Material..."; FLUSH;
						is_processing = Parse_Type::NONE;
						materials.push_back(f_loadMaterial(data, pointer_map));
					}
					else if (is_processing == Parse_Type::HEADER and tokens[0] == "└Header") {
						*LOG << ENDL << HTML_MAGENTA << "[File]" << HTML_RESET << " Loading: " << file_path << "..."; FLUSH;
						is_processing = Parse_Type::NONE;
						f_loadHeader(data, pointer_map);
					}
					else if (is_processing == Parse_Type::OBJECT and tokens[0] == "└Object") {
						*LOG << ENDL << HTML_BLUE << "[File]" << HTML_RESET << " Reading: Object..."; FLUSH;
						is_processing = Parse_Type::NONE;
						objects.push_back(f_loadObject(data, pointer_map));
					}
					else if (is_processing == Parse_Type::SCENE and tokens[0] == "└Scene") {
						*LOG << ENDL << HTML_BLUE << "[File]" << HTML_RESET << " Reading: Scene..."; FLUSH;
						is_processing = Parse_Type::NONE;
						scenes.push_back(f_loadScene(data, pointer_map));
					}
					else if (is_processing == Parse_Type::DATA and tokens[0] == "└Data") {
						*LOG << ENDL << HTML_BLUE << "[File]" << HTML_RESET << " Reading: Data..."; FLUSH;
						is_processing = Parse_Type::NONE;
						object_data.push_back(f_loadData(data, pointer_map));
					}
					else {
						data.push_back(tokens);
					}
				}
			}
		}
	}
	else {
		*LOG << ENDL << HTML_RED << "[File]" << HTML_RESET << " Error Opening File"; FLUSH;
	}
	*LOG << ENDL << HTML_GREEN << "[File]" << HTML_RESET << " Loaded"; FLUSH;
	#ifdef LOG2
		cout << endl << "Objects     [" << objects.size() << "]";
		cout << endl << "Object Data [" << object_data.size() << "]";
		cout << endl << "Pointer_Map [" << pointer_map.size() << "]";
	#endif
}

void CLASS::File::f_loadHeader(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	for (const vector<string>& tokens : token_data) {
		if (tokens[0] == "Version") {
			version = tokens[1];
		}
	}
}

CLASS::Node_Tree* CLASS::File::f_loadNodeTree(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
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
		else if (tokens[0] == "┌Build-E") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "┌Build-D") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Node") {
			is_processing = false;
			CLASS::Node* node = nullptr;
			GUI::NODE::Node* gui_node = nullptr;
			if      (read_data[0][2] == "CONSTRAINT") {
			}
			else if (read_data[0][2] == "GENERATE") {
			}
			else if (read_data[0][2] == "PHYSICS") {
			}
			else if (read_data[0][2] == "MODIFY") {
			}
			else if (read_data[0][2] == "EXEC") {
				if (read_data[0][4] == "SCRIPT") {
					node = new NODE::EXEC::Script(f_join(read_data[3]));
					gui_node = new GUI::NODE::EXEC::Script(str_to_i(read_data[2][1], read_data[2][2]), f_join(read_data[3]));
				}
				else if (read_data[0][4] == "TICK") {
					node = new NODE::EXEC::Tick();
					gui_node = new GUI::NODE::EXEC::Tick(str_to_i(read_data[2][1], read_data[2][2]));
					node_tree->tick = static_cast<NODE::EXEC::Tick*>(node);
				}
			}
			else if (read_data[0][2] == "LINK") {
				if      (read_data[0][4] == "POINTER") {
					auto pointer = new NODE::LINK::Pointer();
					pointer->pointer_type = static_cast<DATA::Type>(str_to_u(read_data[3][1]));
					node = pointer;
					gui_node = new GUI::NODE::LINK::Pointer(str_to_i(read_data[2][1], read_data[2][2]), static_cast<DATA::Type>(str_to_u(read_data[3][1])));
				}
				else if (read_data[0][4] == "GET") {
					if      (read_data[0][6] == "FIELD") {
						auto field = new NODE::LINK::GET::Field();
						field->field = f_join(read_data[3]);
						node = field;
						auto gui_field = new GUI::NODE::LINK::GET::Field(str_to_i(read_data[2][1], read_data[2][2]));
						gui_node = gui_field;
						gui_field->field->setText(QString::fromStdString(f_join(read_data[3])));
					}
				}
				else if (read_data[0][4] == "SET") {
					if      (read_data[0][6] == "TRANSFORM") {
						if      (read_data[0][8] == "EULER_ROTATION") {
							if      (read_data[0][10] == "X") {
								node = new NODE::LINK::SET::Euler_Rotation_X();
								gui_node = new GUI::NODE::LINK::SET::Euler_Rotation_X(str_to_i(read_data[2][1], read_data[2][2]));
							}
						}
					}
				}
			}
			else if (read_data[0][2] == "MATH") {
				if      (read_data[0][4] == "ADD") {
					node = new NODE::MATH::Add();
					gui_node = new GUI::NODE::MATH::Add(str_to_i(read_data[2][1], read_data[2][2]));
				}
				else if (read_data[0][4] == "SUB") {
					node = new NODE::MATH::Sub();
					gui_node = new GUI::NODE::MATH::Sub(str_to_i(read_data[2][1], read_data[2][2]));
				}
				else if (read_data[0][4] == "MUL") {
					node = new NODE::MATH::Mul();
					gui_node = new GUI::NODE::MATH::Mul(str_to_i(read_data[2][1], read_data[2][2]));
				}
				else if (read_data[0][4] == "DIV") {
					node = new NODE::MATH::Div();
					gui_node = new GUI::NODE::MATH::Div(str_to_i(read_data[2][1], read_data[2][2]));
				}
			}
			else if (read_data[0][2] == "UTIL") {
				if (read_data[0][4] == "PRINT") {
					node = new NODE::UTIL::Print();
					gui_node = new GUI::NODE::UTIL::Print(str_to_i(read_data[2][1], read_data[2][2]));
				}
			}
			else {
				// TODO load "unkown" node from newer version
			}

			pointer_map[str_to_ul(read_data[1][1])] = node;
			gui_node_tree->nodes.push_back(gui_node);
			node_tree->nodes.push_back(node);
			node_map[node] = gui_node;
		}
		else if (tokens[0] == "└Build-E") {
			*LOG << ENDL << "    Loading Build-E..."; FLUSH;
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				auto port_l = static_cast<NODE::PORT::Exec_O_Port*>(
					static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])])->outputs[str_to_ul(sub_tokens[2])]
				);
				auto port_r = static_cast<NODE::PORT::Exec_I_Port*>(
					static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])])->inputs[str_to_ul(sub_tokens[3])]
				);
				port_l->connection = port_r;
				port_r->incoming_connections.push_back(port_l);

				auto gui_port_l = static_cast<GUI::NODE::PORT::Exec_O_Port*>(
					node_map[static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])])]->outputs[str_to_ul(sub_tokens[2])]
				);
				auto gui_port_r = static_cast<GUI::NODE::PORT::Exec_I_Port*>(
					node_map[static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])])]->inputs[str_to_ul(sub_tokens[3])]
				);
				gui_port_l->connection = new GUI::NODE::Connection(gui_port_l, gui_port_r);
				gui_port_r->incoming_connections.push_back(gui_port_l->connection);
			}
		}
		else if (tokens[0] == "└Build-D") {
			*LOG << ENDL << "    Loading Build-D..."; FLUSH;
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				auto port_l = static_cast<NODE::PORT::Data_O_Port*>(
					static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])])->outputs[str_to_ul(sub_tokens[2])]
					);
				auto port_r = static_cast<NODE::PORT::Data_I_Port*>(
					static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])])->inputs[str_to_ul(sub_tokens[3])]
					);
				port_r->connection = port_l;
				port_l->outgoing_connections.push_back(port_r);

				auto gui_port_l = static_cast<GUI::NODE::PORT::Data_O_Port*>(
					node_map[static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])])]->outputs[str_to_ul(sub_tokens[2])]
					);
				auto gui_port_r = static_cast<GUI::NODE::PORT::Data_I_Port*>(
					node_map[static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])])]->inputs[str_to_ul(sub_tokens[3])]
					);
				gui_port_r->connection = new GUI::NODE::Connection(gui_port_l, gui_port_r);
				gui_port_l->outgoing_connections.push_back(gui_port_r->connection);
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
	this->nodetree_map[node_tree] = gui_node_tree;
	pointer_map[str_to_ul(token_data[1][1])] = node_tree;
	return node_tree;
}

SHADER::Material* CLASS::File::f_loadMaterial(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	SHADER::Material* material = new SHADER::Material();
	material->name = f_join(token_data[0], " ", 4);
	return material;
}

CLASS::OBJECT::Data* CLASS::File::f_loadData(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	if      (token_data[0][2] == "Atmosphere")
		return f_loadAtmosphere(token_data, pointer_map);
	else if (token_data[0][2] == "Primitive")
		return f_loadPrimitive (token_data, pointer_map);
	else if (token_data[0][2] == "Skeleton")
		return f_loadSkeleton  (token_data, pointer_map);
	else if (token_data[0][2] == "Camera")
		return f_loadCamera    (token_data, pointer_map);
	else if (token_data[0][2] == "Volume")
		return f_loadVolume    (token_data, pointer_map);
	else if (token_data[0][2] == "Curve")
		return f_loadCurve     (token_data, pointer_map);
	else if (token_data[0][2] == "Empty")
		return f_loadEmpty     (token_data, pointer_map);
	else if (token_data[0][2] == "Force")
		return f_loadForce     (token_data, pointer_map);
	else if (token_data[0][2] == "Group")
		return f_loadGroup     (token_data, pointer_map);
	else if (token_data[0][2] == "Light")
		return f_loadLight     (token_data, pointer_map);
	else if (token_data[0][2] == "Mesh")
		return f_loadMesh      (token_data, pointer_map);
	else if (token_data[0][2] == "Sfx")
		return f_loadSfx       (token_data, pointer_map);
	else if (token_data[0][2] == "Vfx")
		return f_loadVfx       (token_data, pointer_map);
	return new CLASS::OBJECT::Data();
}

CLASS::OBJECT::Data* CLASS::File::f_loadAtmosphere(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::ATMOSPHERE;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadPrimitive(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::PRIMITIVE;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadSkeleton(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::SKELETON;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadCamera(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	CLASS::OBJECT::DATA::Camera* camera = new CLASS::OBJECT::DATA::Camera();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::CAMERA;
	data->data = camera;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadVolume(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::VOLUME;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadCurve(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::CURVE;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadEmpty(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::EMPTY;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadForce(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::FORCE;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadGroup(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	CLASS::OBJECT::DATA::Group* group = new CLASS::OBJECT::DATA::Group();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::GROUP;
	data->data = group;

	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadLight(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::LIGHT;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadMesh(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
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
			tokens[0] == "┌Normals(" or
			tokens[0] == "┌Faces(" or
			tokens[0] == "┌UVs("
		) {
			is_processing = true;
			read_data.clear();
		}
		else if (
			tokens[0] == "┌Vertex-Group"
		) {
			is_processing = true;
			read_data.clear();
			read_data.push_back(tokens);
		}
		else if (tokens[0] == "└Vertices") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				mesh->vertices.push_back(new CLASS::OBJECT::DATA::MESH::Vertex(str_to_d(token_data[1], token_data[2], token_data[3])));
			}
		}
		else if (tokens[0] == "└Vertex-Group") {
			is_processing = false;
			for (const string& index : read_data[1]) {
				mesh->vertex_groups[f_join(read_data[0], " ", 4)].push_back(mesh->vertices[str_to_d(index)]);
			}
		}
		else if (tokens[0] == "└Faces") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				CLASS::OBJECT::DATA::MESH::Face* face = new CLASS::OBJECT::DATA::MESH::Face();
				for (uint i = 0; i < str_to_u(token_data[1]); i++) {
					face->vertices.push_back(mesh->vertices[str_to_ul(token_data[2+i])]);
				}
				mesh->faces.push_back(face);
			}
		}
		else if (tokens[0] == "└Normals") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				for (uint i = 0; i < str_to_u(token_data[1]); i ++) {
					mesh->normals[mesh->faces[str_to_ul(token_data[0])]].push_back(str_to_d(token_data[2+i*3], token_data[3+i*3], token_data[4+i*3]));
				}
			}
		}
		else if (tokens[0] == "└UVs") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				for (uint i = 0; i < str_to_u(token_data[1]); i ++) {
					mesh->uvs[mesh->faces[str_to_ul(token_data[0])]].push_back(str_to_d(token_data[2+i*2], token_data[3+i*2]));
				}
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadSfx(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::SFX;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadVfx(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], " ", 6);
	data->type = CLASS::OBJECT::DATA::Type::VFX;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::Object* CLASS::File::f_loadObject(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
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
	pointer_map[str_to_ul(token_data[1][1])] = object;
	return object;
};

CLASS::Scene* CLASS::File::f_loadScene(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
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
				scene->objects.push_back(static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[1])]));
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
	pointer_map[str_to_ul(token_data[1][1])] = scene;
	return scene;
}

void CLASS::File::f_loadBuild(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	bool is_processing = false;
	vector<vector<string>> read_data = vector<vector<string>>();
	for (const vector<string>& tokens : token_data) {
		if (tokens[0] == "Active-Scene") {
			active_scene->set(static_cast<CLASS::Scene*>(pointer_map[str_to_ul(tokens[2])]));
		}
		else if (tokens[0] == "┌Data-Group") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Data-Group") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				static_cast<CLASS::OBJECT::Data*>(pointer_map[str_to_ul(sub_tokens[1])])->getGroup()->objects.push_back(static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[3])]));
			}
		}
		else if (tokens[0] == "┌Object-Data") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Object-Data") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[1])])->data = static_cast<CLASS::OBJECT::Data*>(pointer_map[str_to_ul(sub_tokens[3])]);
			}
		}
		else if (tokens[0] == "┌Object-Node") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Object-Node") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[1])])->node_tree = static_cast<CLASS::Node_Tree*>(pointer_map[str_to_ul(sub_tokens[3])]);
			}
		}
		else if (tokens[0] == "┌Node-Pointer") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Node-Pointer") {
			is_processing = false;
			for (const vector<string>& sub_tokens : read_data) {
				auto ptr = static_cast<NODE::LINK::Pointer*>(pointer_map[str_to_ul(sub_tokens[1])]);
				ptr->pointer = static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[3])]);

				auto gui_ptr = static_cast<GUI::NODE::LINK::Pointer*>(node_map.at(ptr));
				gui_ptr->pointer = static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[3])]);
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

#define NL << NL() <<
#define SP << S() <<
#define A += 1;
#define R -= 1;

string CLASS::File::f_printFile() {
	Lace lace = Lace(" ");
	uint64 count = 0;
	f_saveHeader(lace);

	count = 0;
	lace NL "┌Materials( " << materials.size() << " )";
	lace A
	for (const SHADER::Material* material : materials)
		f_saveMaterial(lace, material, count++);
	lace R
	lace NL "└Materials";

	count = 0;
	lace NL "┌Node-Trees( " << node_trees.size() << " )";
	lace A
	for (CLASS::Node_Tree* node : node_trees)
		f_saveNodeTree(lace, node, count++);
	lace R
	lace NL "└Node-Trees";

	lace NL "┌Data( " << object_data.size() << " )";
	lace A
	f_saveData(lace);
	lace R
	lace NL "└Data";

	count = 0;
	lace NL "┌Objects( " << objects.size() << " )";
	lace A
	for (const CLASS::Object* object : objects)
		f_saveObject(lace, object, count++);
	lace R
	lace NL "└Objects";

	count = 0;
	lace NL "┌Scenes( " << objects.size() << " )";
	lace A
	for (const CLASS::Scene* scene : scenes)
		f_saveScene(lace, scene, count++);
	lace R
	lace NL "└Scenes";

	f_saveBuild(lace);

	return lace.str();
}

void CLASS::File::f_saveHeader(Lace& lace) {
	lace NL "┌Header";
	lace A
		lace NL "Version 1.0.0";
	lace R;
	lace NL "└Header";
}

void CLASS::File::f_saveNodeTree(Lace& lace, CLASS::Node_Tree* data, const uint64& i) {
	uint64 j = 0;
	lace NL "┌Node-Tree [ " << i << " ]";
	lace A
		lace NL ptr_to_str(data);
	lace NL "┌Nodes( " << data->nodes.size() << " )";
	lace A
		for (CLASS::Node* node : data->nodes) {
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
				case NODE::EXEC::Type::COUNTER: {
					lace NL "┌Node :: EXEC :: COUNTER [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::EXEC::Type::SCRIPT: {
					lace NL "┌Node :: EXEC :: SCRIPT [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL S() << static_cast<CLASS::NODE::EXEC::Script*>(node)->script_id;
					lace NL "└Node";
					break;
				}
				case NODE::EXEC::Type::TICK: {
					lace NL "┌Node :: EXEC :: TICK [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				}
				break;
			}
			case NODE::Type::LINK: {
				switch (static_cast<NODE::LINK::Type>(node->sub_type)) {
				case NODE::LINK::Type::POINTER: {
					lace NL "┌Node :: LINK :: POINTER [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL S() << "Type " << e_to_u(static_cast<NODE::LINK::Pointer*>(node)->pointer_type);
					lace NL "└Node";
					break;
				}
				case NODE::LINK::Type::GET: {
					switch (static_cast<CLASS::NODE::LINK::Get*>(node)->mini_type) {
					case NODE::LINK::GET::Type::FIELD: {
						lace NL "┌Node :: LINK :: GET :: FIELD [ " << j++ << " ]";
						lace NL S() << ptr_to_str(node);
						lace NL S() << "Pos " << node_map[node]->scenePos();
						lace NL S() << static_cast<NODE::LINK::GET::Field*>(node)->field;
						lace NL "└Node";
						break;
					}
					}
					break;
				}
				case NODE::LINK::Type::SET: {
					switch (static_cast<CLASS::NODE::LINK::Set*>(node)->mini_type) {
					case NODE::LINK::SET::Type::EULER_ROTATION_X: {
						lace NL "┌Node :: LINK :: SET :: TRANSFORM :: EULER_ROTATION :: X [ " << j++ << " ]";
						lace NL S() << ptr_to_str(node);
						lace NL S() << "Pos " << node_map[node]->scenePos();
						lace NL "└Node";
						break;
					}
					}
					break;
				}
				}
				break;
			}
			case NODE::Type::MATH: {
				switch (static_cast<NODE::MATH::Type>(node->sub_type)) {
				case NODE::MATH::Type::ADD: {
					lace NL "┌Node :: MATH :: ADD [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::MATH::Type::SUB: {
					lace NL "┌Node :: MATH :: SUB [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::MATH::Type::MUL: {
					lace NL "┌Node :: MATH :: MUL [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::MATH::Type::DIV: {
					lace NL "┌Node :: MATH :: DIV [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				}
				break;
			}
			case NODE::Type::UTIL: {
				switch (static_cast<NODE::UTIL::Type>(node->sub_type)) {
				case NODE::UTIL::Type::PRINT: {
					lace NL "┌Node :: UTIL :: PRINT [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				case NODE::UTIL::Type::VIEW: {
					lace NL "┌Node :: UTIL :: VIEW [ " << j++ << " ]";
					lace NL S() << ptr_to_str(node);
					lace NL S() << "Pos " << node_map[node]->scenePos();
					lace NL "└Node";
					break;
				}
				}
				break;
			}
			}
		}
	lace R
		lace NL "└Nodes";
	lace NL "┌Build-E"; // [Node L Pointer] [Slot] [Slot] [Node R Pointer]
	lace A
		for (const auto node_l : data->nodes) {
			for (const auto& port : node_l->outputs) {
				if (port->type == NODE::PORT::Type::EXEC_O) {
					const auto port_l = static_cast<CLASS::NODE::PORT::Exec_O_Port*>(port);
					if (port_l->connection) {
						const auto port_r = port_l->connection;
						const auto node_r = port_r->node;
						const uint64 port_l_id = distance(node_l->outputs.begin(), find(node_l->outputs.begin(), node_l->outputs.end(), port_l));
						const uint64 port_r_id = distance(node_r->inputs.begin(), find(node_r->inputs.begin(), node_r->inputs.end(), port_r));
						//const uint64 node_r_id = distance(data->nodes.begin(), find(data->nodes.begin(), data->nodes.end(), node_r));
						//lace NL node_l_id SP port_l_id SP node_r_id SP port_r_id;
						lace NL ptr_to_str(node_l) SP port_l_id SP port_r_id SP ptr_to_str(node_r);
					}
				}
			}
		}
	lace R
		lace NL "└Build-E";
	lace NL "┌Build-D"; // [Node L Pointer] [Slot] [Slot] [Node R Pointer]
	lace A
		for (const auto node_r : data->nodes) {
			for (const auto& port : node_r->inputs) {
				if (port->type == NODE::PORT::Type::DATA_I) {
					const auto port_r = static_cast<CLASS::NODE::PORT::Data_I_Port*>(port);
					if (port_r->connection) {
						const auto port_l = port_r->connection;
						const auto node_l = port_l->node;
						const uint64 port_l_id = distance(node_l->outputs.begin(), find(node_l->outputs.begin(), node_l->outputs.end(), port_l));
						const uint64 port_r_id = distance(node_r->inputs.begin(), find(node_r->inputs.begin(), node_r->inputs.end(), port_r));
						//const uint64 node_l_id = distance(data->nodes.begin(), find(data->nodes.begin(), data->nodes.end(), node_l));
						//lace NL node_l_id SP port_l_id SP node_r_id SP port_r_id;
						lace NL ptr_to_str(node_l) SP port_l_id SP port_r_id SP ptr_to_str(node_r);
					}
				}
			}
		}
	lace R
		lace NL "└Build-D";
	lace R
		lace NL "└Node-Tree";
}

void CLASS::File::f_saveMaterial(Lace& lace, const SHADER::Material* data, const uint64& i) {
	lace NL "┌Material [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Material";
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
	lace NL "┌Data :: Atmosphere [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Atmosphere";
}

void CLASS::File::f_savePrimitive(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Primitve [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Primitive";
}

void CLASS::File::f_saveSkeleton(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Skeleton [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Skeleton";
}

void CLASS::File::f_saveCamera(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	const CLASS::OBJECT::DATA::Camera* camera = static_cast<CLASS::OBJECT::DATA::Camera*>(data->data);
	lace NL "┌Data :: Camera [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace NL "Fov " << camera->focal_angle;
	lace R
	lace NL "└Data :: Camera";
}

void CLASS::File::f_saveVolume(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Volume [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Volume";
}

void CLASS::File::f_saveCurve(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Curve [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Curve";
}

void CLASS::File::f_saveEmpty(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Empty [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Empty";
}

void CLASS::File::f_saveForce(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Force [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Force";
}

void CLASS::File::f_saveGroup(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Group [ " << i << " ] " << data->name;
	lace NL ptr_to_str(data);
	lace NL "└Data :: Group";
}

void CLASS::File::f_saveLight(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Light [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Light";
}

void CLASS::File::f_saveMesh(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	const CLASS::OBJECT::DATA::Mesh* mesh = static_cast<CLASS::OBJECT::DATA::Mesh*>(data->data);
	lace NL "┌Data :: Mesh [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace NL "┌Vertices( " << mesh->vertices.size() << " )";
	lace A
	for (uint64 i = 0; i < mesh->vertices.size(); i++) {
		lace NL i << " " << mesh->vertices[i]->position;
	}
	lace R
	lace NL "└Vertices";
	lace NL "┌Vertex-Groups";
	lace NL "└Vertex-Groups";
	lace NL "┌Faces( " << mesh->faces.size() << " )";
	lace A
	for (uint64 i = 0; i < mesh->faces.size(); i++) {
		lace NL i SP mesh->vertices.size();
		for (uint64 j = 0; j < mesh->faces[i]->vertices.size(); j++)
			for (uint64 k = 0; k < mesh->vertices.size(); k++)
				if (mesh->faces[i]->vertices[j] == mesh->vertices[k])
					lace SP k;
	}
	lace R
	lace NL "└Faces";
	lace NL "┌Normals";
	lace NL "└Normals";
	lace NL "┌UVs";
	lace NL "└UVs";
	lace R
	lace NL "└Data :: Mesh";
}

void CLASS::File::f_saveSfx(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Sfx [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Sfx";
}

void CLASS::File::f_saveVfx(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data :: Vfx [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace R
	lace NL "└Data :: Vfx";
}

void CLASS::File::f_saveObject(Lace& lace, const CLASS::Object* data, const uint64& i) {
	lace NL "┌Object [ " << i << " ] " << data->name;
	lace A
	lace NL ptr_to_str(data);
	lace NL "Position " << data->transform.position;
	lace NL "Rotation " << data->transform.euler_rotation;
	lace NL "Scale    " << data->transform.scale;
	switch (data->data->type) {
		case OBJECT::DATA::Type::ATMOSPHERE: lace NL "Type " << "Atmosphere"; break;
		case OBJECT::DATA::Type::PRIMITIVE : lace NL "Type " << "Primitive";  break;
		case OBJECT::DATA::Type::SKELETON  : lace NL "Type " << "Skeleton";   break;
		case OBJECT::DATA::Type::CAMERA    : lace NL "Type " << "Camera";     break;
		case OBJECT::DATA::Type::VOLUME    : lace NL "Type " << "Volume";     break;
		case OBJECT::DATA::Type::CURVE     : lace NL "Type " << "Curve";      break;
		case OBJECT::DATA::Type::EMPTY     : lace NL "Type " << "Empty";      break;
		case OBJECT::DATA::Type::FORCE     : lace NL "Type " << "Froce";      break;
		case OBJECT::DATA::Type::GROUP     : lace NL "Type " << "Group";      break;
		case OBJECT::DATA::Type::LIGHT     : lace NL "Type " << "Light";      break;
		case OBJECT::DATA::Type::MESH      : lace NL "Type " << "Mesh";       break;
		case OBJECT::DATA::Type::SFX       : lace NL "Type " << "Sfx";        break;
		case OBJECT::DATA::Type::VFX       : lace NL "Type " << "Vfx";        break;
		case OBJECT::DATA::Type::NONE      : lace NL "Type " << "NONE";       break;
	}
	lace R
	lace NL "└Object";
}

void CLASS::File::f_saveScene(Lace& lace, const CLASS::Scene* data, const uint64& i) {
	lace NL "┌Scene [ " << i << " ]";
	lace A
	lace NL ptr_to_str(data);
	lace NL "┌Objects";
	lace A
	for (auto object : data->objects)
		lace NL ptr_to_str(object);
	lace R
	lace NL "└Objects";
	lace R
	lace NL "└Scene";
}

void CLASS::File::f_saveBuild(Lace& lace) {
	lace NL "┌Build-Steps";
	lace A
	lace NL "Active-Scene " << ptr_to_str(active_scene->ptr);
	lace NL "┌Data-Group";
	lace A
	for (auto object : objects) {
		if (object->data->type == OBJECT::DATA::Type::GROUP) {
			for (auto child : object->data->getGroup()->objects) {
				lace NL ptr_to_str(object->data) SP ptr_to_str(child);
			}
		}
	}
	lace R
	lace NL "└Data-Group";
	lace NL "┌Object-Data";
	lace A
	for (auto object : objects) {
		if (object->data and object->data->type != OBJECT::DATA::Type::NONE) {
			lace NL ptr_to_str(object) SP ptr_to_str(object->data);
		}
	}
	lace R
	lace NL "└Object-Data";
	lace NL "┌Object-Node";
	lace A
	for (auto object: objects) {
		if (object->data->type != OBJECT::DATA::Type::NONE and object->node_tree) {
			lace NL ptr_to_str(object) SP ptr_to_str(object->node_tree);
		}
	}
	lace R
	lace NL "└Object-Node";
	lace NL "┌Node-Pointer";
	lace A
	for (auto object : objects) {
		if (object->data->type != OBJECT::DATA::Type::NONE and object->node_tree) {
			for (auto node : object->node_tree->nodes) {
				if (node->type == CLASS::NODE::Type::LINK and node->sub_type == e_to_u(CLASS::NODE::LINK::Type::POINTER)) {
					lace NL ptr_to_str(node) SP ptr_to_str(static_cast<CLASS::NODE::LINK::Pointer*>(node)->pointer);
				}
			}
		}
	}
	lace R
	lace NL "└Node-Pointer";
	lace R
	lace NL "└Build-Steps";
}