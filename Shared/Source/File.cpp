#include "File.hpp"

#include "Session.hpp"

#define NL << Lace_NL() <<
#define SP << Lace_S() <<

CLASS::File::File() {
	pointer_map = {};
	object_data = {};
	node_trees = {};
	materials = {};
	objects = {};
	scenes = {};

	active_object = new Observable_Ptr<Object>();
	active_scene = new Observable_Ptr<Scene>();

	default_camera = new Object();
	default_camera->transform.position = dvec3(2.5, 1.5, 2.5);
	default_camera->transform.euler_rotation = dvec3(-20, 45, 0);
	default_camera->data = new OBJECT::Data(OBJECT::DATA::Type::CAMERA, new OBJECT::DATA::Camera());

	version = "ERROR";
}

void CLASS::File::f_loadAsciiFile(const string& file_path) {
	LOG << ENDL << ANSI_B << "[ASCII File]" << ANSI_RESET  << " Filepath " << file_path; FLUSH;
	ifstream file(file_path, ios::binary);
	map<uint64, void*> pointer_map;

	if (file.is_open()) {
		Token_Array token_data = Token_Array();
		string line;
		while (getline(file, line)) {
			Tokens tokens = f_split(line);
			if (!tokens.empty()) {
				token_data.push_back(tokens);
			}
		}
		f_loadAscii(token_data);
	}
	else {
		LOG << ENDL << ANSI_R << "  [File]" << ANSI_RESET << " Error Opening File"; FLUSH;
	}
	LOG << ENDL << ANSI_G << "  [File]" << ANSI_RESET << " Loaded"; FLUSH;
	f_fileStats();
}

void CLASS::File::f_saveAsciiFile(const string& file_path) {
	ofstream file(file_path);
	if (file.is_open()) {
		file << f_printFile();
		file.close();
	}
}

void CLASS::File::f_loadBinaryFile(const string& file_path) {
	LOG << ENDL << ANSI_B << "[Binary File]" << ANSI_RESET  << " Filepath " << file_path; FLUSH;
	ifstream file(file_path, ios::binary | ios::ate);

	std::ifstream::pos_type fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	vector<Byte> buffer = vector<Byte>(fileSize);

	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
	file.close();

	f_loadBinary(buffer);
}

void CLASS::File::f_saveBinaryFile(const string& file_path) {
	ofstream file(file_path, ios::binary);
	if (file.is_open()) {
		Bin_Lace bin = Bin_Lace();

		f_saveBinary(bin);

		file.write(reinterpret_cast<const char*>(bin.data.data()), bin.data.size());
		file.close();
	}
}

CLASS::File CLASS::File::f_getAsciiFile(const string& file_path) {
	File file;
	file.f_loadAsciiFile(file_path);
	return file;
}

CLASS::File CLASS::File::f_getBinaryFile(const string& file_path) {
	File file;
	file.f_loadBinaryFile(file_path);
	return file;
}

string CLASS::File::f_printFile() {
	Lace lace = Lace();
	f_saveAscii(lace);
	return lace.str();
}

void CLASS::File::f_fileStats() const {
	LOG << ENDL << ANSI_B << "[Stats]" << ANSI_RESET;
	LOG << ENDL << "  Objects     ( " << objects.size() << " )";
	LOG << ENDL << "  Object Data ( " << object_data.size() << " )";
	LOG << ENDL << "  Pointer_Map ( " << pointer_map.size() << " )";
	FLUSH;
}

void CLASS::File::f_loadAscii(const Token_Array& token_data) {
	Token_Array data = Token_Array();

	Parse_Type is_processing = Parse_Type::NONE;
	for (const auto& tokens : token_data) {
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
				LOG << ENDL << ANSI_B << "  [Data-Block]" << ANSI_RESET; FLUSH;
				is_processing = Parse_Type::NONE;
				f_loadAsciiBuild(data);
			}
			else if (is_processing == Parse_Type::NODE_TREE and tokens[0] == "└Node-Tree") {
				LOG << ENDL << ANSI_B << "  [Data-Block]" << ANSI_RESET; FLUSH;
				is_processing = Parse_Type::NONE;
				node_trees.push_back(f_loadAsciiNodeTree(data));
			}
			else if (is_processing == Parse_Type::MATERIAL and tokens[0] == "└Material") {
				LOG << ENDL << ANSI_B << "  [Data-Block]" << ANSI_RESET; FLUSH;
				is_processing = Parse_Type::NONE;
				materials.push_back(f_loadAsciiMaterial(data));
			}
			else if (is_processing == Parse_Type::HEADER and tokens[0] == "└Header") {
				LOG << ENDL << ANSI_B << "  [Data-Block]" << ANSI_RESET; FLUSH;
				is_processing = Parse_Type::NONE;
				f_loadAsciiHeader(data);
			}
			else if (is_processing == Parse_Type::OBJECT and tokens[0] == "└Object") {
				LOG << ENDL << ANSI_B << "  [Data-Block]" << ANSI_RESET; FLUSH;
				is_processing = Parse_Type::NONE;
				objects.push_back(f_loadAsciiObject(data));
			}
			else if (is_processing == Parse_Type::SCENE and tokens[0] == "└Scene") {
				LOG << ENDL << ANSI_B << "  [Data-Block]" << ANSI_RESET; FLUSH;
				is_processing = Parse_Type::NONE;
				scenes.push_back(f_loadAsciiScene(data));
			}
			else if (is_processing == Parse_Type::DATA and tokens[0] == "└Data") {
				LOG << ENDL << ANSI_B << "  [Data-Block]" << ANSI_RESET; FLUSH;
				is_processing = Parse_Type::NONE;
				object_data.push_back(f_loadAsciiData(data));
			}
			else {
				data.push_back(tokens);
			}
		}
	}
}

