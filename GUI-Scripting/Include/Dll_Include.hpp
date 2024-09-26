#pragma once

#include "pch.h"


#include "Shared.hpp"

#include "Object/Data.hpp"

#include "GUI_Nodes.hpp"
#include "GUI_Object_Nodes.hpp"

using Prop_Type  = KL::PROP::Type;
using Gui_Script = GUI::NODE::EXEC::Script_Node;

#define EXPORT extern "C" __declspec(dllexport)
#define BUILD_(name) extern "C" __declspec(dllexport) void name##_buildGui
#define RENDER_(name) extern "C" __declspec(dllexport) void name##_renderGui

#define F_BUILD_(name) extern "C" __declspec(dllexport) void name##_buildGui(Gui_Script* node)
#define F_RENDER_(name) extern "C" __declspec(dllexport) void name##_renderGui(Gui_Script* node, QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)