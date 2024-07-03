#include "Rendering/Cpu_Scene.hpp"

CPU_Scene f_parseCPUData() {
	CPU_Scene scene = CPU_Scene();

	ifstream file("./Resources/Assets/Test.krz", ios::in | ios::binary);
	vector<vector<string>> data = vector<vector<string>>();
	string line;
	bool is_processing = false;

	while (getline(file, line)) {
		vector<string> tokens = f_split(line);

		if (!tokens.empty()) {
			if (tokens[0] == "f-Header") {
				is_processing = true;
				data.clear();
				data.push_back(tokens);
			}
			else if (tokens[0] == "~-Header") {
				is_processing = false;
				data.push_back(tokens);
				scene.render_steps = str_to_u(data[1][1]);
				scene.render_step_stop = str_to_u(data[2][1]);
			}
			else if (tokens[0] == "f-Camera") {
				is_processing = true;
				data.clear();
				data.push_back(tokens);
			}
			else if (tokens[0] == "~-Camera") {
				is_processing = false;
				data.push_back(tokens);
				f_parseCamera(data, scene.camera);
			}
			else if (tokens[0] == "f-Material") {
				is_processing = true;
				data.clear();
				data.push_back(tokens);
			}
			else if (tokens[0] == "~-Material") {
				is_processing = false;
				data.push_back(tokens);
				f_parseMaterial(data, scene.materials);
			}
			else if (tokens[0] == "f-Empty") {
				is_processing = true;
				data.clear();
				data.push_back(tokens);
			}
			else if (tokens[0] == "~-Empty") {
				is_processing = false;
				data.push_back(tokens);
				f_parseEmpty(data, scene.empties);
			}
			else if (tokens[0] == "f-Light") {
				is_processing = true;
				data.clear();
				data.push_back(tokens);
			}
			else if (tokens[0] == "~-Light") {
				is_processing = false;
				data.push_back(tokens);
				f_parseLight(data, scene.lights);
			}
			else if (tokens[0] == "f-Curve") {
				is_processing = true;
				data.clear();
				data.push_back(tokens);
			}
			else if (tokens[0] == "~-Curve") {
				is_processing = false;
				data.push_back(tokens);
				f_parseCurve(data, scene.curves);
			}
			else if (tokens[0] == "f-Mesh") {
				is_processing = true;
				data.clear();
				data.push_back(tokens);
			}
			else if (tokens[0] == "~-Mesh") {
				is_processing = false;
				data.push_back(tokens);
				f_parseMesh(data, scene.meshes);
			}
			else if (is_processing) {
				data.push_back(tokens);
			}
		}
	}

	return scene;
}

void f_parseCamera(const vector<vector<string>>& data, Camera& camera) {
	for (const vector<string>& tokens : data) {
		if (tokens[0] == "Position:")
			camera.position = str_to_f(tokens[2], tokens[3], tokens[4]);
		if (tokens[0] == "Rotation:")
			camera.rotation = str_to_f(tokens[2], tokens[3], tokens[4]);
		if (tokens[0] == "Focal-Length:")
			camera.focal_length = str_to_f(tokens[1]);
		if (tokens[0] == "Sensor-Size:")
			camera.sensor_size = str_to_f(tokens[1]);
		if (tokens[0] == "FOV:")
			camera.focal_angle = str_to_f(tokens[1]);
	}
	camera.f_compileVectors();
}

void f_parseMaterial(const vector<vector<string>>& data, map<string, Material>& materials) {
	const string name = f_join(data[0], " ", 2);

	materials[name] = Material();
	for (const vector<string>& tokens : data) {
		if (tokens[0] == "Color:") {
			materials[name].color = str_to_f(
				tokens[2],
				tokens[3],
				tokens[4]
			);
		}
	}
}

void f_parseEmpty(const vector<vector<string>>& data, map<string, Empty>& empties) {

}

void f_parseLight(const vector<vector<string>>& data, map<string, Light>& lights) {

}

