#pragma once

#include "pch.h"

#define EXPORT __declspec(dllexport)

#include "Include.hpp"
#include "Core/Nodes.hpp"
#include "Core/Node_Def.hpp"

#include "Node_GUI.hpp"
#include "Node_GUI_Def.hpp"

using Data = CLASS::NODE::Data;
using Data_Type = CLASS::NODE::DATA::Type;
using Script = CLASS::NODE::EXEC::Script_Node;
using Gui_Script = GUI::NODE::EXEC::Script_Node;