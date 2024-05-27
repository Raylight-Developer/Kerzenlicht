#pragma once

#include "Include.hpp"
#include "Ops.hpp"
#include "QT.hpp"

struct Node;
struct Port;

struct Data_I_Port;
struct Data_O_Port;
struct Exec_I_Port;
struct Exec_O_Port;

struct Connection;

struct Node : QGraphicsItem {
	QString label;
	QRectF rect;
	QPointF last_pos;

	vector<Port*> inputs;  // src
	vector<Port*> outputs; // src

	Node(const QPoint& pos = QPoint(0,0));
	~Node();
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	QRectF boundingRect() const override { return rect; };
	virtual void updateRect();

	virtual void build() = 0;
	virtual any getData(const string& slot_id) const { return false; };
	virtual void exec(const string& slot_id) {};

};

struct Port : QGraphicsItem {
	Node* node; // ref
	QString label;
	QColor color;
	QRectF rect;

	Port(Node* node);;

	virtual any getData() const { return nullptr; };
	virtual void exec() {};
};

struct Data_I_Port : Port {
	Connection* connection; // src
	string slot_id;
	any default_value;

	Data_I_Port(Node* parent, const uint16& i, const string& slot_id, const QString& label);
	~Data_I_Port();
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	QRectF boundingRect() const override { return rect.marginsAdded(QMarginsF(5, 5, 5, 5)); };

	any getData() const override;
};

struct Data_O_Port : Port {
	vector<Connection*> outgoing_connections; // ref
	string slot_id;

	Data_O_Port(Node* parent, const uint16& i, const string& slot_id, const QString& label);
	~Data_O_Port();
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	QRectF boundingRect() const override { return rect.marginsAdded(QMarginsF(5, 5, 5, 5)); };

	any getData() const override;
};

struct Exec_I_Port : Port {
	vector<Connection*> incoming_connections; // ref
	string slot_id;

	Exec_I_Port(Node* parent, const uint16& i, const string& slot_id, const QString& label);
	~Exec_I_Port();
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	QRectF boundingRect() const override { return rect.marginsAdded(QMarginsF(5, 5, 5, 5)); };

	void exec() override;
};

struct Exec_O_Port : Port {
	Connection* connection; // src
	string slot_id;

	Exec_O_Port(Node* parent, const uint16& i, const string& slot_id, const QString& label);
	~Exec_O_Port();
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	QRectF boundingRect() const override { return rect.marginsAdded(QMarginsF(5, 5, 5, 5)); };

	void exec() override;
};

struct Connection : QGraphicsItem {
	Port* port_l; // ref
	Port* port_r; // ref
	QPointF pos_l;
	QPointF pos_r;

	Connection(Port* parent);
	Connection(Port* left, Port* right);

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	QRectF boundingRect() const override;

	void updateEnd();
	void updateStart();
	void updateEnd(const QPointF& point);
	void updateStart(const QPointF& point);
};