void f_parseCurve(const vector<vector<string>>& data, map<string, Curve>& curves) {
	const string name = f_join(data[0], " ", 2);
	curves[name] = Curve();

	vector<vector<string>> sub_data = vector<vector<string>>();
	bool is_processing = false;

	for (const vector<string>& tokens : data) {
		if (tokens[0] == "f-Handles") {
			is_processing = true;
			sub_data.clear();
		}
		else if (tokens[0] == "~-Handles") {
			vector<Curve_Point> spline;

			for (const vector<string>& point: sub_data) {
				spline.push_back(
					Curve_Point(
						str_to_f(point[3], point[4], point[5]),
						str_to_f(point[8])
					)
				);
			}
			curves[name].splines.push_back(spline);
		}
		else if (is_processing) {
			sub_data.push_back(tokens);
		}
	}
}

void f_parseMesh(const vector<vector<string>>& data, map<string, Mesh>& meshes) {
	const string name = f_join(data[0], " ", 2);
	meshes[name] = Mesh();

	vector<vector<string>> sub_data = vector<vector<string>>();
	bool is_processing = false;

	vector<Vertex> vertices;
	vector<Triangle> triangles;
	for (const vector<string>& tokens : data) {
		if (tokens[0] == "f-Vertices") {
			is_processing = true;
			sub_data.clear();
		}
		else if (tokens[0] == "~-Vertices") {
			is_processing = false;
			if (!sub_data.empty()) {
				for (const vector<string>& sub_line : sub_data) {
					vertices.push_back(
						Vertex(
							str_to_f(
								sub_line[3],
								sub_line[4],
								sub_line[5]
							)
						)
					);
				}
			}
			meshes[name].vertices = vertices;
		}
		else if (tokens[0] == "f-Faces") {
			is_processing = true;
			sub_data.clear();
		}
		else if (tokens[0] == "~-Faces") {
			is_processing = false;
			if (!sub_data.empty()) {
				for (const vector<string>& sub_line : sub_data) {
					triangles.push_back(
						Triangle(
							str_to_f(
								sub_line[3],
								sub_line[4],
								sub_line[5]
							),
							str_to_f(sub_line[10], sub_line[11], sub_line[12]),
							str_to_f(sub_line[15], sub_line[16], sub_line[17]),
							str_to_f(sub_line[20], sub_line[21], sub_line[22]),
							str_to_f(sub_line[28], sub_line[29]),
							str_to_f(sub_line[32], sub_line[33]),
							str_to_f(sub_line[36], sub_line[37])
						)
					);
				}
			}
			meshes[name].triangles = triangles;
		}
		else if (is_processing) {
			sub_data.push_back(tokens);
		}
	}

	vec3 bbmin = vec3(MAX_VEC1);
	vec3 bbmax = vec3(MIN_VEC1);

	for (const Vertex& vert : vertices) {
		bbmin.x = min(bbmin.x, vert.position.x);
		bbmin.y = min(bbmin.y, vert.position.y);
		bbmin.z = min(bbmin.z, vert.position.z);

		bbmax.x = max(bbmax.x, vert.position.x);
		bbmax.y = max(bbmax.y, vert.position.y);
		bbmax.z = max(bbmax.z, vert.position.z);
	}
	meshes[name].bvh.p_min = bbmin;
	meshes[name].bvh.p_max = bbmax;
	meshes[name].bvh.is_leaf = true;
}

vec3 f_averageVertexPos(const vector<Vertex>& vertices) {
	vec3 mean = vec3(0.0f);
	const vec1 count = static_cast<vec1>(vertices.size());

	for (const Vertex& v : vertices) {
		mean.x += v.position.x;
		mean.y += v.position.y;
		mean.z += v.position.z;
	}

	mean.x /= count;
	mean.y /= count;
	mean.z /= count;

	return mean;
}

void CPU_Scene::print() const {
	Lace data;
	data << "Camera {" << NL();
	data << TAB() << camera.position << NL();
	data << TAB() << camera.rotation << NL();
	data << TAB() << camera.focal_length << NL();
	data << TAB() << camera.sensor_size << NL();
	data << "} Camera" << NL();
	data << "Materials {" << NL();
	for (const pair<string, Material>& mat : materials) {
		data << TAB() << mat.second.color << NL();
	}
	data << "} Materials" << NL();
	data << "Lights {" << NL();
	for (const pair<string, Light>& light : lights) {
		data << TAB() << light.second.color << NL();
	}
	data << "} Lights" << NL();
	cout << data.str();
}