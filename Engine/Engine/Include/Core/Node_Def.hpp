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
			namespace GET {
				enum struct Type;
			}
			namespace SET {
				enum struct Type;
			}
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

				void exec(const uint16& slot_id = 0) override;
			};
			struct Counter : Node {
				PORT::Data_O_Port* o_count;
				PORT::Exec_I_Port* i_exec;
				PORT::Exec_O_Port* o_exec;

				uint64 count;

				Counter();

				void exec(const uint16& slot_id = 0) override;
			};
			struct Script : Node {
				unordered_map<string, PORT::Data_I_Port*> data_inputs;
				unordered_map<string, PORT::Data_O_Port*> data_outputs;
				unordered_map<string, PORT::Exec_I_Port*> exec_inputs;
				unordered_map<string, PORT::Exec_O_Port*> exec_outputs;

				map<string, Data> internal_data;

				Script();

				// VIRTUAL required for functions used/called from DLL
				virtual Data getInputData (const string& map_name) const;
				virtual void addDataInput (const uint16& slot_id, const string& map_name, const DATA::Type& type, const DATA::Modifier& modifier = DATA::Modifier::SINGLE);
				virtual void addDataOutput(const uint16& slot_id, const string& map_name, const DATA::Type& type, const DATA::Modifier& modifier = DATA::Modifier::SINGLE);
				virtual void addExecInput (const uint16& slot_id, const string& map_name);
				virtual void addExecOutput(const uint16& slot_id, const string& map_name);
				virtual void clearIO();

			private:
				string script_id;
				HINSTANCE dynlib;

				Data (*getDataFunc)(const Script*, const uint16&);
				void (*buildFunc)(Script*);
				void (*execFunc)(Script*);

				void exec(const uint16& slot_id = 0) override;
				Data getData(const uint16& slot_id) const override;

				void reloadFunctions();
				void reloadDll();
			};
			struct Timer : Node {
				PORT::Exec_O_Port* port;

				QTimer timer;

				Timer();

				void exec(const uint16& slot_id) override;
			};
			struct Tick : Node {
				PORT::Exec_O_Port* port_tick;
				PORT::Data_O_Port* port_delta;

				const dvec1* delta;

				Tick();

				void exec(const uint16& slot_id = 0) override;
				Data getData(const uint16& slot_id) const override;
			};
		}
		namespace MATH {
			enum struct Type { RAMP, MAP, ADD, DIV, MUL, SUB };
			struct MATH : Node {
				PORT::Data_I_Port* i_value_a;
				PORT::Data_I_Port* i_value_b;
				PORT::Data_O_Port* o_value_res;

				MATH();
			};
			struct Add : MATH {
				Add() {};
				NODE::Data getData(const uint16& slot_id) const override;
			};
			struct Sub : MATH {
				Sub() {};
				NODE::Data getData(const uint16& slot_id) const override;
			};
			struct Mul : MATH {
				Mul() {};
				NODE::Data getData(const uint16& slot_id) const override;
			};
			struct Div : MATH {
				Div() {};
				NODE::Data getData(const uint16& slot_id) const override;
			};
		}
		namespace LINK {
			enum struct Type { VARIABLE, POINTER, GET, SET };
			struct Pointer : Node {
				PORT::Data_O_Port* o_pointer;

				DATA::Type pointer_type;
				void* pointer;

				Pointer();

				NODE::Data getData(const uint16& slot_id) const override;
			};
			struct Get : Node {
				PORT::Data_I_Port* i_pointer;
				PORT::Data_O_Port* o_value;

				GET::Type micro_type;

				Get();
			};
			struct Set : Node {
				PORT::Data_I_Port* i_pointer;
				PORT::Data_I_Port* i_value;
				PORT::Data_O_Port* o_value;

				PORT::Exec_I_Port* i_exec;
				PORT::Exec_O_Port* o_exec;

				SET::Type micro_type;

				Set();
			};
			namespace GET {
				enum struct Type { NONE, EULER_ROTATION_X, EULER_ROTATION_Y, EULER_ROTATION_Z, FIELD };
				struct Field : Get {
					string field;

					Field();

					NODE::Data getData(const uint16& slot_id) const override;
				};
			}
			namespace SET {
				enum struct Type { NONE, EULER_ROTATION_X, EULER_ROTATION_Y, EULER_ROTATION_Z, FIELD };
				struct Euler_Rotation_X : Set {
					Euler_Rotation_X();

					void exec(const uint16& slot_id = 0) override;
				};
				struct Field : Set {
					string field;

					Field();

					void exec(const uint16& slot_id = 0) override;
				};
			}
		}
		namespace UTIL {
			enum struct Type { CONVERT, SWITCH, BREAK, MAKE, VIEW };
			struct Convert : Node {
				PORT::Data_I_Port* i_value;
				PORT::Data_O_Port* o_value;

				Convert();
			};
			namespace CONVERT {
				enum struct Type { UINT_TO_DOUBLE };
				struct Uint_To_Double : Convert {
					Uint_To_Double();

					NODE::Data getData(const uint16& slot_id) const override;
				};
			}
			struct Break : Node {
				PORT::Data_I_Port* port;

				Break();

				NODE::Data getData(const uint16& slot_id) const override;
			};
			namespace BREAK {
				enum struct Type { };

			}
			struct Make : Node {
				PORT::Data_O_Port* port;

				Make();

				NODE::Data getData(const uint16& slot_id) const override;
			};
			namespace MAKE {
				enum struct Type { };

			}
			struct View : Node {
				PORT::Data_I_Port* port;

				View();

				NODE::Data getData(const uint16& slot_id) const override;
			};
			namespace VIEW {
				enum struct Type {  };

			}
		}
	}
}