#pragma once

#include "Include.hpp"
#include "Data_Property.hpp"

#include "Nodes.hpp"

// FWD DECL OTHER

// FWD DECL THIS
namespace KL {
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
			struct Script;
			class Script_Node;
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
			namespace CAST {
				enum struct Type;
			}
		}
	}
}

// DECL
namespace KL {
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
			enum struct Type { SEQUENCE, COUNTER, SCRIPT, START, TIMER, TICK, FOR, IF };
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
				string script_id;
				unordered_map<string, PORT::Data_I_Port*> data_inputs;
				unordered_map<string, PORT::Data_O_Port*> data_outputs;
				unordered_map<string, PORT::Exec_I_Port*> exec_inputs;
				unordered_map<string, PORT::Exec_O_Port*> exec_outputs;

				map<string, Prop> internal_data;

				Script(const string& script_id = "");
				~Script();

				// VIRTUAL required for functions used/called from DLL
				Prop getPortData (const string& map_name) const;
				void addDataInput (const uint16& slot_id, const string& map_name, const PROP::Type& type, const PROP::Modifier& modifier = PROP::Modifier::SINGLE);
				void addDataOutput(const uint16& slot_id, const string& map_name, const PROP::Type& type, const PROP::Modifier& modifier = PROP::Modifier::SINGLE);
				void addExecInput (const uint16& slot_id, const string& map_name);
				void addExecOutput(const uint16& slot_id, const string& map_name);
				void clearIO();

				void recompile(const HINSTANCE& library);

				HINSTANCE dynlib;
				Script_Node* wrapper;

				Prop (*getDataFunc)(const Script_Node*, const uint16&);
				void (*buildFunc)(Script_Node*);
				void (*execFunc)(Script_Node*);

				void exec(const uint16& slot_id = 0) override;
				Prop getData(const uint16& slot_id) const override;

				void reloadFunctions();
				void reloadDll();
			};
			class Script_Node {
			private:
				Script* node;
			public:
				Script_Node(Script* node);
				// Create a new port
				virtual void addDataInput (const string& map_name, const PROP::Type& type, const PROP::Modifier& modifier = PROP::Modifier::SINGLE) const;
				// Create a new port
				virtual void addDataOutput(const string& map_name, const PROP::Type& type, const PROP::Modifier& modifier = PROP::Modifier::SINGLE) const;
				// Create a new port
				virtual void addExecInput (const string& map_name) const;
				// Create a new port
				virtual void addExecOutput(const string& map_name) const;
				// Clear All Inputs and Outputs, destroying their connections.
				virtual void clearIO() const;

				// Get pointer to the specified Port
				virtual PORT::Data_I_Port* getDataInput (const string& map_name) const;
				// Get pointer to the specified Port
				virtual PORT::Data_O_Port* getDataOutput(const string& map_name) const;
				// Get pointer to the specified Port
				virtual PORT::Exec_I_Port* getExecInput (const string& map_name) const;
				// Get pointer to the specified Port
				virtual PORT::Exec_O_Port* getExecOutput(const string& map_name) const;
				// Fetch the data from a Port
				virtual Prop getData(const string& map_name) const;
			};
			struct Timer : Node {
				PORT::Exec_O_Port* port;

				Timer();

				void exec(const uint16& slot_id) override;
			};
			struct Tick : Node {
				PORT::Exec_O_Port* port_tick;
				PORT::Data_O_Port* port_delta;

				const dvec1* delta;

				Tick();

				void exec(const uint16& slot_id = 0) override;
				Prop getData(const uint16& slot_id) const override;
			};
		}
		namespace MATH {
			enum struct Type { NONE, RAMP, MAP, ADD, DIV, MUL, SUB };
			struct Math : Node {
				PORT::Data_I_Port* i_value_a;
				PORT::Data_I_Port* i_value_b;
				PORT::Data_O_Port* o_value_res;

				Math();
			};
			struct Add : Math {
				Add();
				Prop getData(const uint16& slot_id) const override;
			};
			struct Sub : Math {
				Sub();
				Prop getData(const uint16& slot_id) const override;
			};
			struct Mul : Math {
				Mul();
				Prop getData(const uint16& slot_id) const override;
			};
			struct Div : Math {
				Div();
				Prop getData(const uint16& slot_id) const override;
			};
		}
		namespace LINK {
			enum struct Type { VARIABLE, POINTER, SELF, GET, SET };
			struct Pointer : Node {
				PORT::Data_O_Port* o_pointer;

				PROP::Type pointer_type;
				void* pointer;

				Pointer();

				Prop getData(const uint16& slot_id) const override;
			};
			struct Get : Node {
				PORT::Data_I_Port* i_pointer;
				PORT::Data_O_Port* o_value;

				GET::Type mini_type;

				Get();
			};
			struct Set : Node {
				PORT::Data_I_Port* i_pointer;
				PORT::Data_I_Port* i_value;
				PORT::Data_O_Port* o_value;

				PORT::Exec_I_Port* i_exec;
				PORT::Exec_O_Port* o_exec;

				SET::Type mini_type;

				Set();
			};
			namespace GET {
				enum struct Type { NONE, EULER_ROTATION_X, EULER_ROTATION_Y, EULER_ROTATION_Z, FIELD };
				struct Field : Get {
					string field;
					Field(const string& field = "");

					Prop getData(const uint16& slot_id) const override;
				};
			}
			namespace SET {
				enum struct Type { NONE, EULER_ROTATION_X, EULER_ROTATION_Y, EULER_ROTATION_Z, FIELD };
				struct Euler_Rotation_X : Set {
					Euler_Rotation_X();

					void exec(const uint16& slot_id = 0) override;
				};
				struct Euler_Rotation_Y : Set {
					Euler_Rotation_Y();

					void exec(const uint16& slot_id = 0) override;
				};
				struct Euler_Rotation_Z : Set {
					Euler_Rotation_Z();

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
			enum struct Type { NONE, COLLAPSE, EXPAND, SWITCH, PRINT, CAST, VIEW, LOGS };
			struct Cast : Node {
				CAST::Type mini_type;
				PORT::Data_I_Port* i_value;
				PORT::Data_O_Port* o_value;

				Cast();
			};
			namespace CAST {
				enum struct Type { NONE, UINT_TO_DOUBLE, INT_TO_DOUBLE };
				struct Uint_To_Double : Cast {
					Uint_To_Double();
					Prop getData(const uint16& slot_id) const override;
				};
				struct Int_To_Double : Cast {
					Int_To_Double();
					Prop getData(const uint16& slot_id) const override;
				};
			}
			struct Expand : Node {
				PORT::Data_I_Port* port;

				Expand();

				Prop getData(const uint16& slot_id) const override;
			};
			namespace EXPAND {
				enum struct Type { };

			}
			struct Collapse : Node {
				PORT::Data_O_Port* port;

				Collapse();

				Prop getData(const uint16& slot_id) const override;
			};
			namespace COLLAPSE {
				enum struct Type { };

			}
			struct Print : Node {
				PORT::Exec_I_Port* i_exec;
				PORT::Data_I_Port* i_value;
				PORT::Exec_O_Port* o_exec;

				Print();

				void exec(const uint16& slot_id = 0) override;
			};
			struct View : Node {
				PORT::Data_I_Port* port;

				View();

				Prop getData(const uint16& slot_id) const override;
			};
			namespace VIEW {
				enum struct Type {  };

			}
		}
	}
}