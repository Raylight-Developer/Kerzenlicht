#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Core/File.hpp"
#include "Core/Ops.hpp"

// FWD DECL OTHER
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Viewport;
	}
}

// FWD DECL THIS

// DECL
namespace GUI {
	namespace WORKSPACE {
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
				dvec4 v0, v1, v2;
				GPU_Triangle(const dvec3& v0, const dvec3& v1, const dvec3& v2) : v0(dvec4(v0, 1.0)), v1(dvec4(v1, 1.0)), v2(dvec4(v2, 1.0)) {};
			};

			bool f_rayTriangleIntersection(const Ray& ray, const Triangle& tri, dvec1& ray_length);
		}

		struct Viewport_Realtime : QOpenGLWindow, protected QOpenGLFunctions_4_5_Core {
			Workspace_Viewport* parent;
			Lace* log;
			CLASS::File* file;

			uvec2 resolution;
			dvec1 aspect_ratio;

			uvec3 compute_layout;
			vector<VIEWPORT_REALTIME::GPU_Triangle> triangles;
			map<CLASS::Object*, vector<VIEWPORT_REALTIME::Triangle>> triangle_map;

			GLuint compute_shader_program;
			GLuint display_shader_program;

			GLuint compute_render;

			GLuint fullscreen_quad_VAO;
			GLuint fullscreen_quad_VBO;
			GLuint fullscreen_quad_EBO;

			uint8 fps_counter;
			chrono::steady_clock::time_point fps_measure;

			chrono::steady_clock::time_point last_delta;
			dvec1 delta;

			Viewport_Realtime(Workspace_Viewport* parent, Lace* log, CLASS::File* file);

			void f_pipeline();
			void f_uploadData();
			void f_updateTick();
			void f_updateFrame();

			void f_selectObject(const dvec2& uv);

			void initializeGL() override;
			void paintGL() override;
			void resizeGL(int w, int h) override;
		};
	}
}