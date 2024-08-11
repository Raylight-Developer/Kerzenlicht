#pragma once

#include "Include.hpp"

#include "Data_Property.hpp"
#include "Session.hpp"
#include "Scene.hpp"
#include "Lace.hpp"
#include "File.hpp"
#include "Ops.hpp"

#undef LOG
#undef FILE

#define LOG *Session::getInstance().getLog()
#define FILE Session::getInstance().getFile()
#define FLUSH Session::getInstance().flushLog()
#define COUT_S Lace lace; lace <<
#define COUT_E ; cout << lace.str()