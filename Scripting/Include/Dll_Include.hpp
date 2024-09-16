#pragma once

#include "pch.h"

#define EXPORT __declspec(dllexport)

#include "Shared.hpp"

#include "Object/Data.hpp"

#include "Nodes.hpp"
#include "Object/Object.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

using Prop      = KL::Prop;
using Prop_Type = KL::PROP::Type;
using Script    = KL::NODE::EXEC::Script_Node;