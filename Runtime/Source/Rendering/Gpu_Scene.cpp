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
						loadData(data, pointer_map);
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
	loadTexture("./Resources/Ganyu.jpg");
}

void GPU_Scene::print() const {
	Lace data;
	data << "Triangles " << triangles.size() << NL();
	for (const GPU_Triangle& tri : triangles)
		data << TAB() << tri.print() << NL();
	data << "BVHs " << bvh_nodes.size() << NL();
	for (const GPU_BVH& bvh : bvh_nodes)
		data << TAB() << bvh.print() << NL();
	data << "Textures " << textures.size() << NL();
	for (const GPU_Texture& texture : textures)
		data << TAB() << texture.print() << NL();
	data << "Texture Data " << texture_data.size() << NL() << TAB();
	for (const uint& pixel : texture_data)
		data << S() << pixel;
	data << NL();
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
	printSize("	Triangles            ", triangles);
	printSize("	BVHs                 ", bvh_nodes);
	printSize("	Textures             ", textures);
	printSize("	Texture Data         ", texture_data);
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

void GPU_Scene::loadHeader(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
}

void GPU_Scene::loadNodeTree(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	
}

void GPU_Scene::loadMaterial(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadData(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	if      (token_data[0][2] == "Atmosphere")
		loadAtmosphere(token_data, pointer_map);
	else if (token_data[0][2] == "Primitive")
		loadPrimitive (token_data, pointer_map);
	else if (token_data[0][2] == "Skeleton")
		loadSkeleton  (token_data, pointer_map);
	else if (token_data[0][2] == "Camera")
		loadCamera    (token_data, pointer_map);
	else if (token_data[0][2] == "Volume")
		loadVolume    (token_data, pointer_map);
	else if (token_data[0][2] == "Curve")
		loadCurve     (token_data, pointer_map);
	else if (token_data[0][2] == "Empty")
		loadEmpty     (token_data, pointer_map);
	else if (token_data[0][2] == "Force")
		loadForce     (token_data, pointer_map);
	else if (token_data[0][2] == "Group")
		loadGroup     (token_data, pointer_map);
	else if (token_data[0][2] == "Light")
		loadLight     (token_data, pointer_map);
	else if (token_data[0][2] == "Mesh")
		loadMesh      (token_data, pointer_map);
	else if (token_data[0][2] == "Sfx")
		loadSfx       (token_data, pointer_map);
	else if (token_data[0][2] == "Vfx")
		loadVfx       (token_data, pointer_map);
}

void GPU_Scene::loadAtmosphere(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadPrimitive(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadSkeleton(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadCamera(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadVolume(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadCurve(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadEmpty(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadForce(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadGroup(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadLight(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadMesh(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	vector<vec3> vertices;
	vector<GPU_Triangle> mesh_triangles;

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
				mesh_triangles[str_to_ul(token_data[0])].normal_a = str_to_f(token_data[1], token_data[2], token_data[3]);
				mesh_triangles[str_to_ul(token_data[0])].normal_b = str_to_f(token_data[4], token_data[5], token_data[6]);
				mesh_triangles[str_to_ul(token_data[0])].normal_c = str_to_f(token_data[7], token_data[8], token_data[9]);
			}
		}
		else if (tokens[0] == "└UVs") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				mesh_triangles[str_to_ul(token_data[0])].uv_a_x = str_to_f(token_data[1]);
				mesh_triangles[str_to_ul(token_data[0])].uv_a_y = str_to_f(token_data[2]);
				mesh_triangles[str_to_ul(token_data[0])].uv_b_x = str_to_f(token_data[3]);
				mesh_triangles[str_to_ul(token_data[0])].uv_b_y = str_to_f(token_data[4]);
				mesh_triangles[str_to_ul(token_data[0])].uv_c_x = str_to_f(token_data[5]);
				mesh_triangles[str_to_ul(token_data[0])].uv_c_y = str_to_f(token_data[6]);
			}
		}
		else if (tokens[0] == "└Faces") {
			is_processing = false;
			for (const vector<string>& token_data : read_data) {
				mesh_triangles.push_back(
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

	const uint64 triangle_offset = triangles.size();
	const uint bvh_depth = d_to_u(glm::log2(ul_to_d(mesh_triangles.size()) / 64.0));

	BVH_Builder bvh_build = BVH_Builder(mesh_triangles, bvh_depth);
	bvh_nodes.insert(bvh_nodes.end(), bvh_build.node_list.begin(), bvh_build.node_list.end());
	triangles.insert(triangles.end(), bvh_build.triangles.begin(), bvh_build.triangles.end());
	cout << "Triangle Count: " << mesh_triangles.size() << "  BVH Depth: " << bvh_depth << "  BVH Nodes: " << bvh_nodes.size() << endl;
}

void GPU_Scene::loadSfx(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadVfx(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadObject(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

};

void GPU_Scene::loadScene(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {

}

void GPU_Scene::loadBuild(const vector<vector<string>>& token_data, map<uint64, void*>& pointer_map) {
	
}

void GPU_Scene::loadTexture(const string& file_path) {
	uvec2 resolution;
	vector<uint> data = loadRgba8Texture(file_path, resolution);

	texture_data.insert(texture_data.end(), data.begin(), data.end());
	textures.push_back(GPU_Texture(0U, resolution.x, resolution.y, 0U));
}

Lace GPU_Triangle::print() const {
	Lace lace;
	lace << "Tri: (" << pos_a<< ") : (" << normal_a << ") : (" << vec2(uv_a_x, uv_a_y) << ") | (" << pos_b << ") : (" << normal_b << ") : (" << vec2(uv_b_x, uv_b_y) << ") | (" << pos_c << ") : (" << normal_c << ") : (" << vec2(uv_c_x, uv_c_y) << ")";
	return lace;
}

void GPU_BVH::growToInclude(const vec3& min, const vec3& max) {
	p_min.x = min.x < p_min.x ? min.x : p_min.x;
	p_min.y = min.y < p_min.y ? min.y : p_min.y;
	p_min.z = min.z < p_min.z ? min.z : p_min.z;
	p_max.x = max.x > p_max.x ? max.x : p_max.x;
	p_max.y = max.y > p_max.y ? max.y : p_max.y;
	p_max.z = max.z > p_max.z ? max.z : p_max.z;
}

vec3 GPU_BVH::getSize() {
	return p_max - p_min;
}

vec3 GPU_BVH::getCenter() {
	return (p_min + p_max) / 2.0f;
}

Lace GPU_BVH::print() const {
	Lace lace;
	lace << "Bvh: (" << p_min<< ") | (" << p_max << ") | " << pointer << " | " << tri_count;
	return lace;
}

BVH_Builder::BVH_Builder(const vector<GPU_Triangle>& triangles, const uint& depth) :
	triangles(triangles)
{
	for (uint i = 0; i < len32(triangles); i++) {
		const vec3 a = triangles[i].pos_a;
		const vec3 b = triangles[i].pos_b;
		const vec3 c = triangles[i].pos_c;
		const vec3 center = (a + b + c) / 3.0f;
		const vec3 max = glm::max(glm::max(a, b), c);
		const vec3 min = glm::min(glm::min(a, b), c);
		bvh_tris.push_back(BVH_Triangle(min, max, center, i, triangles[i]));
		mesh_bounds.growToInclude(min, max);
	}

	node_list.push_back(mesh_bounds);
	splitBvh(0, 0, len32(triangles), depth);

	this->triangles.clear();
	for (uint i = 0; i < len32(bvh_tris); i++) {
		this->triangles.push_back(bvh_tris[i].tri);
	}
}

void BVH_Builder::splitBvh(const uint& parentIndex, const uint& triGlobalStart, const uint& triNum, const uint& depth) {
	GPU_BVH parent = node_list[parentIndex];
	vec3 size = parent.getSize();
	float parentCost = nodeCost(size, triNum);

	uint8 split_axis;
	float splitPos;
	float cost;

	splitAxis(parent, triGlobalStart, triNum, split_axis, splitPos, cost);

	if (cost < parentCost && depth > 0) {
		GPU_BVH boundsLeft;
		GPU_BVH boundsRight;
		uint numOnLeft = 0;

		for (uint i = triGlobalStart; i < triGlobalStart + triNum; i++) {
			BVH_Triangle tri = bvh_tris[i];
			if (tri.center[split_axis] < splitPos) {
				boundsLeft.growToInclude(tri.p_min, tri.p_max);

				BVH_Triangle swap = bvh_tris[triGlobalStart + numOnLeft];
				bvh_tris[triGlobalStart + numOnLeft] = tri;
				bvh_tris[i] = swap;
				numOnLeft++;
			}
			else {
				boundsRight.growToInclude(tri.p_min, tri.p_max);
			}
		}

		uint numOnRight = triNum - numOnLeft;
		uint triStartLeft = triGlobalStart;
		uint triStartRight = triGlobalStart + numOnLeft;

		node_list.push_back(GPU_BVH(boundsLeft.p_min, boundsLeft.p_max, triStartLeft));
		uint childIndexRight = len32(node_list);
		uint childIndexLeft = childIndexRight - 1;
		node_list.push_back(GPU_BVH(boundsRight.p_min, boundsRight.p_max, triStartRight));

		parent.pointer = childIndexLeft;
		node_list[parentIndex] = parent;

		splitBvh(childIndexLeft, triGlobalStart, numOnLeft, depth - 1);
		splitBvh(childIndexRight, triGlobalStart + numOnLeft, numOnRight, depth - 1);
	}
	else {
		parent.pointer = triGlobalStart;
		parent.tri_count = triNum;
		node_list[parentIndex] = parent;
	}
}

void BVH_Builder::splitAxis(const GPU_BVH& node, const uint& start, const uint& count, uint8& axis, float& pos, float& cost) const {
	if (count <= 1) {
		axis = 0;
		pos = 0.0f;
		cost = MAX_VEC1;
		return;
	}

	float bestSplitPos = 0;
	uint8 bestSplitAxis = 0;
	const uint8 numSplitTests = 5;

	float bestCost = MAX_VEC1;

	for (uint8 axis = 0; axis < 3; axis++) {
		for (uint8 i = 0; i < numSplitTests; i++) {
			float splitT = (u_to_f(i) + 1.0f) / (u_to_f(numSplitTests) + 1.0f);
			float splitPos = glm::lerp(node.p_min[axis], node.p_max[axis], splitT);
			float cost = splitEval(axis, splitPos, start, count);
			if (cost < bestCost) {
				bestCost = cost;
				bestSplitPos = splitPos;
				bestSplitAxis = axis;
			}
		}
	}

	axis = bestSplitAxis;
	pos = bestSplitPos;
	cost = bestCost;
}

float BVH_Builder::splitEval(const uint8& splitAxis, const float& splitPos, const uint& start, const uint& count) const {
	GPU_BVH boundsLeft;
	GPU_BVH boundsRight;
	uint numOnLeft = 0;
	uint numOnRight = 0;

	for (uint i = start; i < start + count; i++) {
		BVH_Triangle tri = bvh_tris[i];
		if (tri.center[splitAxis] < splitPos)
		{
			boundsLeft.growToInclude(tri.p_min, tri.p_max);
			numOnLeft++;
		}
		else
		{
			boundsRight.growToInclude(tri.p_min, tri.p_max);
			numOnRight++;
		}
	}

	float costA = nodeCost(boundsLeft.getSize(), numOnLeft);
	float costB = nodeCost(boundsRight.getSize(), numOnRight);
	return costA + costB;
}

float BVH_Builder::nodeCost(const vec3& size, const uint& numTriangles) {
	const float halfArea = size.x * size.y + size.x * size.z + size.y * size.z;
	return halfArea * numTriangles;
}

Lace GPU_Texture::print() const {
	Lace lace;
	lace << "Texture: (" << start<< ") | (" << width << " x " << height << ") | " << format;
	return lace;
}