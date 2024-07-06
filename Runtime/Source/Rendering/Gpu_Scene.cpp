#include "Rendering/Gpu_Scene.hpp"

GPU_Scene f_parseGPUData(const string& data_file_path, const string& shader_file_path) {
	Camera                    camera;
	vector<GPU_Material>      materials;
	vector<GPU_Light>         lights;
	vector<GPU_Spline_Point>  spline_controls;
	vector<GPU_Spline>        splines;
	vector<GPU_Curve>         curves;
	vector<GPU_Vertex>        vertices;
	vector<GPU_Triangle>      triangles;
	vector<GPU_Mesh>          meshes;
	vector<GPU_BVH>           bvh_nodes;

	ifstream data_file(data_file_path, ios::binary);
	if (data_file.is_open()) {
		vector<vector<string>> data = vector<vector<string>>();
		string line;

		while (getline(data_file, line)) {
			vector<string> tokens = f_split(line);
			if (!tokens.empty()) {
			}
		}
	}

	ifstream shader_file(shader_file_path, ios::binary);
	if (shader_file.is_open()) {
		vector<vector<string>> data = vector<vector<string>>();
		string line;

		while (getline(shader_file, line)) {
			vector<string> tokens = f_split(line);
			if (!tokens.empty()) {
			}
		}
	}

	return GPU_Scene(
		camera,
		materials,
		lights,
		spline_controls,
		splines,
		curves,
		vertices,
		triangles,
		meshes,
		bvh_nodes
	);
}

void GPU_Scene::print() const {
	Lace data;
	data << "Materials" << NL();
	for (const GPU_Material& mat : materials) {
		data << TAB() << mat.albedo << NL();
	}
	data << "Light" << NL();
	for (const GPU_Light& light : lights) {
		data << TAB() << light.color << NL();
	}
	data << "Spline_Handle" << NL();
	for (const GPU_Spline_Point& handle : spline_controls) {
		data << TAB() << handle.position << NL();
	}
	data << "Spline" << NL();
	for (const GPU_Spline& spline : splines) {
		data << TAB() << spline.handle_pointer << NL();
	}
	data << "Curve" << NL();
	for (const GPU_Curve& curve : curves) {
		data << TAB() << curve.spline_pointer << NL();
	}
	data << "Vertex" << NL();
	data << TAB() << vertices.size() << NL();
	//for (const GPU_Vertex& vert : vertices) {
	//	data << TAB() << vert.position << NL();
	//}
	data << "Triangle" << NL();
	data << TAB() << triangles.size() << NL();
	//for (const GPU_Triangle& tri : triangles) {
	//	data << TAB() << tri.vertex_pointer << NL();
	//}
	data << "Mesh" << NL();
	for (const GPU_Mesh& mesh : meshes) {
		data << TAB() << mesh.triangle_pointer << NL();
	}
	data << "BVH" << NL();
	for (const GPU_BVH& bvh : bvh_nodes) {
		data << TAB() << bvh.mesh_pointer << NL();
		data << TAB() << bvh.p_min << NL();
		data << TAB() << bvh.p_max << NL();
	}
	cout << data.str();
}

void GPU_Scene::printInfo(const uint64& max_size) const {
	cout << "GPU Data:" << endl;
	cout << fixed << setprecision(4) << "    Materials             : " << static_cast<double>(sizeof(GPU_Material)     * materials.size()) / (1024.0 * 1024.0)       << "mb  | " << static_cast<double>(sizeof(GPU_Material)     * materials.size()) / 1024.0       << "kb  | " << sizeof(GPU_Material)     * materials.size()       << "b" << endl;
	cout << fixed << setprecision(4) << "    Lights                : " << static_cast<double>(sizeof(GPU_Light)        * lights.size()) / (1024.0 * 1024.0)          << "mb  | " << static_cast<double>(sizeof(GPU_Light)        * lights.size()) / 1024.0          << "kb  | " << sizeof(GPU_Light)        * lights.size()          << "b" << endl;
	cout << fixed << setprecision(4) << "    Spline Control Points : " << static_cast<double>(sizeof(GPU_Spline_Point) * spline_controls.size()) / (1024.0 * 1024.0) << "mb  | " << static_cast<double>(sizeof(GPU_Spline_Point) * spline_controls.size()) / 1024.0 << "kb  | " << sizeof(GPU_Spline_Point) * spline_controls.size() << "b" << endl;
	cout << fixed << setprecision(4) << "    Splines               : " << static_cast<double>(sizeof(GPU_Spline)       * splines.size()) / (1024.0 * 1024.0)         << "mb  | " << static_cast<double>(sizeof(GPU_Spline)       * splines.size()) / 1024.0         << "kb  | " << sizeof(GPU_Spline)       * splines.size()         << "b" << endl;
	cout << fixed << setprecision(4) << "    Curves                : " << static_cast<double>(sizeof(GPU_Curve)        * curves.size()) / (1024.0 * 1024.0)          << "mb  | " << static_cast<double>(sizeof(GPU_Curve)        * curves.size()) / 1024.0          << "kb  | " << sizeof(GPU_Curve)        * curves.size()          << "b" << endl;
	cout << fixed << setprecision(4) << "    Vertices              : " << static_cast<double>(sizeof(GPU_Vertex)       * vertices.size()) / (1024.0 * 1024.0)        << "mb  | " << static_cast<double>(sizeof(GPU_Vertex)       * vertices.size()) / 1024.0        << "kb  | " << sizeof(GPU_Vertex)       * vertices.size()        << "b" << endl;
	cout << fixed << setprecision(4) << "    Triangles             : " << static_cast<double>(sizeof(GPU_Triangle)     * triangles.size()) / (1024.0 * 1024.0)       << "mb  | " << static_cast<double>(sizeof(GPU_Triangle)     * triangles.size()) / 1024.0       << "kb  | " << sizeof(GPU_Triangle)     * triangles.size()       << "b" << endl;
	cout << fixed << setprecision(4) << "    Meshes                : " << static_cast<double>(sizeof(GPU_Mesh)         * meshes.size()) / (1024.0 * 1024.0)          << "mb  | " << static_cast<double>(sizeof(GPU_Mesh)         * meshes.size()) / 1024.0          << "kb  | " << sizeof(GPU_Mesh)         * meshes.size()          << "b" << endl;
	cout << fixed << setprecision(4) << "    BVHs                  : " << static_cast<double>(sizeof(GPU_BVH)          * bvh_nodes.size()) / (1024.0 * 1024.0)       << "mb  | " << static_cast<double>(sizeof(GPU_BVH)          * bvh_nodes.size()) / 1024.0       << "kb  | " << sizeof(GPU_BVH)          * bvh_nodes.size()       << "b" << endl;

	if (
		sizeof(GPU_Material) * materials.size() > max_size or
		sizeof(GPU_Light) * lights.size() > max_size or
		sizeof(GPU_Spline_Point) * spline_controls.size() > max_size or
		sizeof(GPU_Spline) * splines.size() > max_size or
		sizeof(GPU_Curve) * curves.size() > max_size or
		sizeof(GPU_Vertex) * vertices.size() > max_size or
		sizeof(GPU_Triangle) * triangles.size() > max_size or
		sizeof(GPU_Mesh) * meshes.size() > max_size or
		sizeof(GPU_BVH) * bvh_nodes.size() > max_size
		)
		throw "GPU out of memory";
}