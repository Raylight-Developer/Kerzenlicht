#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/Editor_File.hpp"
#include "GPU/Gpu_Scene.hpp"

// FWD DECL OTHER
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;
	}
}
namespace KL {
	struct Renderer;
}

// FWD DECL THIS
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Viewport;
		struct Viewport;
		class Render_Thread;
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

		struct Viewport : QOpenGLWindow, protected QOpenGLFunctions_4_5_Core {
			Workspace_Viewport* parent;

			dvec1 aspect_ratio;
			uvec2 resolution;

			unordered_map<uint64, vector<vec1>> gl_triangle_cache;
			vector<vec1> gl_triangles;

			unordered_map<string, GLuint> data;
			unordered_map<KL::Object*, unordered_map<string, GLuint>> gl_data;

			uint   frame_counter;
			uint   frame_count;
			uint64 runframe;

			chrono::high_resolution_clock::time_point current_time;
			chrono::high_resolution_clock::time_point start_time;
			chrono::high_resolution_clock::time_point last_time;
			dvec1 window_time;
			dvec1 frame_time;

			uint view_layer;

			dvec1 camera_move_sensitivity;
			dvec1 camera_view_sensitivity;
			dvec1 camera_orbit_sensitivity;
			unordered_map<uint, bool> inputs;
			dvec2 current_mouse;
			dvec2 last_mouse;

			Viewport(Workspace_Viewport* parent);
			~Viewport(); // TODO Cleanup

			void f_pipeline();
			void f_recompile();

			void f_tickUpdate();

			void f_timings();
			void f_guiUpdate();
			void f_inputLoop();
			void f_displayLoop();
			void f_frameUpdate();

			void f_selectClosestObject(const dvec2& uv);

			void initializeGL() override;
			void paintGL() override;
			void resizeGL(int w, int h) override;

			void mouseReleaseEvent(QMouseEvent* event) override;
			void mousePressEvent(QMouseEvent* event) override;
			void mouseMoveEvent(QMouseEvent* event) override;
			void keyReleaseEvent(QKeyEvent* event) override;
			void keyPressEvent(QKeyEvent* event) override;
			void wheelEvent(QWheelEvent* event) override;

			void f_renderMesh(const GLuint& raster_program, KL::Object* object);
			void f_renderGroup(const GLuint& raster_program, KL::Object* object);
			void f_renderCurve(const GLuint& raster_program, KL::Object* object);

			GLuint renderLayer(const uvec2& resolution);
			KL::Confirm<GLuint> fragmentShaderProgram(const string& vert_file_path, const string& frag_file_path);
			KL::Confirm<GLuint> computeShaderProgram(const string& file_path);
			void bindRenderLayer(const GLuint& program_id, const GLuint& unit, const GLuint& id, const string& name);
			bool checkShaderCompilation(const GLuint& shader, const string& shader_code);
			bool checkProgramLinking(const GLuint& program);
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