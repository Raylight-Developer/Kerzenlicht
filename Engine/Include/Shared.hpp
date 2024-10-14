#pragma once

#if defined(COMPILE_EDITOR) || defined(COMPILE_GUI_SCRIPTING)
	#include <QtOpenGL/qopenglfunctions_4_5_core.h>
	#include <QtOpenGLWidgets>
#endif

#include "Core.hpp"

#include "Core/Property.hpp"
#include "Core/Nodes.hpp"
#include "Core/Scene.hpp"
#include "Core/File.hpp"

#include "Utils/Session.hpp"

#if defined(COMPILE_RENDERER)
	#include <glad/glad.h>
#endif