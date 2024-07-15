#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Ops.hpp"
#include "Core/File.hpp"
#include "Core/Session.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

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


		struct Viewport_Realtime : QObject {
			Workspace_Viewport* parent;
			GLFWwindow* window;

			dvec1 display_aspect_ratio;
			dvec1 render_aspect_ratio;
			uvec2 display_resolution;
			uvec2 render_resolution;

			uint   frame_counter;
			uint   frame_count;
			uint64 runframe;

			dvec1 current_time;
			dvec1 window_time;
			dvec1 frame_time;
			dvec1 last_time;

			vector<VIEWPORT_REALTIME::GPU_Triangle> triangles;
			map<CLASS::Object*, vector<VIEWPORT_REALTIME::Triangle>> triangle_map;

			Viewport_Realtime(Workspace_Viewport* parent = nullptr);

			void init();
			void exit();

			void initGlfw();

			void pipeline();
			void renderTick();
			void dataTransfer();

			void selectObject(const dvec2& uv);

			void displayLoop();

			static void framebufferSize(GLFWwindow* window, int width, int height);
		};
	}
}