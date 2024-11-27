#include "GPU/GPU_Scene.hpp"

#include "Utils/Session.hpp"

KL::GPU::Scene::Scene() {
	mesh_instances = {};
	mesh_triangles = {};
	mesh_blas = {};

	textures = {};
	texture_data = {};

	point_lights.push_back(Point_Light(vec3(1.5f), 0.5f, vec3(1.0f), 5.0f));
}

void KL::GPU::Scene::f_init() {
	mesh_instances.clear();
	mesh_triangles.clear();
	mesh_blas.clear();

	textures.clear();
	texture_data.clear();
}

void KL::GPU::Scene::printInfo() const {
	cout << endl << "GPU Vram Information" << endl;
	const uint label_size = 16;
	printSizeHeader(label_size);

	printSizeRow("Mesh Instances ", label_size, mesh_instances);
	printSizeRow("Mesh Triangles ", label_size, mesh_triangles);
	printSizeRow("Mesh BVHs      ", label_size, mesh_blas);

	printSizeRow("Textures       ", label_size, textures);
	printSizeRow("Texture Data   ", label_size, texture_data);
}

void KL::GPU::Scene::updateTextures() { // TODO handle different formats
	for (SHADER::Texture* texture : FILE->textures) {
		vector<uint> data = texture->toPackedRgba8Texture();
		textures.push_back(GPU::Texture(ul_to_u(texture_data.size()), texture->resolution.x, texture->resolution.y, SHADER::TEXTURE::Format::RGBA8u, SHADER::TEXTURE::Filtering::RAW));
		texture_data.insert(texture_data.end(), data.begin(), data.end());
	}
}

void KL::GPU::Scene::f_update() {
	mesh_instances.clear();
	mesh_triangles.clear();
	mesh_blas.clear();

	vector<uint64> blas_tracker;

	//mesh_instances.push_back(Object_Instance(mat4(1.0), ul_to_u(mesh_bvh.size())));
	for (KL::Object* object : FILE->active_scene.pointer->objects) {
		object->f_compileMatrix();
		mesh_instances.push_back(Object_Instance(d_to_f(object->transform_matrix), ul_to_u(mesh_blas.size())));

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
		const uint bvh_depth = max(1u, min(32u, d_to_u(pow(ul_to_d(triangles.size()), 0.25))));

		BVH::BLAS blas = BVH::BLAS(triangles, bvh_depth);
		blas_tracker.push_back(mesh_blas.size());
		mesh_blas.insert(mesh_blas.end(), blas.mesh_blas.begin(), blas.mesh_blas.end());
		mesh_triangles.insert(mesh_triangles.end(), blas.mesh_triangles.begin(), blas.mesh_triangles.end());
	}

	//camera_lenses.clear();
	//for (const auto& lens : FILE->f_activeCamera()->getCamera()->lenses) {
	//	camera_lenses.push_back(KL::GPU::Camera_Lens(lens.curvature_radius, lens.aperture_radius, lens.z_distance, lens.ior));
	//}
}