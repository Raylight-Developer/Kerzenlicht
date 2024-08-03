#pragma once

#include "Shared.hpp"

#include "QT.hpp"

//#include "Core/Session.hpp"

#include "Object/Nodes/Object_Node.hpp"
#include "Object/Nodes/Object_Nodes.hpp"

// FWD DECL OTHER

// FWD DECL THIS
namespace GUI {
	namespace NODE {
		struct Node;
		struct Node_Tree;
		struct Port;
		namespace PORT {
			struct Data_I_Port;
			struct Data_O_Port;
			struct Exec_I_Port;
			struct Exec_O_Port;
		}
		struct Connection;
	}
}

// DECL
namespace GUI {
	namespace NODE {
		struct Node_Tree {
			vector<Node*> nodes;      // src

			vector<Node*> references; // ref
			vector<Node*> variables;  // ref

			Node_Tree();
			~Node_Tree();

			CLASS::Node_Tree* toExecTree();
		};
		struct Node : QGraphicsItem {
			QString label;
			QRectF rect;
			CLASS::NODE::Type type;
			uint16 sub_type;
			QPointF real_pos;
			QPointF load_pos;

			vector<Port*> inputs;  // src
			vector<Port*> outputs; // src

			unordered_map<string, any> internal_data;

			Node(QGraphicsItem* parent = nullptr);
			~Node();

			void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			QRectF boundingRect() const override { return rect; };
		};
		struct Port : QGraphicsItem {
			Node* node; // ref
			uint16 slot_id;

			QRectF rect;
			QColor color;
			QString label;
			CLASS::NODE::PORT::Type type;

			Port(Node* node);

			void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			QRectF boundingRect() const override;
		};
		namespace PORT {
			struct Data_I_Port : Port {
				Connection* connection; // src

				CLASS::DATA::Modifier modifier;
				CLASS::DATA::Type data_type;

				CLASS::DATA::Type any_data_type;

				Data_I_Port(Node* parent, const uint16& slot_id, const QString& label, const CLASS::DATA::Type& type, const CLASS::DATA::Modifier& modifier = CLASS::DATA::Modifier::SINGLE);
				~Data_I_Port();

				void setDataType(const CLASS::DATA::Type& type);
				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};

			struct Data_O_Port : Port {
				vector<Connection*> outgoing_connections; // ref

				CLASS::DATA::Modifier modifier;
				CLASS::DATA::Type data_type;

				CLASS::DATA::Type any_data_type;

				Data_O_Port(Node* parent, const uint16& slot_id, const QString& label, const CLASS::DATA::Type& type, const CLASS::DATA::Modifier& modifier = CLASS::DATA::Modifier::SINGLE);
				~Data_O_Port();

				void setDataType(const CLASS::DATA::Type& type);
				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};

			struct Exec_I_Port : Port {
				vector<Connection*> incoming_connections; // ref

				Exec_I_Port(Node* parent, const uint16& slot_id, const QString& label);
				~Exec_I_Port();

				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};

			struct Exec_O_Port : Port {
				Connection* connection; // src

				Exec_O_Port(Node* parent, const uint16& slot_id, const QString& label);
				~Exec_O_Port();

				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};
		}
		struct Connection : QGraphicsItem {
			Port* port_l; // ref
			Port* port_r; // ref
			QPointF pos_l;
			QPointF pos_r;

			CLASS::DATA::Type data_type;
			QColor color;

			Connection(Port* port_l, Port* port_r);
			~Connection();

			void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			QRectF boundingRect() const override;

			void updateL(const QPointF& point);
			void updateR(const QPointF& point);
		};
	};
};