#pragma once

#include <unordered_map>
#include <unordered_set>
#include <type_traits>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <optional>
#include <cstdlib>
#include <cstddef>
#include <iomanip>
#include <fstream>
#include <numeric>
#include <sstream>
#include <variant>
#include <cerrno>
#include <chrono>
#include <future>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <array>
#include <regex>
#include <tuple>
#include <any>
#include <map>
#include <set>

#include <windows.h>
#include <direct.h>
#include <math.h>

#if defined(COMPILE_EDITOR) || defined(COMPILE_GUI_SCRIPTING)
	#include <QtOpenGL/qopenglfunctions_4_5_core.h>
	#include <QtOpenGLWidgets>
	#include <QtWidgets>
	#include <QtCore>
	#include <QtGui>
#endif

#if defined(COMPILE_RENDERER)
	#include <glad/glad.h>
#endif

#include "Utils/Inline/Glm.hpp"
#include "Utils/Inline/Types.hpp"
#include "Utils/Inline/String.hpp"
#include "Utils/Inline/Macros.hpp"
#include "Utils/Inline/Templates.hpp"

using namespace std;