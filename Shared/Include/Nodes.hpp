#pragma once

#include "Include.hpp"

#include "Data_Property.hpp"

// FWD DECL OTHER
namespace KL {
	struct Scene;
	struct Object;
	namespace NODE {
		namespace EXEC {
			struct Tick;
		}
	}
}
namespace KL  {
	struct Prop;
	namespace PROP {
		enum struct Type;
		enum struct Modifier;
	}
}

// FWD DECL THIS
namespace KL {
	struct Node;
	struct Node_Tree;
	namespace NODE {
		enum struct Type;
		struct Port;
		namespace PORT {
			enum struct Type;
			struct Data_I_Port;
			struct Data_O_Port;
			struct Exec_I_Port;
			struct Exec_O_Port;
		}
	}
}

// DECL
namespace KL {
	struct Node_Tree {
		string name;

		vector<Node*> nodes;      // src

		vector<Node*> references; // ref
		vector<Node*> variables;  // ref
		NODE::EXEC::Tick* tick;   // ref

		Node_Tree();
		~Node_Tree();

		void exec(const dvec1* delta) const;

	};
	struct Node {
		string name;

		NODE::Type type;
		uint16 sub_type;

		vector<NODE::Port*> inputs;  // src
		vector<NODE::Port*> outputs; // src

		Node();
		~Node();

		virtual void exec(const uint16& slot_id) {}
		virtual Prop getData(const uint16& slot_id) const;
	};
	namespace NODE {
		enum struct Type {
			NONE,
			CONSTRAINT,
			GENERATE,
			PHYSICS,
			MODIFY,
			EXEC,
			MATH,
			LINK,
			UTIL
		};

		struct Port {
			Node* node; // ref
			uint16 slot_id;
			PORT::Type type;

			Port(Node* node);
			virtual ~Port() = default;

			virtual Prop getData() const;
			virtual void exec() const {}
		};
		namespace PORT {
			enum struct Type {
				NONE,
				DATA_I,
				DATA_O,
				EXEC_I,
				EXEC_O
			};
			// TODO (maybe) for faster use typename T and make variants of for example Math nodes for each type e.g. float, vec3, etc. and discard std::any use
			struct Data_I_Port : Port {
				Data_O_Port* connection; // ref
				Prop default_value;

				PROP::Type data_type;
				PROP::Modifier modifier;

				Data_I_Port(Node* parent, const uint16& slot_id, const PROP::Type& type, const PROP::Modifier& modifier = PROP::Modifier::SINGLE);
				~Data_I_Port();

				Prop getData() const override;
			};

			struct Data_O_Port : Port {
				vector<Data_I_Port*> outgoing_connections; // ref

				PROP::Type data_type;
				PROP::Modifier modifier;

				Data_O_Port(Node* parent, const uint16& slot_id, const PROP::Type& type, const PROP::Modifier& modifier = PROP::Modifier::SINGLE);
				~Data_O_Port();

				Prop getData() const override;
			};

			struct Exec_I_Port : Port {
				vector<Exec_O_Port*> incoming_connections; // ref

				Exec_I_Port(Node* parent, const uint16& slot_id);
				~Exec_I_Port();

				void exec() const override;
			};

			struct Exec_O_Port : Port {
				Exec_I_Port* connection; // ref

				Exec_O_Port(Node* parent, const uint16& slot_id);
				~Exec_O_Port();

				void exec() const override;
			};
		}
	}
}