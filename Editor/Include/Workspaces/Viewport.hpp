#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/Editor_File.hpp"
#include "Gpu_Scene.hpp"

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

			KL::GPU_Scene* gpu_data;

			dvec1 render_scale;
			dvec1 display_aspect_ratio;
			dvec1 render_aspect_ratio;
			uvec2 display_resolution;
			uvec2 render_resolution;

			unordered_map<KL::Object*, vector<VIEWPORT_REALTIME::Triangle>> triangle_map;
			vector<VIEWPORT_REALTIME::GPU_Triangle> triangles;
			vector<vec1> gl_triangles;
			unordered_map<string, GLuint> gl_data;

			uint   frame_counter;
			uint   frame_count;
			uint64 runframe;

			bool recompile;
			bool reset;
			bool debug;

			chrono::high_resolution_clock::time_point current_time;
			chrono::high_resolution_clock::time_point start_time;
			chrono::high_resolution_clock::time_point last_time;
			dvec1 window_time;
			dvec1 frame_time;

			uint view_layer;

			Viewport(Workspace_Viewport* parent);

			void f_pipeline();
			void f_uploadData();
			void f_tickUpdate();

			void f_selectObject(const dvec2& uv);

			void initializeGL() override;
			void paintGL() override;
			void resizeGL(int w, int h) override;

			GLuint renderLayer(const uvec2& resolution);
			GLuint fragmentShaderProgram(const string& file_path);
			GLuint computeShaderProgram(const string& file_path);
			void bindRenderLayer(const GLuint& program_id, const GLuint& unit, const GLuint& id, const string& name);
			void checkShaderCompilation(const GLuint& shader, const string& shader_code);
			void checkProgramLinking(const GLuint& program);
			void printShaderErrorWithContext(const string& shaderSource, const string& errorLog);

			template <typename T>
			GLuint ssboBinding(const GLuint& binding, const GLuint& size, const T& data) {
				GLuint buffer;
				glGenBuffers(1, &buffer);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
				glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
				return buffer;
			}
		};
	}
}