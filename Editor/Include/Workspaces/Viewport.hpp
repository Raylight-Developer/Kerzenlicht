#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/Editor_File.hpp"

// FWD DECL OTHER
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;
	}
}

// FWD DECL THIS
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Viewport;
		struct Viewport;
	}
}

// DECL
namespace GUI {
	namespace WORKSPACE {
		struct Timeline : GUI::Linear_Contents {
			Workspace_Viewport* parent;

			GUI::Label* info;
			GUI::Slider* slider;

			Timeline(Workspace_Viewport* parent);
		};

		struct Workspace_Viewport : GUI::Linear_Contents {
			Workspace_Manager* parent;

			Viewport* viewport;
			Timeline* timeline;

			void f_systemInfo();

			Workspace_Viewport(Workspace_Manager* parent);
			~Workspace_Viewport();
		};

		namespace VIEWPORT_REALTIME {
			struct Ray {
				dvec3 origin, direction;
				Ray(const dvec3& origin, const dvec3& direction) : origin(origin), direction(direction) {};
			};
			struct Triangle {
				dvec3 v0, v1, v2;
				Triangle(const dvec3& v0, const dvec3& v1, const dvec3& v2) : v0(v0), v1(v1), v2(v2) {};
			};
			struct alignas(16) GPU_Triangle {
				vec4 v0, v1, v2;
				GPU_Triangle(const vec3& v0, const vec3& v1, const vec3& v2) : v0(vec4(v0, 1.0f)), v1(vec4(v1, 1.0f)), v2(vec4(v2, 1.0f)) {};
			};

			bool f_rayTriangleIntersection(const Ray& ray, const Triangle& tri, dvec1& ray_length);
		}

		struct Viewport : QOpenGLWindow, protected QOpenGLFunctions_4_5_Core {
			Workspace_Viewport* parent;

			uvec2 resolution;
			dvec1 aspect_ratio;
			dvec1 resolution_scale;
			uvec2 render_resolution;

			uvec3 compute_layout;
			vector<VIEWPORT_REALTIME::GPU_Triangle> triangles;
			unordered_map<CLASS::Object*, vector<VIEWPORT_REALTIME::Triangle>> triangle_map;

			GLuint compute_shader_program;
			GLuint display_shader_program;

			GLuint compute_render;

			GLuint fullscreen_quad_VAO;
			GLuint fullscreen_quad_VBO;
			GLuint fullscreen_quad_EBO;

			uint8 fps_counter;
			chrono::steady_clock::time_point fps_measure;

			uint64 frame_counter;
			chrono::steady_clock::time_point last_delta;
			dvec1 delta;

			Viewport(Workspace_Viewport* parent);

			void f_pipeline();
			void f_uploadData();
			void f_updateTick();

			void f_selectObject(const dvec2& uv);

			void initializeGL() override;
			void paintGL() override;
			void resizeGL(int w, int h) override;
		};
	}
}