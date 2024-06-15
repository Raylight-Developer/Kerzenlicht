#include "CPU_Renderer.hpp"

#include "Workspaces/Viewport.hpp"

CPU_Renderer::CPU_Renderer(GUI::WORKSPACE::Viewport_CPU_Renderer* viewport, Log_Console * log, CLASS::File* file) :
	QThread(),
	viewport      (viewport),
	kernel        (Render_Kernel(viewport, file)),
	render_pixmap (nullptr),
	resolution    (viewport->render_resolution),
	file          (file),
	log           (log)
{
	render_pixmap = new float[resolution.x * resolution.y * 4U];

	for (uint64 i = 0; i < resolution.x * resolution.y * 4U; i++)
		render_pixmap[i] = 0.0f;
}

void CPU_Renderer::run() {
	//static_cast<CLASS::OBJECT::DATA::Camera*>(static_cast<CLASS::OBJECT::Data*>(file->default_camera->object_store->getObjectStore()->data->getData(file->active_scene->ptr, file->default_camera))->data)->f_compile(file->active_scene->ptr, file->default_camera);
	//for (CLASS::Object* object : file->objects) {
	//	object->f_compileMatrix(file->active_scene->ptr);
	//}
	Lace log_msg;

	clock_t start_time = clock();
	clock_t delta_time = clock();
	log_msg << NL() << "CPU Rendering:";

	const uint32 step_count = 6U; // 4 = 16 | 6 = 64 | 8 = 256 | 10 = 1024 | 12 = 4096
	const uint32 step_stop = 2U;
	vector<uint32> steps;
	for (uint32 i = step_count; i > 0; i--) {
		steps.push_back(d_to_u(pow(2, i)));
	}
	for (uint i = 0; i <= steps.size() - step_stop; i++) {
		uint32 update = 0U;
		if (i == 0) {
			log_msg << NL() << "    Rendering Start Step: " << steps[0];
			const uint32 step_size = steps[0];
			const uint32 draw_size = steps[0];
			for (uint x = 0; x < resolution.x; x += step_size) {
				for (uint y = 0; y < resolution.y; y += step_size) {
					const dvec3 color = kernel.f_renderPixel(x, y);
					f_drawPixels(x, y, draw_size, color, 1.0);
				}
			}
			clock_t now_time = clock();
			log_msg << NL() << "        (" << ((double)(now_time - delta_time) / CLOCKS_PER_SEC) << ")s";
			delta_time = now_time;
			emit f_cpuUpdateRender(render_pixmap);
			msleep(d_to_u(pow(steps[0] + 16U, 0.55)));
		}
		else if (i == steps.size()) {
			log_msg << NL() << "    Rendering End Step: 1";
			for (uint x = 1U; x < resolution.x; x += 2U) {
				for (uint y = 0U; y < resolution.y; y += 2U) {
					const dvec3 color_a = kernel.f_renderPixel(x, y);
					f_drawPixel(x, y, color_a, 1.0);
					const dvec3 color_b = kernel.f_renderPixel(x, y + 1);
					f_drawPixel(x, y + 1, color_b, 1.0);
					const dvec3 color_c = kernel.f_renderPixel(x - 1, y + 1);
					f_drawPixel(x - 1, y + 1, color_c, 1.0);
				}
				if (update >= 32) {
					update = 0;
					emit f_cpuUpdateRender(render_pixmap);
					cout << "\r        " << (double)x / resolution.x * 100.0 << "%";
					msleep(4);
				}
				update++;
			}
			clock_t now_time = clock();
			log_msg << "\r        (" << ((double)(now_time - delta_time) / CLOCKS_PER_SEC) << ")s";
			delta_time = now_time;
		}
		else {
			log_msg << NL() << "    Rendering Step: " << steps[i];
			const uint32 step_size = steps[i];
			const uint32 draw_size = steps[i];
			const uint32 sleep = d_to_u(pow(steps[i] + 16U, 0.55));
			const uint32 update_step = d_to_u(pow(steps[i], 0.55));
			for (uint x = step_size; x < resolution.x; x += steps[i - 1]) {
				for (uint y = 0U; y < resolution.y; y += steps[i - 1]) {
					const dvec3 color_a = kernel.f_renderPixel(x, y);
					f_drawPixels(x, y, draw_size, color_a, 1.0);
					const dvec3 color_b = kernel.f_renderPixel(x, y + step_size);
					f_drawPixels(x, y + step_size, draw_size, color_b, 1.0);
					const dvec3 color_c = kernel.f_renderPixel(x - step_size, y + step_size);
					f_drawPixels(x - step_size, y + step_size, draw_size, color_c, 1.0);
				}
				if (update >= 32) {
					update = 0;
					emit f_cpuUpdateRender(render_pixmap);
					cout << "\r        " << (double)x / resolution.x * 100.0 << "%";
					msleep(sleep);
				}
				update += update_step;
			}
			clock_t now_time = clock();
			log_msg << "        (" << ((double)(now_time - delta_time) / CLOCKS_PER_SEC) << ")s";
			delta_time = now_time;
		}
	}
	emit f_cpuUpdateRender(render_pixmap);
	log_msg << NL() << "--------------------------RENDER FINISHED--------------------------";
	log_msg << NL() << "Total Render Time: (" << ((double)(clock() - start_time) / CLOCKS_PER_SEC) << ")s";
}

void CPU_Renderer::f_drawPixel(const uint32& x, const uint32& y, const dvec4& color) {
	if (x < resolution.x and y < resolution.y and x >= 0U and y >= 0U) {
		render_pixmap[(y * resolution.x + x) * 4U]      = static_cast<float>(color.r);
		render_pixmap[(y * resolution.x + x) * 4U + 1U] = static_cast<float>(color.g);
		render_pixmap[(y * resolution.x + x) * 4U + 2U] = static_cast<float>(color.b);
		render_pixmap[(y * resolution.x + x) * 4U + 3U] = static_cast<float>(color.a);
	}
}

void CPU_Renderer::f_drawPixel(const uint32& x, const uint32& y, const dvec3& color, const dvec1& alpha) {
	if (x < resolution.x and y < resolution.y and x >= 0U and y >= 0U) {
		render_pixmap[(y * resolution.x + x) * 4U]      = static_cast<float>(color.r);
		render_pixmap[(y * resolution.x + x) * 4U + 1U] = static_cast<float>(color.g);
		render_pixmap[(y * resolution.x + x) * 4U + 2U] = static_cast<float>(color.b);
		render_pixmap[(y * resolution.x + x) * 4U + 3U] = static_cast<float>(alpha);
	}
}

void CPU_Renderer::f_drawPixels(const uint32& x, const uint32& y, const uint32& size, const dvec3& color, const dvec1& alpha) {
	for (uint32 dx = 0U; dx < size; dx++) {
		for (uint32 dy = 0U; dy < size; dy++) {
			f_drawPixel(x + dx, y + dy, color, alpha);
		}
	}
}