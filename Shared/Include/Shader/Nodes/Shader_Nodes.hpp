#pragma once

#include "Include.hpp"
#include "Data_Property.hpp"

#include "Nodes.hpp"


// FWD DECL OTHER

// FWD DECL THIS
namespace CLASS {
	namespace NODE {
		enum struct Type;
		namespace CONSTRAINT {
			enum struct Type;
		}
		namespace GENERATE {
			enum struct Type;
		}
		namespace PHYSICS {
			enum struct Type;
		}
		namespace MODIFY {
			enum struct Type;
		}
		namespace EXEC {
			enum struct Type;
		}
		namespace MATH {
			enum struct Type;
		}
		namespace LINK {
			enum struct Type;
		}
		namespace UTIL {
			enum struct Type;
		}
	}
}

using Data = CLASS::Data;
namespace DATA = CLASS::DATA;

// DECL
namespace SHADER {
	namespace NODE {
		namespace EXEC {
			enum struct Type { TICK };
			struct Tick : CLASS::Node {
				CLASS::NODE::PORT::Exec_O_Port* port_tick;
				CLASS::NODE::PORT::Data_O_Port* port_delta;

				const dvec1* delta;

				Tick();

				void exec(const uint16& slot_id = 0) override;
				Data getData(const uint16& slot_id) const override;
			};
		}
	}
}