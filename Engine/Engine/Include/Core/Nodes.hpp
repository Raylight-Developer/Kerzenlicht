#pragma once

#include "Include.hpp"
#include "Ops.hpp"
#include "QT.hpp"

// FWD DECL OTHER
namespace GUI {
	namespace NODE {
		struct Node_Tree;
	}
}
namespace CLASS {
	namespace NODE {
		namespace EXEC {
			struct Tick;
		}
	}
}

// FWD DECL THIS
namespace CLASS {
	struct Node;
	struct Node_Tree;
	namespace NODE {
		enum struct Type;
		struct Data;
		namespace DATA {
			enum struct Type;
			enum struct Modifier;
		}
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
namespace CLASS {
	struct Node_Tree {
		vector<Node*> nodes;      // src
		vector<Node*> variables;  // ref
		vector<Node*> references; // ref
		NODE::EXEC::Tick* tick;   // ref

		Node_Tree() {};
		Node_Tree(const GUI::NODE::Node_Tree* gui_tree);
		~Node_Tree();

		void exec(const dvec1* delta) const;

	};
	struct Node {
		NODE::Type type;
		uint16 sub_type;

		vector<NODE::Port*> inputs;  // src
		vector<NODE::Port*> outputs; // src

		Node() {};
		~Node();

		virtual void exec(const string& slot_id) {}
		virtual NODE::Data getData(const string& slot_id) const;
	};
	namespace NODE {
		enum struct Type {
			CONSTRAINT,
			GENERATE,
			PHYSICS,
			MODIFY,
			EXEC,
			MATH,
			LINK,
			UTIL
		};
		namespace DATA {
			enum struct Type {
				EMPTY,
				ANY,
				STRING, DOUBLE, BOOL, UINT, INT,
				MAT2, MAT3, MAT4, UMAT2, UMAT3, UMAT4, IMAT2, IMAT3, IMAT4,
				VEC2, VEC3, VEC4, UVEC2, UVEC3, UVEC4, IVEC2, IVEC3, IVEC4,
				TRANSFORM, TEXTURE, OBJECT, SCENE, DATA,
			};
			enum struct Modifier {
				SINGLE,
				VECTOR,
				MAP
			};
		}
		struct Data {
			any data;
			DATA::Type type;
			DATA::Modifier modifier;

			Data();
			Data(const any& data, const DATA::Type& type, const DATA::Modifier& modifier = DATA::Modifier::SINGLE);

			Data operator+(const Data& other);
			Data operator-(const Data& other);
			Data operator*(const Data& other);
			Data operator/(const Data& other);
		};

		struct Port {
			Node* node; // ref
			string slot_id;

			Port(Node* node);

			virtual Data getData() const { return Data(); }
			virtual void exec() const {}
		};
		namespace PORT {
			enum struct Type {
				DATA_I,
				DATA_O,
				EXEC_I,
				EXEC_O
			};
			struct Data_I_Port : Port {
				Data_O_Port* connection; // ref
				Data default_value;

				DATA::Type type;
				DATA::Modifier modifier;

				Data_I_Port(Node* parent, const string& slot_id, const DATA::Type& type, const DATA::Modifier& modifier = DATA::Modifier::SINGLE);
				~Data_I_Port();

				Data getData() const override;
			};

			struct Data_O_Port : Port {
				vector<Data_I_Port*> outgoing_connections; // ref

				DATA::Type type;
				DATA::Modifier modifier;

				Data_O_Port(Node* parent, const string& slot_id, const DATA::Type& type, const DATA::Modifier& modifier = DATA::Modifier::SINGLE);
				~Data_O_Port();

				Data getData() const override;
			};

			struct Exec_I_Port : Port {
				vector<Exec_O_Port*> incoming_connections; // ref

				Exec_I_Port(Node* parent, const string& slot_id);
				~Exec_I_Port();

				void exec() const override;
			};

			struct Exec_O_Port : Port {
				Exec_I_Port* connection; // ref

				Exec_O_Port(Node* parent, const string& slot_id);
				~Exec_O_Port();

				void exec() const override;
			};
		}
	}
}

QColor typeColor(const CLASS::NODE::DATA::Type& type);