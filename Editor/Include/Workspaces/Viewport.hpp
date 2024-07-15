#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Ops.hpp"
#include "Core/File.hpp"
#include "Core/Session.hpp"

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

		struct Workspace_Viewport : GUI::Linear_Contents {
			Workspace_Manager* parent;

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
	}
}