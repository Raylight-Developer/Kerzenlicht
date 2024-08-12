#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "GUI_Nodes.hpp"

#include "Core/Editor_File.hpp"

#include "Nodes.hpp"
#include "Object/Nodes/Object_Nodes.hpp"
#include "Object/Nodes/Compiler.hpp"

// FWD DECL OTHER

// FWD DECL THIS
namespace GUI {
	namespace NODE {
		namespace EXEC {
			class Script_Node;
			struct Script;
		}
	}
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
			struct Counter : GUI::NODE::Node {
				PORT::Data_O_Port* o_count;
				PORT::Exec_I_Port* i_exec;
				PORT::Exec_O_Port* o_exec;

				Counter(const ivec2& pos);
			};
			struct Script : Node {
				QGraphicsProxyWidget* script_identifier_proxyWidget;
				QGraphicsProxyWidget* reload_proxyWidget;
				GUI::Value_Input* script_identifier;
				GUI::Button* reload;

				HINSTANCE dynlib;
				Script_Node* wrapper;

				Script(const ivec2& pos, const string& script_id = "");
				~Script();
				void clearIO();
				void addDataInput (const uint16& slot_id, const string& label, const KL::DATA::Type& type, const KL::DATA::Modifier& modifier = KL::DATA::Modifier::SINGLE);
				void addDataOutput(const uint16& slot_id, const string& label, const KL::DATA::Type& type, const KL::DATA::Modifier& modifier = KL::DATA::Modifier::SINGLE);
				void addExecInput (const uint16& slot_id, const string& label);
				void addExecOutput(const uint16& slot_id, const string& label);
				void renderDefault(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

				void (*buildGuiFunc)(Script_Node*);
				void (*paintGuiFunc)(Script_Node*, QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

				void reloadFunctions();
				void reloadDll();
				void recompile(const HINSTANCE& library);
				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
			};
			class Script_Node {
			private:
				Script* node;
			public:
				Script_Node(Script* node);
				virtual void addDataInput (const string& label, const KL::DATA::Type& type, const KL::DATA::Modifier& modifier = KL::DATA::Modifier::SINGLE) const;
				virtual void addDataOutput(const string& label, const KL::DATA::Type& type, const KL::DATA::Modifier& modifier = KL::DATA::Modifier::SINGLE) const;
				virtual void addExecInput (const string& label) const;
				virtual void addExecOutput(const string& label) const;
				virtual void clearIO() const;

				virtual void renderDefault(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) const;

				virtual bool isSelected() const;
				virtual QRectF rect() const;
				virtual QString label() const;
			};
			struct Tick : Node {
				Tick(const ivec2& pos);
			};
		}
		namespace LINK {
			struct Pointer : Node {
				PORT::Data_O_Port* o_pointer;

				KL::DATA::Type pointer_type;
				void* pointer;

				Pointer(const ivec2& pos, const KL::DATA::Type& pointer_type);

				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};
			struct Get : Node {
				PORT::Data_I_Port* i_pointer;
				PORT::Data_O_Port* o_value;

				KL::NODE::LINK::GET::Type mini_type;

				Get(const ivec2& pos);
			};
			struct Set : Node {
				PORT::Data_I_Port* i_pointer;
				PORT::Data_I_Port* i_value;
				PORT::Data_O_Port* o_value;
				PORT::Exec_I_Port* i_exec;
				PORT::Exec_O_Port* o_exec;

				KL::NODE::LINK::SET::Type mini_type;

				Set(const ivec2& pos);
			};
			namespace GET {
				struct Field : Get {
					Value_Input* field;

					Field(const ivec2& pos);
				};
			}
			namespace SET {
				struct Euler_Rotation_X : Set {
					Euler_Rotation_X(const ivec2& pos);
				};
			}
		}
		namespace MATH {
			struct Math : Node {
				PORT::Data_I_Port* in_a;
				PORT::Data_I_Port* in_b;
				PORT::Data_O_Port* out_a;

				Math(const ivec2& pos);
			};
			struct Add : Math {
				Add(const ivec2& pos);
			};
			struct Sub : Math {
				Sub(const ivec2& pos);
			};
			struct Mul : Math {
				Mul(const ivec2& pos);
			};
			struct Div : Math {
				Div(const ivec2& pos);
			};
		}
		namespace UTIL {
			struct Print : Node {
				PORT::Exec_I_Port* i_exec;
				PORT::Data_I_Port* i_value;
				PORT::Exec_O_Port* o_exec;

				Print(const ivec2& pos);
			};
			struct Cast : Node {
				KL::NODE::UTIL::CAST::Type mini_type;
				PORT::Data_I_Port* i_value;
				PORT::Data_O_Port* o_value;

				Cast(const ivec2& pos);
			};
			namespace CAST {
				struct Uint_To_Double : Cast {
					Uint_To_Double(const ivec2& pos);
				};
				struct Int_To_Double : Cast {
					Int_To_Double(const ivec2& pos);
				};
			}
		}
	}
}