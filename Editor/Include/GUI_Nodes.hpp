#pragma once

#include "Shared.hpp"

#include "QT.hpp"

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

			KL::Node_Tree* toExecTree();
		};
		struct Node : QGraphicsObject {
			QString label;
			QRectF rect;
			KL::NODE::Type type;
			uint16 sub_type;
			QPointF real_pos;
			QPointF node_pos;

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
			KL::NODE::PORT::Type type;

			Port(Node* node);

			void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			QRectF boundingRect() const override;
		};
		namespace PORT {
			struct Data_I_Port : Port {
				Connection* connection; // src

				KL::DATA::Modifier modifier;
				KL::DATA::Type data_type;

				KL::DATA::Type any_data_type;

				Data_I_Port(Node* parent, const uint16& slot_id, const QString& label, const KL::DATA::Type& type, const KL::DATA::Modifier& modifier = KL::DATA::Modifier::SINGLE);
				~Data_I_Port();

				void setDataType(const KL::DATA::Type& type);
				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};

			struct Data_O_Port : Port {
				vector<Connection*> outgoing_connections; // ref

				KL::DATA::Modifier modifier;
				KL::DATA::Type data_type;

				KL::DATA::Type any_data_type;

				Data_O_Port(Node* parent, const uint16& slot_id, const QString& label, const KL::DATA::Type& type, const KL::DATA::Modifier& modifier = KL::DATA::Modifier::SINGLE);
				~Data_O_Port();

				void setDataType(const KL::DATA::Type& type);
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

			KL::DATA::Type data_type;
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