#include "Rendering/Gpu_Scene.hpp"

GPU_Scene::GPU_Scene(const string& data_file_path, const string& shader_file_path) {

	ifstream file(data_file_path, ios::binary);
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
						COUT_S "[File] Reading: Build..." COUT_E
						is_processing = Parse_Type::NONE;
						//f_loadBuild(data, pointer_map, node_gui_map);
					}
					else if (is_processing == Parse_Type::NODE_TREE and tokens[0] == "└Node-Tree") {
						COUT_S "[File] Reading: Node-Tree..." COUT_E
						is_processing = Parse_Type::NONE;
						//nodes.push_back(f_loadNodeTree(data, pointer_map, node_gui_map));
					}
					else if (is_processing == Parse_Type::MATERIAL and tokens[0] == "└Material") {
						COUT_S "[File] Reading: Material..." COUT_E
						is_processing = Parse_Type::NONE;
						//materials.push_back(f_loadMaterial(data, pointer_map));
					}
					else if (is_processing == Parse_Type::HEADER and tokens[0] == "└Header") {
						COUT_S "[File] Loading: " << data_file_path << "..." COUT_E
						is_processing = Parse_Type::NONE;
						//f_loadHeader(data, pointer_map);
					}
					else if (is_processing == Parse_Type::OBJECT and tokens[0] == "└Object") {
						COUT_S "[File] Reading: Object..." COUT_E
						is_processing = Parse_Type::NONE;
						//objects.push_back(f_loadObject(data, pointer_map));
					}
					else if (is_processing == Parse_Type::SCENE and tokens[0] == "└Scene") {
						COUT_S "[File] Reading: Scene..." COUT_E
						is_processing = Parse_Type::NONE;
						//scenes.push_back(f_loadScene(data, pointer_map));
					}
					else if (is_processing == Parse_Type::DATA and tokens[0] == "└Data") {
						COUT_S "[File] Reading: Data..." COUT_E
						is_processing = Parse_Type::NONE;
						f_loadData(data, pointer_map);
					}
					else {
						data.push_back(tokens);
					}
				}
			}
		}
	}
	else {
		COUT_S "[File] Error Opening File" COUT_E
		return;
	}
	COUT_S "[File] Loaded" COUT_E

	//ifstream shader_file(shader_file_path, ios::binary);
	//if (shader_file.is_open()) {
	//	vector<vector<string>> data = vector<vector<string>>();
	//	string line;
	//
	//	while (getline(shader_file, line)) {
	//		vector<string> tokens = f_split(line);
	//		if (!tokens.empty()) {
	//		}
	//	}
	//}
}

void GPU_Scene::print() const {
	Lace data;
	data << "Triangles " << triangles.size() << NL();
	for (const GPU_Triangle& tri : triangles)
		data << TAB() << tri.print() << NL();
	data << "BVHs " << bvh_nodes.size() << NL();
	for (const GPU_BVH& bvh : bvh_nodes)
		data << TAB() << bvh.print() << NL();
	//data << "Materials" << NL();
	//for (const GPU_Material& mat : materials) {
	//	data << TAB() << mat.albedo << NL();
	//}
	//data << "Light" << NL();
	//for (const GPU_Light& light : lights) {
	//	data << TAB() << light.color << NL();
	//}
	//data << "Spline_Handle" << NL();
	//for (const GPU_Spline_Point& handle : spline_controls) {
	//	data << TAB() << handle.position << NL();
	//}
	//data << "Spline" << NL();
	//for (const GPU_Spline& spline : splines) {
	//	data << TAB() << spline.handle_pointer << NL();
	//}
	//data << "Curve" << NL();
	//for (const GPU_Curve& curve : curves) {
	//	data << TAB() << curve.spline_pointer << NL();
	//}
	//data << "Vertex" << NL();
	//data << TAB() << vertices.size() << NL();
	//for (const GPU_Vertex& vert : vertices) {
	//	data << TAB() << vert.position << NL();
	//}
	//data << "Mesh" << NL();
	//for (const GPU_Mesh& mesh : meshes) {
	//	data << TAB() << mesh.triangle_pointer << NL();
	//}
	cout << data.str() << endl;
}

