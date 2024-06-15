#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Core/Lace.hpp"
#include "Core/File.hpp"

#include "Rendering/Render_Kernel.hpp"
#include "Log_Console.hpp"

namespace GUI {
	namespace WORKSPACE {
		class Viewport_CPU_Renderer;
	}
}

class CPU_Renderer : public QThread {
	Q_OBJECT
public:
	GUI::WORKSPACE::Viewport_CPU_Renderer* viewport;
	Log_Console* log;
	CLASS::File* file;

	Render_Kernel kernel;
	float* render_pixmap;

	const uvec2 resolution;

	CPU_Renderer(GUI::WORKSPACE::Viewport_CPU_Renderer* viewport, Log_Console* log, CLASS::File* file);

	void f_drawPixel(const uint32& x, const uint32& y, const dvec4& color);
	void f_drawPixel(const uint32& x, const uint32& y, const dvec3& color, const dvec1& alpha);
	void f_drawPixels(const uint32& x, const uint32& y, const uint32& size, const dvec3& color, const dvec1& alpha);

	void run() override;

signals:
	void f_cpuUpdateRender(const float* display_pixmap);
};