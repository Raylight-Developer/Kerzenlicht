#pragma once

#include "Include.hpp"

#include "QT.hpp"

#include "Core/Nodes.hpp"
#include "Core/Compiler.hpp"

#include "Node_GUI.hpp"

// FWD DECL OTHER

// FWD DECL THIS
namespace GUI {
}

// DECL
namespace GUI {
	namespace NODE {
		namespace CONSTRAINT {
		}
		namespace GENERATE {
		}
		namespace PHYSICS {
		}
		namespace MODIFY {
		}
		namespace EXEC {
			struct Counter : Node {
				PORT::Data_O_Port* o_count;
				PORT::Exec_I_Port* i_exec;
				PORT::Exec_O_Port* o_exec;

				Counter(const ivec2& pos);
			};
			struct Script : Node {
				GUI::Value_Input* script_identifier;
				GUI::Button* reload;

				HINSTANCE dynlib;

				Script(const ivec2& pos);

				virtual void clearIO();
				virtual void addDataInput (const uint16& slot_id, const string& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier = CLASS::NODE::DATA::Modifier::SINGLE);
				virtual void addDataOutput(const uint16& slot_id, const string& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier = CLASS::NODE::DATA::Modifier::SINGLE);
				virtual void addExecInput (const uint16& slot_id, const string& label);
				virtual void addExecOutput(const uint16& slot_id, const string& label);

				void (*buildGuiFunc)(Script*);

				void reloadFunctions();
				void reloadDll();
			};
			struct Tick : Node {
				Tick(const ivec2& pos);
			};
		}
		namespace LINK {
			struct Pointer : Node {
				PORT::Data_O_Port* out_pointer;

				CLASS::NODE::DATA::Type pointer_type;
				void* pointer;

				Pointer(const ivec2& pos);

				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};
			struct Get : Node {
				Get(const ivec2& pos);
			};
			struct Set : Node {
				PORT::Data_I_Port* i_pointer;
				PORT::Data_I_Port* i_value;
				PORT::Data_O_Port* o_value;
				PORT::Exec_I_Port* i_exec;
				PORT::Exec_O_Port* o_exec;

				CLASS::NODE::LINK::SET::Type micro_type;

				Set(const ivec2& pos);
			};
			namespace GET {
			}
			namespace SET {
				struct Euler_Rotation_X : Set {
					Euler_Rotation_X(const ivec2& pos);
				};
			}
		}
		namespace MATH {
			struct MATH : Node {
				PORT::Data_I_Port* in_a;
				PORT::Data_I_Port* in_b;
				PORT::Data_O_Port* out_a;

				CLASS::NODE::DATA::Type data_type;

				MATH(const ivec2& pos);
			};
			struct Add : MATH {
				Add(const ivec2& pos);
			};
			struct Sub : MATH {
				Sub(const ivec2& pos);
			};
			struct Mul : MATH {
				Mul(const ivec2& pos);
			};
			struct Div : MATH {
				Div(const ivec2& pos);
			};
		}
		namespace UTIL {
		}
	}
}