void GPU_Scene::printInfo(const uint64& max_size) const {
	cout << "GPU Data:" << endl;
	f_printSize("	Triangles            ", triangles);
	f_printSize("	BVHs                 ", bvh_nodes);
	//f_printSize("	Materials            ", materials);
	//f_printSize("	Lights               ", lights);
	//f_printSize("	Spline Control Points", spline_controls);
	//f_printSize("	Splines              ", splines);
	//f_printSize("	Curves               ", curves);
	//f_printSize("	Vertices             ", vertices);
	//f_printSize("	Meshes               ", meshes);
	if (
		//sizeof(GPU_Material)     * materials.size()       +
		//sizeof(GPU_Light)        * lights.size()          +
		//sizeof(GPU_Spline_Point) * spline_controls.size() +
		//sizeof(GPU_Spline)       * splines.size()         +
		//sizeof(GPU_Curve)        * curves.size()          +
		//sizeof(GPU_Vertex)       * vertices.size()        +
		//sizeof(GPU_Mesh)         * meshes.size()          +
		sizeof(GPU_Triangle)     * triangles.size()       +
		sizeof(GPU_BVH)          * bvh_nodes.size() > max_size
	)
		throw runtime_error("GPU out of memory");
}

void GPU_Scene::f_loadHeader(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
}

void GPU_Scene::f_loadNodeTree(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	
}

void GPU_Scene::f_loadMaterial(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadData(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	if      (token_data[0][2] == "Atmosphere")
		f_loadAtmosphere(token_data, pointer_map);
	else if (token_data[0][2] == "Primitive")
		f_loadPrimitive (token_data, pointer_map);
	else if (token_data[0][2] == "Skeleton")
		f_loadSkeleton  (token_data, pointer_map);
	else if (token_data[0][2] == "Camera")
		f_loadCamera    (token_data, pointer_map);
	else if (token_data[0][2] == "Volume")
		f_loadVolume    (token_data, pointer_map);
	else if (token_data[0][2] == "Curve")
		f_loadCurve     (token_data, pointer_map);
	else if (token_data[0][2] == "Empty")
		f_loadEmpty     (token_data, pointer_map);
	else if (token_data[0][2] == "Force")
		f_loadForce     (token_data, pointer_map);
	else if (token_data[0][2] == "Group")
		f_loadGroup     (token_data, pointer_map);
	else if (token_data[0][2] == "Light")
		f_loadLight     (token_data, pointer_map);
	else if (token_data[0][2] == "Mesh")
		f_loadMesh      (token_data, pointer_map);
	else if (token_data[0][2] == "Sfx")
		f_loadSfx       (token_data, pointer_map);
	else if (token_data[0][2] == "Vfx")
		f_loadVfx       (token_data, pointer_map);
}

void GPU_Scene::f_loadAtmosphere(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadPrimitive(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadSkeleton(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadCamera(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadVolume(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadCurve(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadEmpty(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadForce(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadGroup(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadLight(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadMesh(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	vector<vec3> vertices;

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
		else if (tokens[0] == "└Vertices") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				vertices.push_back(str_to_f(token_data[1], token_data[2], token_data[3]));
			}
		}
		else if (tokens[0] == "└Normals") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				triangles[str_to_ul(token_data[0])].normal_a = str_to_f(token_data[1], token_data[2], token_data[3]);
				triangles[str_to_ul(token_data[0])].normal_b = str_to_f(token_data[4], token_data[5], token_data[6]);
				triangles[str_to_ul(token_data[0])].normal_c = str_to_f(token_data[7], token_data[8], token_data[9]);
			}
		}
		else if (tokens[0] == "└UVs") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				triangles[str_to_ul(token_data[0])].uv_a = str_to_f(token_data[1], token_data[2]);
				triangles[str_to_ul(token_data[0])].uv_b = str_to_f(token_data[3], token_data[4]);
				triangles[str_to_ul(token_data[0])].uv_c = str_to_f(token_data[5], token_data[6]);
			}
		}
		else if (tokens[0] == "└Faces") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				triangles.push_back(
					GPU_Triangle(
						vertices[str_to_ul(token_data[2])],
						vertices[str_to_ul(token_data[3])],
						vertices[str_to_ul(token_data[4])]
					)
				);
			}
		}
		else if (is_processing) {
			read_data.push_back(tokens);
		}
	}
}

void GPU_Scene::f_loadSfx(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadVfx(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadObject(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

};

void GPU_Scene::f_loadScene(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::f_loadBuild(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	
}

Lace GPU_Triangle::print() const {
	Lace lace;
	lace << "Tri: (" << pos_a<< ") : (" << normal_a << ") : (" << uv_a << ") | (" << pos_a << ") : (" << normal_a << ") : (" << uv_a << ") | (" << pos_a << ") : (" << normal_a << ") : (" << uv_a << ")";
	return lace;
}

Lace GPU_BVH::print() const {
	Lace lace;
	lace << "Bvh: (" << p_min<< ") | (" << p_max << ") | " << pointer << " | " << tri_count;
	return lace;
}