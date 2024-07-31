#pragma once

#include "pch.h"

#define EXPORT __declspec(dllexport)

#include "Shared.hpp"

#include "Node/Nodes.hpp"
#include "Node/Node_Def.hpp"

#include "Node_GUI.hpp"
#include "Node_GUI_Def.hpp"

#include "Core/File.hpp"
#include "Object/Object.hpp"
#include "Object/Data.hpp"

using Data = CLASS::Data;
using Data_Type = CLASS::DATA::Type;
using Script = CLASS::NODE::EXEC::Script_Node;
using Gui_Script = GUI::NODE::EXEC::Script_Node;