void CLASS::File::f_loadAsciiHeader(const Token_Array& token_data) {
	LOG << ENDL << ANSI_B << "    [Header]" << ANSI_RESET; FLUSH;
	for (const Tokens& tokens : token_data) {
		if (tokens[0] == "Version") {
			version = tokens[1];
		}
		else if (tokens[0] == "Type") {
			string file_type = tokens[1];
		}
	}
}

CLASS::Node_Tree* CLASS::File::f_loadAsciiNodeTree(const Token_Array& token_data) {
	auto node_tree = new CLASS::Node_Tree();
	node_tree->name = f_join(token_data[0], 4);

	LOG << ENDL << ANSI_B << "    [Node-Tree]" << ANSI_RESET; FLUSH;
	LOG << ENDL << "      " << ANSI_Purple  << node_tree->name << ANSI_RESET; FLUSH;

	bool is_processing = false;
	Token_Array read_data = Token_Array();
	for (const Tokens& tokens : token_data) {
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
			const string name = f_join(read_data[0], 4);
			const uint64 pointer = str_to_ul(read_data[1][1]);
			const ivec2  pos = str_to_i(read_data[2][1], read_data[2][2]);

			CLASS::Node* node = nullptr;
			if      (read_data[3][1] == "CONSTRAINT") {
			}
			else if (read_data[3][1] == "GENERATE") {
			}
			else if (read_data[3][1] == "PHYSICS") {
			}
			else if (read_data[3][1] == "MODIFY") {
			}
			else if (read_data[3][1] == "EXEC") {
				if (read_data[3][3] == "SCRIPT") {
					LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
					LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
					node = new NODE::EXEC::Script(f_join(read_data[5]));
				}
				else if (read_data[3][3] == "TICK") {
					LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
					LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
					node = new NODE::EXEC::Tick();
					node_tree->tick = static_cast<NODE::EXEC::Tick*>(node);
				}
			}
			else if (read_data[3][1] == "LINK") {
				if      (read_data[3][3] == "POINTER") {
					LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
					LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
					auto node_t = new NODE::LINK::Pointer();
					node_t->pointer_type = DATA::fromString(read_data[5][1]);
					node = node_t;
				}
				else if (read_data[3][3] == "GET") {
					if      (read_data[3][5] == "FIELD") {
						LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
						LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
						auto node_t = new NODE::LINK::GET::Field();
						node_t->field = f_join(read_data[5]);
						node = node_t;
					}
				}
				else if (read_data[3][3] == "SET") {
					if      (read_data[3][5] == "TRANSFORM") {
						if      (read_data[3][7] == "EULER_ROTATION") {
							if      (read_data[3][9] == "X") {
								LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
								LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
								node = new NODE::LINK::SET::Euler_Rotation_X();
							}
						}
					}
				}
			}
			else if (read_data[3][1] == "MATH") {
				if      (read_data[3][3] == "ADD") {
					LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
					LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
					node = new NODE::MATH::Add();
				}
				else if (read_data[3][3] == "SUB") {
					LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
					LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
					node = new NODE::MATH::Sub();
				}
				else if (read_data[3][3] == "MUL") {
					LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
					LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
					node = new NODE::MATH::Mul();
				}
				else if (read_data[3][3] == "DIV") {
					LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
					LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
					node = new NODE::MATH::Div();
				}
			}
			else if (read_data[3][1] == "UTIL") {
				if (read_data[3][3] == "PRINT") {
					LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
					LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
					node = new NODE::UTIL::Print();
				}
				if (read_data[3][3] == "CAST") {
					if (read_data[3][5] == "UINT_TO_DOUBLE") {
						LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
						LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
						node = new NODE::UTIL::CAST::Uint_To_Double();
					}
					else if (read_data[3][5] == "INT_TO_DOUBLE") {
						LOG << ENDL << ANSI_B << "      [Node]" << ANSI_RESET; FLUSH;
						LOG << ENDL << "        " << ANSI_Purple << name << ANSI_RESET; FLUSH;
						node = new NODE::UTIL::CAST::Int_To_Double();
					}
				}
			}
			else {
				// TODO load "unkown" node from newer version
			}
			if (node) {
				node->name = name;

				pointer_map[pointer] = node;
				node_tree->nodes.push_back(node);
			}
		}
		else if (tokens[0] == "└Build-E") {
			LOG << ENDL << ANSI_B << "      [Build-E]" << ANSI_RESET; FLUSH;
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				auto node_l = static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])]);
				auto port_l = static_cast<NODE::PORT::Exec_O_Port*>(
					node_l->outputs[str_to_ul(sub_tokens[2])]
				);
				auto node_r = static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])]);
				auto port_r = static_cast<NODE::PORT::Exec_I_Port*>(
					node_r->inputs[str_to_ul(sub_tokens[3])]
				);
				port_l->connection = port_r;
				port_r->incoming_connections.push_back(port_l);
			}
		}
		else if (tokens[0] == "└Build-D") {
			LOG << ENDL << ANSI_B << "      [Build-D]" << ANSI_RESET; FLUSH;
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				auto node_l = static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[1])]);
				auto port_l = static_cast<NODE::PORT::Data_O_Port*>(
					node_l->outputs[str_to_ul(sub_tokens[2])]
				);
				auto node_r = static_cast<CLASS::Node*>(pointer_map[str_to_ul(sub_tokens[5])]);
				auto port_r = static_cast<NODE::PORT::Data_I_Port*>(
					node_r->inputs[str_to_ul(sub_tokens[3])]
				);
				port_r->connection = port_l;
				port_l->outgoing_connections.push_back(port_r);
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
	pointer_map[str_to_ul(token_data[1][1])] = node_tree;
	return node_tree;
}

