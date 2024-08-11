#pragma once

#include "pch.h"

#define EXPORT __declspec(dllexport)

#include "Shared.hpp"

#include "Nodes.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

#include "Core/Editor_File.hpp"
#include "GUI_Nodes.hpp"
#include "GUI_Object_Nodes.hpp"

#include "Object/Object.hpp"
#include "Object/Data.hpp"

using Data = CLASS::Data;
using Data_Type = CLASS::DATA::Type;
using Script = CLASS::NODE::EXEC::Script_Node;
using Gui_Script = GUI::NODE::EXEC::Script_Node;