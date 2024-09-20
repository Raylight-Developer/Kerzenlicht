#pragma once

#include "pch.h"

#define EXPORT extern "C" __declspec(dllexport)

#include "Shared.hpp"

#include "Object/Data.hpp"

#include "GUI_Nodes.hpp"
#include "GUI_Object_Nodes.hpp"

using Data_Type  = KL::PROP::Type;
using Gui_Script = GUI::NODE::EXEC::Script_Node;