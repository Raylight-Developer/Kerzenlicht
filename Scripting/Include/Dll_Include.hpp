#pragma once

#include "pch.h"

#define EXPORT __declspec(dllexport)

#include "Shared.hpp"

#include "Object/Data.hpp"

#include "Nodes.hpp"
#include "Object/Object.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

using Data = CLASS::Data;
using Data_Type = CLASS::DATA::Type;
using Script = CLASS::NODE::EXEC::Script_Node;