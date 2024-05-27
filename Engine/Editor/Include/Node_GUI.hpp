#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Core/Nodes.hpp"
#include "Core/Node_Def.hpp"

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
			vector<Node*> variables;  // ref
			vector<Node*> references; // ref

			~Node_Tree();
		};
		struct Node : QGraphicsItem {
			QString label;
			QRectF rect;
			CLASS::NODE::Type type;
			uint16 sub_type;

			vector<Port*> inputs;  // src
			vector<Port*> outputs; // src

			unordered_map<string, any> internal_data;

			Node();
			~Node();

			virtual void onPortConnect(Port* port, Connection* connection) {};
			virtual void onPortDisconnect(Port* port) {};

			QRectF boundingRect() const override { return rect; };
			void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			void paintDefault(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
		};
		struct Port : QGraphicsItem {
			Node* node; // ref
			uint16 index;
			string slot_id;

			QRectF rect;
			QColor color;
			QString label;
			CLASS::NODE::PORT::Type type;

			Port(Node* node);

			void onConnect(Connection* connection);
			void onDisconnect();

			QRectF boundingRect() const override;
			void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
		};
		namespace PORT {
			struct Data_I_Port : Port {
				Connection* connection; // src

				CLASS::NODE::DATA::Type data_type;
				CLASS::NODE::DATA::Modifier modifier;

				CLASS::NODE::DATA::Type any_data_type;

				Data_I_Port(Node* parent, const uint16& index, const QString& label, const string& slot_id, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier = CLASS::NODE::DATA::Modifier::SINGLE);
				~Data_I_Port();

				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};

			struct Data_O_Port : Port {
				vector<Connection*> outgoing_connections; // ref

				CLASS::NODE::DATA::Type data_type;
				CLASS::NODE::DATA::Modifier modifier;

				CLASS::NODE::DATA::Type any_data_type;

				Data_O_Port(Node* parent, const uint16& index, const QString& label, const string& slot_id, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier = CLASS::NODE::DATA::Modifier::SINGLE);
				~Data_O_Port();

				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};

			struct Exec_I_Port : Port {
				vector<Connection*> incoming_connections; // ref

				Exec_I_Port(Node* parent, const uint16& index, const QString& label, const string& slot_id);
				~Exec_I_Port();

				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};

			struct Exec_O_Port : Port {
				Connection* connection; // src

				Exec_O_Port(Node* parent, const uint16& index, const QString& label, const string& slot_id);
				~Exec_O_Port();

				void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
			};
		}
		struct Connection : QGraphicsItem {
			Port* port_l; // ref
			Port* port_r; // ref
			QPointF pos_l;
			QPointF pos_r;

			QColor color;

			Connection(Port* port_l, Port* port_r);

			QRectF boundingRect() const override;
			void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

			void updateL(const QPointF& point);
			void updateR(const QPointF& point);
		};
	};
};