#include "GPU/GPU_Scene.hpp"

#include "Utils/Session.hpp"

KL::GPU::Scene::Scene() {
	mesh_instances = {};
	mesh_triangles = {};
	mesh_bvh = {};

	textures = {};
	texture_data = {};

	point_lights.push_back(Point_Light(vec3(1.5f), 0.5f, vec3(1.0f), 5.0f));
}

void KL::GPU::Scene::f_init() {
	mesh_instances.clear();
	mesh_triangles.clear();
	mesh_bvh.clear();

	textures.clear();
	texture_data.clear();
}

void KL::GPU::Scene::printInfo() const {
	cout << endl << "GPU Data:" << endl;
	printSize("  Mesh Instances    ", mesh_instances);
	printSize("  Mesh Triangles    ", mesh_triangles);
	printSize("  Mesh BVHs         ", mesh_bvh);

	printSize("  Textures     ", textures);
	printSize("  Texture Data ", texture_data);
}

void KL::GPU::Scene::updateTextures() { // TODO handle different formats
	for (SHADER::Texture* texture : FILE->textures) {
		vector<uint> data = texture->toRgba8Texture();
		textures.push_back(GPU::Texture(ul_to_u(texture_data.size()), texture->resolution.x, texture->resolution.y, 0));
		texture_data.insert(texture_data.end(), data.begin(), data.end());
	}
}

void KL::GPU::Scene::f_update() {
	mesh_instances.clear();
	mesh_triangles.clear();
	mesh_bvh.clear();

	//mesh_instances.push_back(Object_Instance(mat4(1.0), ul_to_u(mesh_bvh.size())));
	for (KL::Object* object : FILE->active_scene.pointer->objects) {
		object->f_compileMatrix();
		mesh_instances.push_back(Object_Instance(d_to_f(object->transform_matrix), ul_to_u(mesh_bvh.size())));

		vector<GPU::Triangle> triangles;
		if (object->data->type == KL::OBJECT::DATA::Type::MESH) {
			KL::OBJECT::DATA::Mesh* mesh = object->getMesh();
			for (KL::OBJECT::DATA::MESH::Face* face : mesh->faces) {
				triangles.push_back(faceToGpuTri(object->transform_matrix, mesh, face));
			}
		}
		else if (object->data->type == KL::OBJECT::DATA::Type::GROUP) {
			const KL::OBJECT::DATA::Group* group = object->getGroup();
			for (KL::Object* sub_object : group->objects) {
				sub_object->f_compileMatrix();
				sub_object->transform_matrix *= object->transform_matrix;
				if (sub_object->data->type == KL::OBJECT::DATA::Type::MESH) {
					KL::OBJECT::DATA::Mesh* mesh = sub_object->getMesh();
					for (KL::OBJECT::DATA::MESH::Face* face : mesh->faces) {
						triangles.push_back(faceToGpuTri(sub_object->transform_matrix, mesh, face));
					}
				}
			}
		}
		const uint64 triangle_offset = mesh_triangles.size();
		const uint bvh_depth = d_to_u(glm::log2(ul_to_d(triangles.size()) / 64.0));

		BVH::Builder bvh_build = BVH::Builder(triangles, bvh_depth);
		mesh_bvh.insert(mesh_bvh.end(), bvh_build.node_list.begin(), bvh_build.node_list.end());
		mesh_triangles.insert(mesh_triangles.end(), bvh_build.triangles.begin(), bvh_build.triangles.end());
	}

	//camera_lenses.clear();
	//for (const auto& lens : FILE->f_activeCamera()->getCamera()->lenses) {
	//	camera_lenses.push_back(KL::GPU::Camera_Lens(lens.curvature_radius, lens.aperture_radius, lens.z_distance, lens.ior));
	//}
}