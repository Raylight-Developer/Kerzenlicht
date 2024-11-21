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

#undef FILE
#undef SESSION

#define EXPORT extern "C" __declspec(dllexport)
#define BUILD_(name) EXPORT void name##_build
#define FETCH_(name) EXPORT Prop name##_getData
#define EXEC_(name)  EXPORT void name##_exec

#define F_BUILD_(name) EXPORT void name##_build(Script* NODE)
#define F_FETCH_(name) EXPORT Prop name##_getData(Script* NODE, const uint16& PORT)
#define F_EXEC_(name)  EXPORT void name##_exec(Script* NODE, File* FILE, Session* SESSION, Lace* LACE)