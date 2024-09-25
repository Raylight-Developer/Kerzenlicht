#pragma once

#include "Types.hpp"

#undef min
#undef max

#undef LOG
#undef FILE

#define MIN_VEC1  numeric_limits<vec1 >::min()
#define MAX_VEC1  numeric_limits<vec1 >::max()
#define MIN_DVEC1 numeric_limits<dvec1>::min()
#define MAX_DVEC1 numeric_limits<dvec1>::max()

#define MIN_INT8  numeric_limits<int8 >::min()
#define MAX_INT8  numeric_limits<int8 >::max()
#define MIN_INT16 numeric_limits<int16>::min()
#define MAX_INT16 numeric_limits<int16>::max()
#define MIN_INT32 numeric_limits<int32>::min()
#define MAX_INT32 numeric_limits<int32>::max()
#define MIN_INT64 numeric_limits<int64>::min()
#define MAX_INT64 numeric_limits<int64>::max()

#define MAX_UINT8  numeric_limits<uint8 >::max()
#define MAX_UINT16 numeric_limits<uint16>::max()
#define MAX_UINT32 numeric_limits<uint32>::max()
#define MAX_UINT64 numeric_limits<uint64>::max()

#define PI          3.141592653589793
#define TWO_PI      6.283185307179586
#define INVERTED_PI 0.318309886183791
#define DEG_RAD     0.017453292519943
#define RAD_DEG     57.29577951308232
#define EULER       2.718281828459045
#define PHI         1.618033988749895
#define FPS_60      0.016666666666667
#define MAX_DIST    10000.0
#define EPSILON     0.00001

#define ENDL << KL::Lace_NL()

#define ANSI_RESET  << "\033[0m"

#define ANSI_BLACK  << "\033[90m"
#define ANSI_R      << "\033[91m"
#define ANSI_G      << "\033[92m"
#define ANSI_YELLOW << "\033[93m"
#define ANSI_B      << "\033[94m"
#define ANSI_PURPLE << "\033[95m"
#define ANSI_CYAN   << "\033[96m"
#define ANSI_WHITE  << "\033[97m"

#define HTML_RESET  << "</span>"
#define HTML_ENDL  << "<br>"

#define HTML_R      << "<span style='color: rgb(255,100,100);'>"
#define HTML_G      << "<span style='color: rgb(100,255,100);'>"
#define HTML_B      << "<span style='color: rgb(100,100,255);'>"

#define NL  << KL::Lace_NL()
#define SP  << KL::Lace_S()
#define TAB << KL::Lace_TAB()
#define PTR << "* "

#define CMD(command) KL::Session::getInstance().getHistory()->execute(command);
#define UNDO(count)  KL::Session::getInstance().getHistory()->undo(count);
#define REDO(count)  KL::Session::getInstance().getHistory()->redo(count);

#define NEW_UID KL::Session::getInstance().getUID()++

#define RAM KL::Session::getInstance().ram
#define RAM_A(type, count) KL::Session::getInstance().ram += (sizeof(type) * count)
#define RAM_R(type, count) KL::Session::getInstance().ram -= (sizeof(type) * count)

#define VRAM KL::Session::getInstance().vram
#define VRAM_A(type, count) KL::Session::getInstance().vram += (sizeof(type) * count)
#define VRAM_R(type, count) KL::Session::getInstance().vram -= (sizeof(type) * count)

#define LOG *KL::Session::getInstance().getLog()
#define FLUSH KL::Session::getInstance().flushLog()
#define COUT_S KL::Lace lace; lace <<
#define COUT_E ; cout << lace.str()

#if defined(COMPILE_EDITOR) || defined(COMPILE_GUI_SCRIPTING)
	#define FILE static_cast<KL::Editor_File*>(KL::Session::getInstance().getFile())
#elif defined(COMPILE_RENDERER)
	#define FILE static_cast<KL::Render_File*>(KL::Session::getInstance().getFile())
#else
	#define FILE KL::Session::getInstance().getFile()
#endif