SHADER::Shader* CLASS::File::f_loadAsciiMaterial(const Token_Array& token_data) {
	SHADER::Shader* material = new SHADER::Shader();
	material->name = f_join(token_data[0], 4);
	return material;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiData(const Token_Array& token_data) {
	if      (token_data[2][1] == "ATMOSPHERE")
		return f_loadAsciiAtmosphere(token_data);
	else if (token_data[2][1] == "PRIMITIVE")
		return f_loadAsciiPrimitive (token_data);
	else if (token_data[2][1] == "SKELETON")
		return f_loadAsciiSkeleton  (token_data);
	else if (token_data[2][1] == "CAMERA")
		return f_loadAsciiCamera    (token_data);
	else if (token_data[2][1] == "VOLUME")
		return f_loadAsciiVolume    (token_data);
	else if (token_data[2][1] == "CURVE")
		return f_loadAsciiCurve     (token_data);
	else if (token_data[2][1] == "EMPTY")
		return f_loadAsciiEmpty     (token_data);
	else if (token_data[2][1] == "FORCE")
		return f_loadAsciiForce     (token_data);
	else if (token_data[2][1] == "GROUP")
		return f_loadAsciiGroup     (token_data);
	else if (token_data[2][1] == "LIGHT")
		return f_loadAsciiLight     (token_data);
	else if (token_data[2][1] == "MESH")
		return f_loadAsciiMesh      (token_data);
	else if (token_data[2][1] == "SFX")
		return f_loadAsciiSfx       (token_data);
	else if (token_data[2][1] == "VFX")
		return f_loadAsciiVfx       (token_data);
	return new CLASS::OBJECT::Data();
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiAtmosphere(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::ATMOSPHERE;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiPrimitive(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::PRIMITIVE;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiSkeleton(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::SKELETON;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiCamera(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	CLASS::OBJECT::DATA::Camera* camera = new CLASS::OBJECT::DATA::Camera();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::CAMERA;
	data->data = camera;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiVolume(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::VOLUME;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiCurve(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::CURVE;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiEmpty(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::EMPTY;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiForce(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::FORCE;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiGroup(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	CLASS::OBJECT::DATA::Group* group = new CLASS::OBJECT::DATA::Group();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::GROUP;
	data->data = group;

	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiLight(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::LIGHT;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiMesh(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	CLASS::OBJECT::DATA::Mesh * mesh = new CLASS::OBJECT::DATA::Mesh();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::MESH;
	data->data = mesh;

	LOG << ENDL << ANSI_B << "    [Data] " << ANSI_RESET; FLUSH;
	LOG << ENDL << "      " << ANSI_Purple  << data->name << ANSI_RESET; FLUSH;
	LOG << ENDL << ANSI_B << "      [Mesh]" << ANSI_RESET; FLUSH;

	bool is_processing = false;
	Token_Array read_data = Token_Array();
	for (const Tokens& tokens : token_data) {
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
			LOG << ENDL << "        Vertices"; FLUSH;
			for (const Tokens& token_data : read_data) {
				mesh->vertices.push_back(new CLASS::OBJECT::DATA::MESH::Vertex(str_to_d(token_data[2], token_data[3], token_data[4])));
			}
		}
		else if (tokens[0] == "└Vertex-Group") {
			is_processing = false;
			LOG << ENDL << "        Vertex-Group"; FLUSH;
			for (const string& index : read_data[1]) {
				mesh->vertex_groups[f_join(read_data[0], 4)].push_back(mesh->vertices[str_to_d(index)]);
			}
		}
		else if (tokens[0] == "└Faces") {
			is_processing = false;
			LOG << ENDL << "        Faces"; FLUSH;
			for (const Tokens& token_data : read_data) {
				CLASS::OBJECT::DATA::MESH::Face* face = new CLASS::OBJECT::DATA::MESH::Face();
				for (uint64 i = 0; i < str_to_u(token_data[1]); i++) {
					face->vertices.push_back(mesh->vertices[str_to_ul(token_data[i + 3])]);
				}
				mesh->faces.push_back(face);
			}
		}
		else if (tokens[0] == "└Normals") {
			is_processing = false;
			LOG << ENDL << "       Normals"; FLUSH;
			for (const Tokens& token_data : read_data) {
				for (uint64 i = 0; i < str_to_u(token_data[1]); i ++) {
					mesh->normals[mesh->faces[str_to_ul(token_data[0])]].push_back(str_to_d(token_data[3 + i * 5], token_data[4 + i * 5], token_data[5 + i * 5]));
				}
			}
		}
		else if (tokens[0] == "└UVs") {
			is_processing = false;
			LOG << ENDL << "        UVs"; FLUSH;
			for (const Tokens& token_data : read_data) {
				for (uint64 i = 0; i < str_to_u(token_data[1]); i ++) {
					mesh->uvs[mesh->faces[str_to_ul(token_data[0])]].push_back(str_to_d(token_data[3 + i * 4], token_data[4 + i * 4]));
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

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiSfx(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::SFX;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::OBJECT::Data* CLASS::File::f_loadAsciiVfx(const Token_Array& token_data) {
	CLASS::OBJECT::Data* data = new CLASS::OBJECT::Data();
	data->name = f_join(token_data[0], 4);
	data->type = CLASS::OBJECT::DATA::Type::VFX;
	pointer_map[str_to_ul(token_data[1][1])] = data;
	return data;
}

CLASS::Object* CLASS::File::f_loadAsciiObject(const Token_Array& token_data) {
	CLASS::Object* object = new CLASS::Object();
	object->name = f_join(token_data[0], 4);

	LOG << ENDL << ANSI_B << "    [Object] " << ANSI_RESET; FLUSH;
	LOG << ENDL << "      " << ANSI_Purple << object->name << ANSI_RESET; FLUSH;

	CLASS::Transform transform;

	bool is_processing = false;
	Token_Array read_data = Token_Array();
	for (const Tokens& tokens : token_data) {
		if (tokens[0] == "Position") {
			transform.position = str_to_d(tokens[2], tokens[3], tokens[4]);
		}
		else if (tokens[0] == "Rotation") {
			transform.euler_rotation = str_to_d(tokens[2], tokens[3], tokens[4]);
		}
		else if (tokens[0] == "Scale") {
			transform.scale = str_to_d(tokens[2], tokens[3], tokens[4]);
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
	object->transform = transform;
	pointer_map[str_to_ul(token_data[1][1])] = object;
	return object;
};

CLASS::Scene* CLASS::File::f_loadAsciiScene(const Token_Array& token_data) {
	LOG << ENDL << ANSI_B << "    [Scene]" << ANSI_RESET; FLUSH;

	CLASS::Scene* scene = new CLASS::Scene();

	bool is_processing = false;
	Token_Array read_data = Token_Array();
	for (const Tokens& tokens : token_data) {
		if (tokens[0] == "Active-Camera") {
			scene->active_camera = objects[str_to_ul(tokens[1])];
		}
		else if (tokens[0] == "┌Objects") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Objects") {
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
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

void CLASS::File::f_loadAsciiBuild(const Token_Array& token_data) {
	LOG << ENDL << ANSI_B << "    [Build]" << ANSI_RESET; FLUSH;

	bool is_processing = false;
	Token_Array read_data = Token_Array();
	for (const Tokens& tokens : token_data) {
		if (tokens[0] == "Active-Scene") {
			active_scene->set(static_cast<CLASS::Scene*>(pointer_map[str_to_ul(tokens[2])]));
		}
		else if (tokens[0] == "┌Data-Group") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Data-Group") {
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				static_cast<CLASS::OBJECT::Data*>(pointer_map[str_to_ul(sub_tokens[1])])->getGroup()->objects.push_back(static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[3])]));
			}
		}
		else if (tokens[0] == "┌Object-Data") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Object-Data") {
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[1])])->data = static_cast<CLASS::OBJECT::Data*>(pointer_map[str_to_ul(sub_tokens[3])]);
			}
		}
		else if (tokens[0] == "┌Object-Node") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Object-Node") {
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[1])])->node_tree = static_cast<CLASS::Node_Tree*>(pointer_map[str_to_ul(sub_tokens[3])]);
			}
		}
		else if (tokens[0] == "┌Node-Pointer") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Node-Pointer") {
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				auto ptr = static_cast<NODE::LINK::Pointer*>(pointer_map[str_to_ul(sub_tokens[1])]);
				ptr->pointer = static_cast<CLASS::Object*>(pointer_map[str_to_ul(sub_tokens[3])]);
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
}

void CLASS::File::f_loadBinary(const vector<Byte>& byte_data) {
	map<uint64, void*> pointer_map;

	version = f_readBinary<string>(byte_data, 0, 5);
	type    = f_readBinary<string>(byte_data, 5, 4);

	//const uint64 start_materials   = f_readBinary<uint64>(byte_data, 11);
	//const uint64 start_node_trees  = f_readBinary<uint64>(byte_data, 19);
	//const uint64 start_object_data = f_readBinary<uint64>(byte_data, 27);
	//const uint64 start_objects     = f_readBinary<uint64>(byte_data, 35);
	//const uint64 start_scenes      = f_readBinary<uint64>(byte_data, 43);
	//
	//const uint64 end_materials   = f_readBinary<uint64>(byte_data, 51);
	//const uint64 end_node_trees  = f_readBinary<uint64>(byte_data, 59);
	//const uint64 end_object_data = f_readBinary<uint64>(byte_data, 67);
	//const uint64 end_objects     = f_readBinary<uint64>(byte_data, 75);
	//const uint64 end_scenes      = f_readBinary<uint64>(byte_data, 83);
}

void CLASS::File::f_saveAscii(Lace & lace) {
	f_saveAsciiHeader(lace);
	uint64 count = 0;

	count = 0;
	lace NL "┌Node-Trees( " << node_trees.size() << " )";
	lace++;
	for (CLASS::Node_Tree* node : node_trees)
		f_saveAsciiNodeTree(lace, node, count++);
	lace--;
	lace NL "└Node-Trees";

	count = 0;
	lace NL "┌Materials( " << materials.size() << " )";
	lace++;
	for (const SHADER::Shader* material : materials)
		f_saveAsciiMaterial(lace, material, count++);
	lace--;
	lace NL "└Materials";

	lace NL "┌Data( " << object_data.size() << " )";
	lace++;
	f_saveAsciiData(lace);
	lace--;
	lace NL "└Data";

	count = 0;
	lace NL "┌Objects( " << objects.size() << " )";
	lace++;
	for (const CLASS::Object* object : objects)
		f_saveAsciiObject(lace, object, count++);
	lace--;
	lace NL "└Objects";

	count = 0;
	lace NL "┌Scenes( " << scenes.size() << " )";
	lace++;
	for (const CLASS::Scene* scene : scenes)
		f_saveAsciiScene(lace, scene, count++);
	lace--;
	lace NL "└Scenes";

	f_saveAsciiBuild(lace);
}

void CLASS::File::f_saveAsciiHeader(Lace& lace) {
	lace NL "┌Header";
	lace++;
	lace NL "Version 1.0.0";
	lace NL "Type FILE";
	lace--;
	lace NL "└Header";
}

void CLASS::File::f_saveAsciiNodeTree(Lace& lace, CLASS::Node_Tree* data, const uint64& i) {
	uint64 j = 0;
	lace NL "┌Node-Tree [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "┌Nodes( " << data->nodes.size() << " )";
	lace++;
	for (CLASS::Node* node : data->nodes) {
		lace NL "┌Node [ " << j++ << " ] " << node->name;
		lace++;
		lace NL ptr_to_str(node);
		switch (node->type) {
			case NODE::Type::CONSTRAINT: {
				switch (static_cast<NODE::CONSTRAINT::Type>(node->sub_type)) {
					case NODE::CONSTRAINT::Type::PARENT: {
						lace NL"Type CONSTRAINT :: PARENT";
						break;
					}
				}
				break;
			}
			case NODE::Type::EXEC: {
				switch (static_cast<NODE::EXEC::Type>(node->sub_type)) {
					case NODE::EXEC::Type::COUNTER: {
						lace NL "Type EXEC :: COUNTER";
						break;
					}
					case NODE::EXEC::Type::SCRIPT: {
						lace NL "Type EXEC :: SCRIPT";
						lace NL "┌Script";
						lace NL Lace_S() << static_cast<CLASS::NODE::EXEC::Script*>(node)->script_id;
						lace NL "└Script";
						break;
					}
					case NODE::EXEC::Type::TICK: {
						lace NL "Type EXEC :: TICK";
						break;
					}
				}
				break;
			}
			case NODE::Type::LINK: {
				switch (static_cast<NODE::LINK::Type>(node->sub_type)) {
					case NODE::LINK::Type::POINTER: {
						lace NL "Type LINK :: POINTER";
						lace NL "┌Pointer";
						lace NL Lace_S() << "Type " << toString(static_cast<NODE::LINK::Pointer*>(node)->pointer_type);
						lace NL "└Pointer";
						break;
					}
					case NODE::LINK::Type::GET: {
						switch (static_cast<CLASS::NODE::LINK::Get*>(node)->mini_type) {
						case NODE::LINK::GET::Type::FIELD: {
							lace NL "Type LINK :: GET :: FIELD";
							lace NL "┌Field";
							lace NL Lace_S() << static_cast<NODE::LINK::GET::Field*>(node)->field;
							lace NL "└Field";
							break;
						}
						}
						break;
					}
					case NODE::LINK::Type::SET: {
						switch (static_cast<CLASS::NODE::LINK::Set*>(node)->mini_type) {
							case NODE::LINK::SET::Type::EULER_ROTATION_X: {
								lace NL "Type LINK :: SET :: TRANSFORM :: EULER_ROTATION :: X";
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
						lace NL "Type MATH :: ADD";
						break;
					}
					case NODE::MATH::Type::SUB: {
						lace NL "Type MATH :: SUB";
						break;
					}
					case NODE::MATH::Type::MUL: {
						lace NL "Type MATH :: MUL";
						break;
					}
					case NODE::MATH::Type::DIV: {
						lace NL "Type MATH :: DIV";
						break;
					}
				}
				break;
			}
			case NODE::Type::UTIL: {
				switch (static_cast<NODE::UTIL::Type>(node->sub_type)) {
					case NODE::UTIL::Type::PRINT: {
						break;
					}
					case NODE::UTIL::Type::CAST: {
						switch (static_cast<NODE::UTIL::Cast*>(node)->mini_type) {
							case NODE::UTIL::CAST::Type::UINT_TO_DOUBLE: {
								lace NL "Type UTIL :: CAST :: UINT_TO_DOUBLE";
								break;
							}
							case NODE::UTIL::CAST::Type::INT_TO_DOUBLE: {
								lace NL "Type UTIL :: CAST :: INT_TO_DOUBLE";
								break;
							}
							break;
						}
						break;
					}
					case NODE::UTIL::Type::VIEW: {
						lace NL "Type UTIL :: VIEW";
						break;
					}
				}
				break;
			}
		}
		lace--;
		lace NL "└Node";
	}
	lace--;
		lace NL "└Nodes";
	lace NL "┌Build-E"; // [Node L Pointer] [Slot] [Slot] [Node R Pointer]
	lace++;
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
	lace--;
		lace NL "└Build-E";
	lace NL "┌Build-D"; // [Node L Pointer] [Slot] [Slot] [Node R Pointer]
	lace++;
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
	lace--;
		lace NL "└Build-D";
	lace--;
		lace NL "└Node-Tree";
}

void CLASS::File::f_saveAsciiMaterial(Lace& lace, const SHADER::Shader* data, const uint64& i) {
	lace NL "┌Material [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace--;
	lace NL "└Material";
}

void CLASS::File::f_saveAsciiData(Lace& lace) {
	for (uint64 i = 0; i < object_data.size(); i++) {
		switch (object_data[i]->type) {
			case CLASS::OBJECT::DATA::Type::ATMOSPHERE: f_saveAsciiAtmosphere(lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::PRIMITIVE:  f_saveAsciiPrimitive (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::SKELETON:   f_saveAsciiSkeleton  (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::CAMERA:     f_saveAsciiCamera    (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::VOLUME:     f_saveAsciiVolume    (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::CURVE:      f_saveAsciiCurve     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::EMPTY:      f_saveAsciiEmpty     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::FORCE:      f_saveAsciiForce     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::GROUP:      f_saveAsciiGroup     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::LIGHT:      f_saveAsciiLight     (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::MESH:       f_saveAsciiMesh      (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::SFX:        f_saveAsciiSfx       (lace, object_data[i], i); break;
			case CLASS::OBJECT::DATA::Type::VFX:        f_saveAsciiVfx       (lace, object_data[i], i); break;
		}
	}
}

void CLASS::File::f_saveAsciiAtmosphere(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type ATMOSPHERE";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiPrimitive(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type PRIMITIVE";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiSkeleton(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type SKELETON";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiCamera(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type CAMERA";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiVolume(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type VOLUME";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiCurve(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type CURVE";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiEmpty(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type EMPTY";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiForce(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type FORCE";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiGroup(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type GROUP";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiLight(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type LIGHT";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiMesh(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	const CLASS::OBJECT::DATA::Mesh* mesh = static_cast<CLASS::OBJECT::DATA::Mesh*>(data->data);
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type MESH";
	lace NL "┌Mesh";
	lace++;
	lace NL "┌Vertices( " << mesh->vertices.size() << " )";
	lace++;
	for (uint64 i = 0; i < mesh->vertices.size(); i++) {
		lace NL i << " ( " << mesh->vertices[i]->position << " )";
	}
	lace--;
	lace NL "└Vertices";
	lace NL "┌Vertex-Groups( " << mesh->vertex_groups.size() << " )";
	lace NL "└Vertex-Groups";
	lace NL "┌Faces( " << mesh->faces.size() << " )";
	lace++;
	for (uint64 i = 0; i < mesh->faces.size(); i++) {
		lace NL i SP mesh->faces[i]->vertices.size() SP "[";
		for (uint64 j = 0; j < mesh->faces[i]->vertices.size(); j++)
			for (uint64 k = 0; k < mesh->vertices.size(); k++)
				if (mesh->faces[i]->vertices[j] == mesh->vertices[k])
					lace SP k;
		lace SP "]";
	}
	lace--;
	lace NL "└Faces";
	lace NL "┌Normals( 0 )";
	lace++;
	// TODO Fix crash if no normal layers are present
	//for (uint64 i = 0; i < mesh->faces.size(); i++) {
	//	lace NL i SP mesh->normals.at(mesh->faces[i]).size();
	//	for (uint64 j = 0; j < mesh->normals.at(mesh->faces[i]).size(); j++) {
	//		lace SP "(" SP mesh->normals.at(mesh->faces[i])[j] SP ")";
	//	}
	//}
	lace--;
	lace NL "└Normals";
	lace NL "┌UVs( 0 )";
	lace++;
	// TODO Fix crash if no uv layers are present
	//for (uint64 i = 0; i < mesh->faces.size(); i++) {
	//	lace NL i SP mesh->uvs.at(mesh->faces[i]).size();
	//	for (uint64 j = 0; j < mesh->uvs.at(mesh->faces[i]).size(); j++) {
	//		lace SP "(" SP mesh->uvs.at(mesh->faces[i])[j] SP ")";
	//	}
	//}
	lace--;
	lace NL "└UVs";
	lace--;
	lace NL "└Mesh";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiSfx(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type SFX";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiVfx(Lace& lace, const CLASS::OBJECT::Data* data, const uint64& i) {
	lace NL "┌Data [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Type VFX";
	lace--;
	lace NL "└Data";
}

void CLASS::File::f_saveAsciiObject(Lace& lace, const CLASS::Object* data, const uint64& i) {
	lace NL "┌Object [ " << i << " ] " << data->name;
	lace++;
	lace NL ptr_to_str(data);
	lace NL "Position ( " << data->transform.position << " )";
	lace NL "Rotation ( " << data->transform.euler_rotation << " )";
	lace NL "Scale    ( " << data->transform.scale << " )";
	switch (data->data->type) {
		case OBJECT::DATA::Type::NONE      : lace NL "Type " << "NONE";       break;
		case OBJECT::DATA::Type::ATMOSPHERE: lace NL "Type " << "ATMOSPHERE"; break;
		case OBJECT::DATA::Type::PRIMITIVE : lace NL "Type " << "PRIMITIVE";  break;
		case OBJECT::DATA::Type::SKELETON  : lace NL "Type " << "SKELETON";   break;
		case OBJECT::DATA::Type::CAMERA    : lace NL "Type " << "CAMERA";     break;
		case OBJECT::DATA::Type::VOLUME    : lace NL "Type " << "VOLUME";     break;
		case OBJECT::DATA::Type::CURVE     : lace NL "Type " << "CURVE";      break;
		case OBJECT::DATA::Type::EMPTY     : lace NL "Type " << "EMPTY";      break;
		case OBJECT::DATA::Type::FORCE     : lace NL "Type " << "FORCE";      break;
		case OBJECT::DATA::Type::GROUP     : lace NL "Type " << "GROUP";      break;
		case OBJECT::DATA::Type::LIGHT     : lace NL "Type " << "LIGHT";      break;
		case OBJECT::DATA::Type::MESH      : lace NL "Type " << "MESH";       break;
		case OBJECT::DATA::Type::SFX       : lace NL "Type " << "SFX";        break;
		case OBJECT::DATA::Type::VFX       : lace NL "Type " << "VFX";        break;
	}
	lace--;
	lace NL "└Object";
}

void CLASS::File::f_saveAsciiScene(Lace& lace, const CLASS::Scene* data, const uint64& i) {
	lace NL "┌Scene [ " << i << " ]";
	lace++;
	lace NL ptr_to_str(data);
	lace NL "┌Objects";
	lace++;
	for (auto object : data->objects)
		lace NL ptr_to_str(object);
	lace--;
	lace NL "└Objects";
	lace--;
	lace NL "└Scene";
}

void CLASS::File::f_saveAsciiBuild(Lace& lace) {
	lace NL "┌Build-Steps";
	lace++;
	lace NL "Active-Scene " << ptr_to_str(active_scene->ptr);
	lace NL "┌Data-Group";
	lace++;
	for (auto object : objects) {
		if (object->data->type == OBJECT::DATA::Type::GROUP) {
			for (auto child : object->data->getGroup()->objects) {
				lace NL ptr_to_str(object->data) SP ptr_to_str(child);
			}
		}
	}
	lace--;
	lace NL "└Data-Group";
	lace NL "┌Object-Data";
	lace++;
	for (auto object : objects) {
		if (object->data and object->data->type != OBJECT::DATA::Type::NONE) {
			lace NL ptr_to_str(object) SP ptr_to_str(object->data);
		}
	}
	lace--;
	lace NL "└Object-Data";
	lace NL "┌Object-Node";
	lace++;
	for (auto object: objects) {
		if (object->data->type != OBJECT::DATA::Type::NONE and object->node_tree) {
			lace NL ptr_to_str(object) SP ptr_to_str(object->node_tree);
		}
	}
	lace--;
	lace NL "└Object-Node";
	lace NL "┌Node-Pointer";
	lace++;
	for (auto object : objects) {
		if (object->data->type != OBJECT::DATA::Type::NONE and object->node_tree) {
			for (auto node : object->node_tree->nodes) {
				if (node->type == CLASS::NODE::Type::LINK and node->sub_type == e_to_u(CLASS::NODE::LINK::Type::POINTER)) {
					lace NL ptr_to_str(node) SP ptr_to_str(static_cast<CLASS::NODE::LINK::Pointer*>(node)->pointer);
				}
			}
		}
	}
	lace--;
	lace NL "└Node-Pointer";
	lace--;
	lace NL "└Build-Steps";
}

void CLASS::File::f_saveBinary(Bin_Lace & bin) {
	f_saveBinaryHeader(bin);

	bin << node_trees.size();
	for (CLASS::Node_Tree* node : node_trees) {
		f_saveBinaryNodeTree(bin, node);
	}

	bin << materials.size();

	bin << object_data.size();

	bin << objects.size();

	bin << scenes.size();
}

void CLASS::File::f_saveBinaryHeader(Bin_Lace & bin) {
	bin << string("1.0.0");
	bin << string("FILE");
}

void CLASS::File::f_saveBinaryNodeTree(Bin_Lace & bin, Node_Tree* data) {
	uint64 size = 0;
	Bin_Lace bytes;

	bytes << ul_to_u(data->name.size());
	bytes << data->name;
	bytes << ptr(data);
	bytes << ul_to_uh(data->nodes.size());

	size += 4;
	size += data->name.size();
	size += 8;
	size += 2;

	for (CLASS::Node* node : data->nodes) {
		bytes << ul_to_uh(node->name.size());
		bytes << node->name;
		bytes << ptr(node);

		size += 2;
		size += node->name.size();
		size += 8;
	}

	bin << size;
	bin << bytes;
}