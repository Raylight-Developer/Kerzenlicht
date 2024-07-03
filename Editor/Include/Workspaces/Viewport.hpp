#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Ops.hpp"
#include "Core/File.hpp"
#include "Core/Session.hpp"

#include "CPU_Renderer.hpp"
#include "Viewport_Realtime.hpp"

namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;

		struct Workspace_Viewport;
		struct Timeline : GUI::Linear_Contents {
			Workspace_Viewport* parent;

			GUI::Label* info;
			GUI::Slider* slider;

			Timeline(Workspace_Viewport* parent);
		};

		struct Viewport_Realtime;
		class  Viewport_CPU_Renderer;
		struct Viewport_GPU_Renderer;

		enum struct Type {
			REALTIME,
			CPU,
			GPU
		};

		struct Workspace_Viewport : GUI::Linear_Contents {
			Workspace_Manager* parent;

			Viewport_CPU_Renderer* viewport_cpu_renderer;
			Viewport_GPU_Renderer* viewport_gpu_renderer;
			Viewport_Realtime* viewport_realtime;
			Timeline* timeline;

			QWidget* container;

			void f_systemInfo();

			Workspace_Viewport(Workspace_Manager* parent);
			~Workspace_Viewport();
		};

		struct alignas(16) GPU_Triangle {
			vec4 v0, v1, v2;

			GPU_Triangle(
				const vec3& v0, const vec3& v1, const vec3& v2
			) :
				v0(vec4(v0, 1.0)), v1(vec4(v1, 1.0)), v2(vec4(v2, 1.0))
			{}
		};

		class Viewport_CPU_Renderer : public QOpenGLWindow, protected QOpenGLFunctions_4_5_Core {
			Q_OBJECT
		public:
			Workspace_Viewport* parent;

			uvec2 display_resolution;
			dvec1 display_aspect_ratio;

			uvec2 render_resolution;
			dvec1 render_aspect_ratio;

			GLuint display_shader_program;
			GLuint cpu_render_result;

			GLuint fullscreen_quad_VAO;
			GLuint fullscreen_quad_VBO;
			GLuint fullscreen_quad_EBO;

			CPU_Renderer* cpu_renderer;

			Viewport_CPU_Renderer(Workspace_Viewport* parent);

			void f_pipeline();
			void f_cpuDisplayRender();

			void initializeGL() override;
			void paintGL() override;
			void resizeGL(int w, int h) override;

		public slots:
			void f_cpuUpdateRender(const float* display_pixmap);
		};

		struct Viewport_GPU_Renderer : QOpenGLWindow, protected QOpenGLFunctions_4_5_Core {
			Workspace_Viewport* parent;

			dvec1 downsampling;
			uvec2 display_resolution;
			dvec1 display_aspect_ratio;
			uvec2 render_resolution;
			dvec1 render_aspect_ratio;

			uvec3 compute_layout;
			vector<GPU_Triangle> triangles;

			GLuint compute_shader_program;
			GLuint display_shader_program;

			GLuint compute_render;

			GLuint fullscreen_quad_VAO;
			GLuint fullscreen_quad_VBO;
			GLuint fullscreen_quad_EBO;

			Viewport_GPU_Renderer(Workspace_Viewport* parent);

			void f_pipeline();
			void f_uploadData();
			void f_updateFrame();
			void f_updateNodes();

			void initializeGL() override;
			void paintGL() override;
			void resizeGL(int w, int h) override;
		};
	}
}