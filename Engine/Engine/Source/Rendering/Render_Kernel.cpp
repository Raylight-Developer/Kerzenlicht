#include "Rendering/Render_Kernel.hpp"

#include "Workspaces/Viewport.hpp"

Render_Kernel::Render_Kernel(GUI::WORKSPACE::Viewport_CPU_Renderer* viewport, CLASS::File* file) :
	resolution(viewport->render_resolution),
	aspect_ratio(viewport->render_aspect_ratio),
	inv_aspect_ratio(1.0 / aspect_ratio),
	file(file)
{}

dvec3 Render_Kernel::f_renderPixel(const dvec1& x, const dvec1& y) {
	dvec3 render_output = dvec3(0.0, 0.0, 0.0);

	const uint samples_per_pixel = 2;
	for (int sample = 0; sample < samples_per_pixel; sample++) {
		const dvec2 uv = dvec2(x, y * inv_aspect_ratio) / u_to_d(resolution) - dvec2(0.5, 0.5 * inv_aspect_ratio);
		render_output += f_renderSample(uv);
	}
	render_output /= u_to_d(samples_per_pixel);
	return clamp(render_output, 0.0, 1.0);
}

dvec3 Render_Kernel::f_renderSample(const dvec2& uv) {
	const Ray ray = f_cameraRay(uv);
	const Hit hit_data = f_sceneIntersection(ray);
	if (hit_data.hit)
		return f_radiance(hit_data);
	return dvec3(0.0);
}

dvec3 Render_Kernel::f_radiance(const Hit& hit_data) {
	const dvec3 color = 
		hit_data.normal_a * hit_data.uvw.x +
		hit_data.normal_b * hit_data.uvw.y +
		hit_data.normal_c * hit_data.uvw.z;
	return color;
}

Hit Render_Kernel::f_sceneIntersection(const Ray& ray) {
	Hit hit_data = Hit();
	dvec1 t_dist = MAX_DIST;

	//for (CLASS::Object* object : file->objects) {
	//	if (static_cast<CLASS::OBJECT::Data*>(object->object_store->getObjectStore()->data->getData(file->active_scene->ptr, object))->type == CLASS::OBJECT::DATA::Type::MESH) {
	//		const CLASS::OBJECT::DATA::Mesh* mesh = static_cast<CLASS::OBJECT::DATA::Mesh*>(static_cast<CLASS::OBJECT::Data*>(object->object_store->getObjectStore()->data->getData(file->active_scene->ptr, object))->data);
	//		for (const CLASS::OBJECT::DATA::MESH::Face* face : mesh->faces) {
	//			const dvec4 vert4_a = object->transform_matrix * dvec4(face->vertices[0]->position, 1.0);
	//			const dvec4 vert4_b = object->transform_matrix * dvec4(face->vertices[1]->position, 1.0);
	//			const dvec4 vert4_c = object->transform_matrix * dvec4(face->vertices[2]->position, 1.0);
	//			const dvec3 vert_a = dvec3(vert4_a.x, vert4_a.y, vert4_a.z) / vert4_a.w;
	//			const dvec3 vert_b = dvec3(vert4_b.x, vert4_b.y, vert4_b.z) / vert4_b.w;
	//			const dvec3 vert_c = dvec3(vert4_c.x, vert4_c.y, vert4_c.z) / vert4_c.w;
	//
	//			if (f_rayTriangleIntersection(
	//				ray,
	//				vert_a,
	//				vert_b,
	//				vert_c,
	//				t_dist
	//			)) {
	//				if (t_dist < hit_data.t_dist && t_dist > EPSILON) {
	//					hit_data.t_dist = t_dist;
	//					hit_data.pos = ray.origin + ray.direction * t_dist;
	//					hit_data.hit = true;
	//					hit_data.inside = false;
	//
	//					hit_data.uvw = f_barycentricCoords(
	//						hit_data.pos,
	//						vert_a,
	//						vert_b,
	//						vert_c
	//					);
	//				}
	//			}
	//		}
	//	}
	//}
	return hit_data;
}

Ray Render_Kernel::f_cameraRay(const dvec2& uv) {
	//const CLASS::OBJECT::DATA::Camera* camera = static_cast<CLASS::OBJECT::DATA::Camera*>(static_cast<CLASS::OBJECT::Data*>(file->default_camera->object_store->getObjectStore()->data->getData(file->active_scene->ptr, file->default_camera))->data);
	return Ray(
		//static_cast<CLASS::Transform*>(file->default_camera->object_store->getObjectStore()->transform->getData(file->active_scene->ptr, file->default_camera))->position,
		//normalize(
		//	  camera->projection_center
		//	+ camera->projection_u * uv.x
		//	+ camera->projection_v * uv.y
		//	- static_cast<CLASS::Transform*>(file->default_camera->object_store->getObjectStore()->transform->getData(file->active_scene->ptr, file->default_camera))->position
		//)
	);
}

bool Render_Kernel::f_rayTriangleIntersection(const Ray& ray, const dvec3& vertex_a, const dvec3& vertex_b, const dvec3& vertex_c, dvec1& t_dist) {
	const dvec3 v1v0 = vertex_b - vertex_a;
	const dvec3 v2v0 = vertex_c - vertex_a;
	const dvec3 rov0 = ray.origin - vertex_a;

	const dvec3  normal = cross(v1v0, v2v0);
	const dvec3  q = cross(rov0, ray.direction);
	const dvec1 d = 1.0 / dot(ray.direction, normal);
	const dvec1 u = d * dot(-q, v2v0);
	const dvec1 v = d * dot(q, v1v0);
	const dvec1 t = d * dot(-normal, rov0);

	if (u < 0.0 || v < 0.0 || (u + v) > 1.0)
		return false;

	t_dist = t;
	return true;
}

dvec3 Render_Kernel::f_barycentricCoords(const dvec3& hit_pos, const dvec3& vertex_a, const dvec3& vertex_b, const dvec3& vertex_c) {
	const dvec3 v0 = vertex_b - vertex_a;
	const dvec3 v1 = vertex_c - vertex_a;
	const dvec3 v2 = hit_pos - vertex_a;

	const dvec1 d00 = dot(v0, v0);
	const dvec1 d01 = dot(v0, v1);
	const dvec1 d11 = dot(v1, v1);
	const dvec1 d20 = dot(v2, v0);
	const dvec1 d21 = dot(v2, v1);

	const dvec1 denom = d00 * d11 - d01 * d01;

	const dvec1 v = (d11 * d20 - d01 * d21) / denom;
	const dvec1 w = (d00 * d21 - d01 * d20) / denom;
	const dvec1 u = 1.0 - v - w;

	return dvec3(u, v, w);
}