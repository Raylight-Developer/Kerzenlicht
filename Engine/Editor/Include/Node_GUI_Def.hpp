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
			struct Tick : Node {
				Tick(const ivec2& pos);
			};
		}
		namespace LINK {
			struct Pointer : Node {
				Pointer(const ivec2& pos);
			};
		}
		namespace MATH {
			struct MATH : Node {
				MATH(const ivec2& pos);
				PORT::Data_I_Port* in_a;
				PORT::Data_I_Port* in_b;
				PORT::Data_O_Port* out_a;

				CLASS::NODE::DATA::Type data_type;

				void onDataTypeSet(const CLASS::NODE::DATA::Type& type) override;
				void onDataTypeUnset() override;
				void onPortConnect(Port* port, Connection* connection) override;
				void onPortDisconnect(Port* port) override;
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