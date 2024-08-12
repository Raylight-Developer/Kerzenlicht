#pragma once

#include "pch.h"

#define EXPORT __declspec(dllexport)

#include "Shared.hpp"

#include "Object/Data.hpp"

#include "Nodes.hpp"
#include "Object/Object.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

using Data      = KL::Data;
using Data_Type = KL::DATA::Type;
using Script    = KL::NODE::EXEC::Script_Node;