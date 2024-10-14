#pragma once

#include "pch.h"

#include "Shared.hpp"

#include "Object/Data.hpp"

#include "Object/Object.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

using Prop      = KL::Prop;
using Prop_Type = KL::PROP::Type;
using Prop_Mod  = CORE::PROP::Modifier;
using Script    = KL::NODE::EXEC::Script_Node;
using File      = KL::File;
using Session   = KL::Session;

#define EXPORT extern "C" __declspec(dllexport)
#define BUILD_(name) extern "C" __declspec(dllexport) void name##_build
#define FETCH_(name) extern "C" __declspec(dllexport) Prop name##_getData
#define EXEC_(name) extern "C" __declspec(dllexport) void name##_exec

#define F_BUILD_(name) extern "C" __declspec(dllexport) void name##_build(Script* node)
#define F_FETCH_(name) extern "C" __declspec(dllexport) Prop name##_getData(Script* node, const uint16& port_request)
#define F_EXEC_(name) extern "C" __declspec(dllexport) void name##_exec(Script* node, File* file, Session* session, Lace* lace)