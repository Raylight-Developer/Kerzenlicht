#pragma once

#include "Include.hpp"

#include "QT.hpp"
#include "Nodes.hpp"
#include "Compiler.hpp"

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

// DECL
namespace CLASS {
	namespace NODE {
		namespace CONSTRAINT {
			enum struct Type { PARENT };
		}
		namespace GENERATE {
			enum struct Type { SUBDIVISION, TRIANGULATE, WIREFRAME, SOLIDIFY, BOOLEAN, MIRROR, ARRAY, BEVEL };
		}
		namespace PHYSICS {
			enum struct Type { COLLISION, CACHE, CLOTH, FLUID, RIGID, SOFT, GAS };
		}
		namespace MODIFY {
			enum struct Type { SURFACE_DEFORM, KEYFRAMING, SHRINKWRAP, ARMATURE, SMOOTH };
		}
		namespace EXEC {
			enum struct Type { SEQUENCE, COUNTER, SCRIPT, START, TIMER, TICK, FOR };
			struct Sequence : Node {
				uint8 count;

				Sequence();

				void exec(const string& slot_id) override;
			};
			struct Counter : Node {
				Counter();

				uint64 count;

				void exec(const string& slot_id) override;
			};
			struct Script : private Node {
				Script();
				map<string, Data> internal_data;

				unordered_map<string, PORT::Data_I_Port*> data_inputs;
				unordered_map<string, PORT::Data_O_Port*> data_outputs;
				unordered_map<string, PORT::Exec_I_Port*> exec_inputs;
				unordered_map<string, PORT::Exec_O_Port*> exec_outputs;

				// VIRTUAL required for functions used/called from DLL
				virtual Data getInputData (const string& slot_id) const;
				virtual void addDataInput (const string& slot_id, const DATA::Type& type, const DATA::Modifier& modifier = DATA::Modifier::SINGLE);
				virtual void addDataOutput(const string& slot_id, const DATA::Type& type, const DATA::Modifier& modifier = DATA::Modifier::SINGLE);
				virtual void addExecInput (const string& slot_id);
				virtual void addExecOutput(const string& slot_id);
				virtual void clearIO();

			private:
				string script_id;
				HINSTANCE dynlib;
				PORT::Exec_I_Port* in;
				PORT::Exec_O_Port* out;

				Data (*getDataFunc)(const Script*, const string&);
				void (*buildFunc)(Script*);
				void (*execFunc)(Script*);

				void exec(const string& slot_id) override;
				Data getData(const string& slot_id) const override;

				void reloadFunctions();
				void reloadDll();
			};
			struct Timer : Node {
				QTimer timer;
				PORT::Exec_O_Port* port;

				Timer();

				void exec(const string& slot_id) override;
			};
			struct Tick : Node {
				PORT::Exec_O_Port* port_tick;
				PORT::Data_O_Port* port_delta;
				const dvec1* delta;

				Tick();

				void exec(const string& slot_id = "") override;
				Data getData(const string& slot_id) const override;
			};
		}
		namespace MATH {
			enum struct Type { RAMP, MAP, ADD, DIV, MUL, SUB };
			struct MATH : Node {
				PORT::Data_I_Port* in_a;
				PORT::Data_I_Port* in_b;
				PORT::Data_O_Port* out_a;

				MATH();
			};
			struct Add : MATH {
				Add() {};
				NODE::Data getData(const string& slot_id) const override;
			};
			struct Sub : MATH {
				Sub() {};
				NODE::Data getData(const string& slot_id) const override;
			};
			struct Mul : MATH {
				Mul() {};
				NODE::Data getData(const string& slot_id) const override;
			};
			struct Div : MATH {
				Div() {};
				NODE::Data getData(const string& slot_id) const override;
			};
		}
		namespace LINK {
			enum struct Type { VARIABLE, POINTER, GET, SET };
			struct Pointer : Node {
				PORT::Data_O_Port* port;
				DATA::Type pointer_type;
				void* pointer;

				Pointer();

				NODE::Data getData(const string& slot_id) const override;
			};
			struct Get : Node {
				PORT::Data_I_Port* port_pointer;
				PORT::Data_O_Port* port_output;
				string field;

				Get();

				NODE::Data getData(const string& slot_id) const override;
			};
			struct Set : Node {
				PORT::Data_I_Port* port_pointer;
				PORT::Data_I_Port* port_value;
				PORT::Exec_I_Port* port_exec_i;
				PORT::Exec_O_Port* port_exec_o;
				string field;

				Set();

				void exec(const string& slot_id = "") override;
				NODE::Data getData(const string& slot_id) const override;
			};
		}
		namespace UTIL {
			enum struct Type { SWITCH, BREAK, MAKE, VIEW };
			struct Break : Node {
				PORT::Data_I_Port* port;

				Break();

				NODE::Data getData(const string& slot_id) const override;
			};
			struct Make : Node {
				PORT::Data_O_Port* port;

				Make();

				NODE::Data getData(const string& slot_id) const override;
			};
			struct View : Node {
				PORT::Data_I_Port* port;

				View();

				NODE::Data getData(const string& slot_id) const override;
			};
		}
	}
}