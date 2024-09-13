#include "Core/Editor_File.hpp"

KL::Editor_File::Editor_File() :
	File()
{
	f_loadEditorTools();
}

void KL::Editor_File::f_loadEditorTools() {
	const auto base_pointer_map = pointer_map;
	pointer_map.clear();

	ifstream file("./Resources/Assets/Gizmo.krz", ios::binary);

	Token_Array token_data = Token_Array();
	Tokens line_data = Tokens();
	string line;
	while (getline(file, line)) {
		Tokens tokens = f_split(line);
		if (!tokens.empty()) {
			token_data.push_back(tokens);
			line_data.push_back(line);
		}
	}
	
	Token_Array t_data = Token_Array();
	Tokens l_data = Tokens();

	Parse_Type is_processing = Parse_Type::NONE;
	for (uint64 i = 0; i < token_data.size(); i++) {
		const Tokens tokens = token_data[i];
		const string line = line_data[i];
		if (is_processing == Parse_Type::NONE) {
			if      (tokens[0] == "┌Build-Steps")
				is_processing = Parse_Type::BUILD_STEPS;
			else if (tokens[0] == "┌Node-Tree")
				is_processing = Parse_Type::NODE_TREE;
			else if (tokens[0] == "┌Shader")
				is_processing = Parse_Type::SHADER;
			else if (tokens[0] == "┌Texture")
				is_processing = Parse_Type::TEXTURE;
			else if (tokens[0] == "┌Header")
				is_processing = Parse_Type::HEADER;
			else if (tokens[0] == "┌Object")
				is_processing = Parse_Type::OBJECT;
			else if (tokens[0] == "┌Scene")
				is_processing = Parse_Type::SCENE;
			else if (tokens[0] == "┌Data")
				is_processing = Parse_Type::DATA;
			t_data.clear();
			t_data.push_back(tokens);
			l_data.clear();
			l_data.push_back(line);
		}
		else {
			if      (is_processing == Parse_Type::BUILD_STEPS and tokens[0] == "└Build-Steps") {
				is_processing = Parse_Type::NONE;
				f_loadAsciiBuild(t_data, l_data);
			}
			else if (is_processing == Parse_Type::NODE_TREE and tokens[0] == "└Node-Tree") {
				is_processing = Parse_Type::NONE;
			}
			else if (is_processing == Parse_Type::SHADER and tokens[0] == "└Shader") {
				is_processing = Parse_Type::NONE;
			}
			else if (is_processing == Parse_Type::TEXTURE and tokens[0] == "└Texture") {
				is_processing = Parse_Type::NONE;
			}
			else if (is_processing == Parse_Type::HEADER and tokens[0] == "└Header") {
				is_processing = Parse_Type::NONE;
			}
			else if (is_processing == Parse_Type::OBJECT and tokens[0] == "└Object") {
				is_processing = Parse_Type::NONE;
				auto data = f_loadAsciiObject(t_data, l_data);
				editor_objects[data->name] = data;
			}
			else if (is_processing == Parse_Type::SCENE and tokens[0] == "└Scene") {
				is_processing = Parse_Type::NONE;
			}
			else if (is_processing == Parse_Type::DATA and tokens[0] == "└Data") {
				is_processing = Parse_Type::NONE;
				auto data = f_loadAsciiData(t_data, l_data);
				editor_object_data[data->name] = data;
			}
			else {
				t_data.push_back(tokens);
				l_data.push_back(line);
			}
		}
	}
	pointer_map = base_pointer_map;
}

KL::Node_Tree* KL::Editor_File::f_loadAsciiNodeTree(const Token_Array&token_data, const Tokens& line_data) {
	auto node_tree = new KL::Node_Tree();
	auto gui_node_tree = new GUI::NODE::Node_Tree();
	node_tree->name = f_join(token_data[0], 4);

	LOG ENDL ANSI_B << "    [Node-Tree]" ANSI_RESET; FLUSH;
	LOG ENDL << "      " ANSI_PURPLE  << node_tree->name ANSI_RESET; FLUSH;

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

			KL::Node* node = nullptr;
			GUI::NODE::Node* gui_node = nullptr;
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
					LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
					LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
					node = new NODE::EXEC::Script(f_join(read_data[5]));
					gui_node = new GUI::NODE::EXEC::Script(pos, f_join(read_data[5]));
				}
				else if (read_data[3][3] == "TICK") {
					LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
					LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
					node = new NODE::EXEC::Tick();
					gui_node = new GUI::NODE::EXEC::Tick(pos);
					node_tree->tick = static_cast<NODE::EXEC::Tick*>(node);
				}
			}
			else if (read_data[3][1] == "LINK") {
				if      (read_data[3][3] == "POINTER") {
					LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
					LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
					auto node_t = new NODE::LINK::Pointer();
					node_t->pointer_type = DATA::fromString(read_data[5][1]);
					gui_node = new GUI::NODE::LINK::Pointer(pos, DATA::fromString(read_data[5][1]));
					node = node_t;
				}
				else if (read_data[3][3] == "GET") {
					if      (read_data[3][5] == "FIELD") {
						LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
						LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
						auto node_t = new NODE::LINK::GET::Field();
						node_t->field = f_join(read_data[5]);
						auto gui_node_t = new GUI::NODE::LINK::GET::Field(pos);
						gui_node_t->field->setText(QString::fromStdString(f_join(read_data[5])));
						node = node_t;
						gui_node = gui_node_t;
					}
				}
				else if (read_data[3][3] == "SET") {
					if      (read_data[3][5] == "TRANSFORM") {
						if      (read_data[3][7] == "EULER_ROTATION") {
							if      (read_data[3][9] == "X") {
								LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
								LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
								node = new NODE::LINK::SET::Euler_Rotation_X();
								gui_node = new GUI::NODE::LINK::SET::Euler_Rotation_X(pos);
							}
						}
					}
				}
			}
			else if (read_data[3][1] == "MATH") {
				if      (read_data[3][3] == "ADD") {
					LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
					LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
					node = new NODE::MATH::Add();
					gui_node = new GUI::NODE::MATH::Add(pos);
				}
				else if (read_data[3][3] == "SUB") {
					LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
					LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
					node = new NODE::MATH::Sub();
					gui_node = new GUI::NODE::MATH::Sub(pos);
				}
				else if (read_data[3][3] == "MUL") {
					LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
					LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
					node = new NODE::MATH::Mul();
					gui_node = new GUI::NODE::MATH::Mul(pos);
				}
				else if (read_data[3][3] == "DIV") {
					LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
					LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
					node = new NODE::MATH::Div();
					gui_node = new GUI::NODE::MATH::Div(pos);
				}
			}
			else if (read_data[3][1] == "UTIL") {
				if (read_data[3][3] == "PRINT") {
					LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
					LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
					node = new NODE::UTIL::Print();
					gui_node = new GUI::NODE::UTIL::Print(pos);
				}
				if (read_data[3][3] == "CAST") {
					if (read_data[3][5] == "UINT_TO_DOUBLE") {
						LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
						LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
						node = new NODE::UTIL::CAST::Uint_To_Double();
						gui_node = new GUI::NODE::UTIL::CAST::Uint_To_Double(pos);
					}
					else if (read_data[3][5] == "INT_TO_DOUBLE") {
						LOG ENDL ANSI_B << "      [Node]" ANSI_RESET; FLUSH;
						LOG ENDL << "        " ANSI_PURPLE << name ANSI_RESET; FLUSH;
						node = new NODE::UTIL::CAST::Int_To_Double();
						gui_node = new GUI::NODE::UTIL::CAST::Int_To_Double(pos);
					}
				}
			}
			else {
				// TODO load "unkown" node from newer version
			}
			if (node) {
				node->name = name;

				pointer_map.insert(pointer, uptr(node));
				gui_node_tree->nodes.push_back(gui_node);
				node_tree->nodes.push_back(node);
				node_map[node] = gui_node;
			}
		}
		else if (tokens[0] == "└Build-E") {
			LOG ENDL ANSI_B << "      [Build-E]" ANSI_RESET; FLUSH;
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				auto node_l = ptr<KL::Node*>(pointer_map.getVal(str_to_ul(sub_tokens[1])));
				auto port_l = static_cast<NODE::PORT::Exec_O_Port*>(
					node_l->outputs[str_to_ul(sub_tokens[2])]
					);
				auto node_r = ptr<KL::Node*>(pointer_map.getVal(str_to_ul(sub_tokens[5])));
				auto port_r = static_cast<NODE::PORT::Exec_I_Port*>(
					node_r->inputs[str_to_ul(sub_tokens[3])]
					);
				port_l->connection = port_r;
				port_r->incoming_connections.push_back(port_l);

				auto gui_port_l = static_cast<GUI::NODE::PORT::Exec_O_Port*>(
					node_map[node_l]->outputs[str_to_ul(sub_tokens[2])]
					);
				auto gui_port_r = static_cast<GUI::NODE::PORT::Exec_I_Port*>(
					node_map[node_r]->inputs[str_to_ul(sub_tokens[3])]
					);
				gui_port_l->connection = new GUI::NODE::Connection(gui_port_l, gui_port_r);
				gui_port_r->incoming_connections.push_back(gui_port_l->connection);
			}
		}
		else if (tokens[0] == "└Build-D") {
			LOG ENDL ANSI_B << "      [Build-D]" ANSI_RESET; FLUSH;
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				auto node_l = ptr<KL::Node*>(pointer_map.getVal(str_to_ul(sub_tokens[1])));
				auto port_l = static_cast<NODE::PORT::Data_O_Port*>(
					node_l->outputs[str_to_ul(sub_tokens[2])]
					);
				auto node_r = ptr<KL::Node*>(pointer_map.getVal(str_to_ul(sub_tokens[5])));
				auto port_r = static_cast<NODE::PORT::Data_I_Port*>(
					node_r->inputs[str_to_ul(sub_tokens[3])]
					);
				port_r->connection = port_l;
				port_l->outgoing_connections.push_back(port_r);

				auto gui_port_l = static_cast<GUI::NODE::PORT::Data_O_Port*>(
					node_map[node_l]->outputs[str_to_ul(sub_tokens[2])]
					);
				auto gui_port_r = static_cast<GUI::NODE::PORT::Data_I_Port*>(
					node_map[node_r]->inputs[str_to_ul(sub_tokens[3])]
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
	pointer_map.insert(str_to_ul(token_data[1][1]), uptr(node_tree));
	return node_tree;
}

void KL::Editor_File::f_loadAsciiBuild(const Token_Array& token_data, const Tokens& line_data) {
	LOG ENDL ANSI_B << "    [Build]" ANSI_RESET; FLUSH;

	bool is_processing = false;
	Token_Array read_data = Token_Array();
	for (const Tokens& tokens : token_data) {
		if (tokens[0] == "Active-Scene") {
			active_scene->set(ptr<KL::Scene*>(pointer_map.getVal(str_to_ul(tokens[2]))));
		}
		else if (tokens[0] == "┌Data-Group") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Data-Group") {
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				ptr<KL::OBJECT::Data*>(pointer_map.getVal(str_to_ul(sub_tokens[1])))->getGroup()->objects.push_back(ptr<KL::Object*>(pointer_map.getVal(str_to_ul(sub_tokens[3]))));
			}
		}
		else if (tokens[0] == "┌Object-Data") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Object-Data") {
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				ptr<KL::Object*>(pointer_map.getVal(str_to_ul(sub_tokens[1])))->data = ptr<KL::OBJECT::Data*>(pointer_map.getVal(str_to_ul(sub_tokens[3])));
			}
		}
		else if (tokens[0] == "┌Object-Node") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Object-Node") {
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				ptr<KL::Object*>(pointer_map.getVal(str_to_ul(sub_tokens[1])))->node_tree = ptr<KL::Node_Tree*>(pointer_map.getVal(str_to_ul(sub_tokens[3])));
			}
		}
		else if (tokens[0] == "┌Node-Pointer") {
			is_processing = true;
			read_data.clear();
		}
		else if (tokens[0] == "└Node-Pointer") {
			is_processing = false;
			for (const Tokens& sub_tokens : read_data) {
				auto pointer = ptr<NODE::LINK::Pointer*>(pointer_map.getVal(str_to_ul(sub_tokens[1])));
				pointer->pointer = ptr<KL::Object*>(pointer_map.getVal(str_to_ul(sub_tokens[3])));

				auto gui_ptr = static_cast<GUI::NODE::LINK::Pointer*>(node_map.at(pointer));
				gui_ptr->pointer = ptr<KL::Object*>(pointer_map.getVal(str_to_ul(sub_tokens[3])));
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
}

void KL::Editor_File::f_saveAsciiNodeTree(Lace& lace, const KL::Node_Tree* data, const uint64& i) {
	uint64 j = 0;
	lace NL << "┌Node-Tree [ " << i << " ] " << data->name;
	lace++;
	lace NL PTR << uptr(data);
	lace NL << "┌Nodes( " << data->nodes.size() << " )";
	lace++;
	for (KL::Node* node : data->nodes) {
		lace NL << "┌Node [ " << j++ << " ] " << node->name;
		lace++;
		lace NL PTR << uptr(node);
		lace NL << "( " << node_map[node]->node_pos << " )";
		switch (node->type) {
		case NODE::Type::CONSTRAINT: {
			switch (static_cast<NODE::CONSTRAINT::Type>(node->sub_type)) {
			case NODE::CONSTRAINT::Type::PARENT: {
				lace NL << "Type CONSTRAINT :: PARENT";
				break;
			}
			}
			break;
		}
		case NODE::Type::EXEC: {
			switch (static_cast<NODE::EXEC::Type>(node->sub_type)) {
			case NODE::EXEC::Type::COUNTER: {
				lace NL << "Type EXEC :: COUNTER";
				break;
			}
			case NODE::EXEC::Type::SCRIPT: {
				lace NL << "Type EXEC :: SCRIPT";
				lace NL << "┌Script";
				lace NL SP << static_cast<KL::NODE::EXEC::Script*>(node)->script_id;
				lace NL << "└Script";
				break;
			}
			case NODE::EXEC::Type::TICK: {
				lace NL << "Type EXEC :: TICK";
				break;
			}
			}
			break;
		}
		case NODE::Type::LINK: {
			switch (static_cast<NODE::LINK::Type>(node->sub_type)) {
			case NODE::LINK::Type::POINTER: {
				lace NL << "Type LINK :: POINTER";
				lace NL << "┌Pointer";
				lace NL SP << "Type " << serialize(static_cast<NODE::LINK::Pointer*>(node)->pointer_type);
				lace NL << "└Pointer";
				break;
			}
			case NODE::LINK::Type::GET: {
				switch (static_cast<KL::NODE::LINK::Get*>(node)->mini_type) {
				case NODE::LINK::GET::Type::FIELD: {
					lace NL << "Type LINK :: GET :: FIELD";
					lace NL << "┌Field";
					lace NL SP << static_cast<NODE::LINK::GET::Field*>(node)->field;
					lace NL << "└Field";
					break;
				}
				}
				break;
			}
			case NODE::LINK::Type::SET: {
				switch (static_cast<KL::NODE::LINK::Set*>(node)->mini_type) {
				case NODE::LINK::SET::Type::EULER_ROTATION_X: {
					lace NL << "Type LINK :: SET :: TRANSFORM :: EULER_ROTATION :: X";
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
				lace NL << "Type MATH :: ADD";
				break;
			}
			case NODE::MATH::Type::SUB: {
				lace NL << "Type MATH :: SUB";
				break;
			}
			case NODE::MATH::Type::MUL: {
				lace NL << "Type MATH :: MUL";
				break;
			}
			case NODE::MATH::Type::DIV: {
				lace NL << "Type MATH :: DIV";
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
						lace NL << "Type UTIL :: CAST :: UINT_TO_DOUBLE";
						break;
					}
					case NODE::UTIL::CAST::Type::INT_TO_DOUBLE: {
						lace NL << "Type UTIL :: CAST :: INT_TO_DOUBLE";
						break;
					}
					break;
				}
				break;
			}
			case NODE::UTIL::Type::VIEW: {
				lace NL << "Type UTIL :: VIEW";
				break;
			}
			}
			break;
		}
		}
		lace--;
		lace NL << "└Node";
	}
	lace--;
	lace NL << "└Nodes";
	lace NL << "┌Build-E"; // [Node L Pointer] [Slot] [Slot] [Node R Pointer]
	lace++;
	for (const auto node_l : data->nodes) {
		for (const auto& port : node_l->outputs) {
			if (port->type == NODE::PORT::Type::EXEC_O) {
				const auto port_l = static_cast<KL::NODE::PORT::Exec_O_Port*>(port);
				if (port_l->connection) {
					const auto port_r = port_l->connection;
					const auto node_r = port_r->node;
					const uint64 port_l_id = distance(node_l->outputs.begin(), find(node_l->outputs.begin(), node_l->outputs.end(), port_l));
					const uint64 port_r_id = distance(node_r->inputs.begin(), find(node_r->inputs.begin(), node_r->inputs.end(), port_r));
					//const uint64 node_r_id = distance(data->nodes.begin(), find(data->nodes.begin(), data->nodes.end(), node_r));
					//lace NL node_l_id SP port_l_id SP node_r_id SP port_r_id;
					lace NL PTR << uptr(node_l) SP << port_l_id SP << port_r_id SP PTR << uptr(node_r);
				}
			}
		}
	}
	lace--;
	lace NL << "└Build-E";
	lace NL << "┌Build-D"; // [Node L Pointer] [Slot] [Slot] [Node R Pointer]
	lace++;
	for (const auto node_r : data->nodes) {
		for (const auto& port : node_r->inputs) {
			if (port->type == NODE::PORT::Type::DATA_I) {
				const auto port_r = static_cast<KL::NODE::PORT::Data_I_Port*>(port);
				if (port_r->connection) {
					const auto port_l = port_r->connection;
					const auto node_l = port_l->node;
					const uint64 port_l_id = distance(node_l->outputs.begin(), find(node_l->outputs.begin(), node_l->outputs.end(), port_l));
					const uint64 port_r_id = distance(node_r->inputs.begin(), find(node_r->inputs.begin(), node_r->inputs.end(), port_r));
					//const uint64 node_l_id = distance(data->nodes.begin(), find(data->nodes.begin(), data->nodes.end(), node_l));
					//lace NL node_l_id SP port_l_id SP node_r_id SP port_r_id;
					lace NL PTR << uptr(node_l) SP << port_l_id SP << port_r_id SP PTR << uptr(node_r);
				}
			}
		}
	}
	lace--;
	lace NL << "└Build-D";
	lace--;
	lace NL << "└Node-Tree";
}

void KL::Editor_File::f_saveBinaryNodeTree(Bin_Lace & bin, Node_Tree* data) {
	uint64 size = 0;
	Bin_Lace bytes;

	bytes << ul_to_u(data->name.size());
	bytes << data->name;
	bytes << uptr(data);
	bytes << ul_to_uh(data->nodes.size());

	size += 4;
	size += data->name.size();
	size += 8;
	size += 2;

	for (KL::Node* node : data->nodes) {
		bytes << ul_to_uh(node->name.size());
		bytes << node->name;
		bytes << uptr(node);
		const auto pos = node_map[node]->scenePos();
		bytes << uvec2(pos.x(), pos.y());

		size += 2;
		size += node->name.size();
		size += 8;
		size += 8;
	}

	bin << size;
	bin << bytes;
}