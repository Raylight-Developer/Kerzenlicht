#include "Node_GUI.hpp"

GUI::NODE::Node_Tree::Node_Tree() {
	nodes = {};

	references = {};
	variables = {};
}

GUI::NODE::Node_Tree::~Node_Tree() {
	for (Node* node : nodes) delete node;
}

GUI::NODE::Node::Node(QGraphicsItem* parent) :
	QGraphicsItem(parent)
{
	setZValue(1);
	setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
	type = CLASS::NODE::Type::NONE;
	sub_type = 0;
	outputs = {};
	inputs = {};
}

void GUI::NODE::Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	if (isSelected())
		painter->setPen(QPen(QColor(150,200,255), 3.0));

	painter->setBrush(QColor(40, 40, 40));
	painter->drawRoundedRect(rect, 5, 5);


	painter->setBrush(QColor(25, 25, 25));
	painter->drawRoundedRect(QRectF(rect.topLeft(), QSize(rect.width(), 20)), 5, 5);

	painter->setPen(Qt::white);
	painter->drawText(QRectF(rect.topLeft(), QSize(rect.width(), 20)), Qt::AlignCenter, label);
}

GUI::NODE::Port::Port(Node* node) :
	QGraphicsItem(node),
	node(node)
{
	setZValue(10);
	slot_id = 0;
	type = CLASS::NODE::PORT::Type::NONE;
}

GUI::NODE::Node::~Node() {
	for (Port* port : inputs) delete port;
	for (Port* port : outputs) delete port;
}

QRectF GUI::NODE::Port::boundingRect() const {
	return rect.marginsAdded(QMarginsF(5, 5, 5, 5));
}

void GUI::NODE::Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->setPen(Qt::white);

	switch (type) {
		case CLASS::NODE::PORT::Type::DATA_I: {
			painter->drawEllipse(rect);

			painter->drawText(rect.bottomRight() + QPointF(10, 0), label);
			break;
		}
		case CLASS::NODE::PORT::Type::DATA_O: {
			painter->drawEllipse(rect);

			const QFontMetrics fontMetrics(painter->font());
			painter->drawText(rect.bottomLeft() - QPointF(10 + fontMetrics.horizontalAdvance(label), 0), label);
			break;
		}
		case CLASS::NODE::PORT::Type::EXEC_I: {
			painter->drawRoundedRect(rect, 5, 5);

			painter->drawText(rect.bottomRight() + QPointF(10, 0), label);
			break;
		}
		case CLASS::NODE::PORT::Type::EXEC_O: {
			painter->drawRoundedRect(rect, 5, 5);

			const QFontMetrics fontMetrics(painter->font());
			painter->drawText(rect.bottomLeft() - QPointF(10 + fontMetrics.horizontalAdvance(label), 0), label);
			break;
		}
	}
}

GUI::NODE::PORT::Data_I_Port::Data_I_Port(Node* parent, const uint16& slot_id, const QString& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier) :
	Port(parent)
{
	this->type = CLASS::NODE::PORT::Type::DATA_I;
	this->modifier = modifier;
	this->slot_id = slot_id;
	this->data_type = type;
	this->label = label;

	this->any_data_type = CLASS::NODE::DATA::Type::NONE;
	this->connection = nullptr;

	rect = QRectF(node->rect.topLeft().x() - 5, node->rect.topLeft().y() + 35 + slot_id * 20, 10, 10);
	color = typeColor(type);
}

GUI::NODE::PORT::Data_I_Port::~Data_I_Port() {
	if (connection) {
		if (Data_O_Port* port = dynamic_cast<Data_O_Port*>(connection->port_l)) {
			for (auto it = port->outgoing_connections.begin(); it != port->outgoing_connections.end(); ++it) {
				if (*it == connection) {
					port->outgoing_connections.erase(it);
					break;
				}
			}
		}
		delete connection;
		connection = nullptr;
	}
}

void GUI::NODE::PORT::Data_I_Port::setDataType(const CLASS::NODE::DATA::Type& type) {
	data_type = type;
	color = typeColor(type);
};

void GUI::NODE::PORT::Data_I_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->setPen(Qt::white);
	painter->drawEllipse(rect);

	painter->drawText(rect.bottomRight() + QPointF(10, 0), label);
}

GUI::NODE::PORT::Data_O_Port::Data_O_Port(Node* parent, const uint16& slot_id, const QString& label, const CLASS::NODE::DATA::Type& type, const CLASS::NODE::DATA::Modifier& modifier) :
	Port(parent)
{
	this->type = CLASS::NODE::PORT::Type::DATA_O;
	this->modifier = modifier;
	this->slot_id = slot_id;
	this->data_type = type;
	this->label = label;

	this->any_data_type = CLASS::NODE::DATA::Type::NONE;
	this->outgoing_connections = {};

	rect = QRectF(node->rect.topRight().x() - 5, node->rect.topLeft().y() + 35 + slot_id * 20, 10, 10);
	color = typeColor(type);
}

GUI::NODE::PORT::Data_O_Port::~Data_O_Port() {
	for (Connection* connection : outgoing_connections) {
		static_cast<Data_I_Port*>(connection->port_r)->connection = nullptr;
		delete connection;
	}
}

void GUI::NODE::PORT::Data_O_Port::setDataType(const CLASS::NODE::DATA::Type& type) {
	data_type = type;
	color = typeColor(type);
};

void GUI::NODE::PORT::Data_O_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->setPen(Qt::white);

	painter->drawEllipse(rect);
	const QFontMetrics fontMetrics(painter->font());
	painter->drawText(rect.bottomLeft() - QPointF(10 + fontMetrics.horizontalAdvance(label), 0), label);
}

GUI::NODE::PORT::Exec_I_Port::Exec_I_Port(Node* parent, const uint16& slot_id, const QString& label) :
	Port(parent)
{
	this->type = CLASS::NODE::PORT::Type::EXEC_I;
	this->slot_id = slot_id;
	this->label = label;

	this->incoming_connections = {};
	rect = QRectF(node->rect.topLeft().x() - 5, node->rect.topLeft().y() + 35 + slot_id * 20, 10, 10);
	color = QColor(250, 250, 250);
}

GUI::NODE::PORT::Exec_I_Port::~Exec_I_Port() {
	for (Connection* connection : incoming_connections) {
		static_cast<Exec_O_Port*>(connection->port_l)->connection = nullptr;
		delete connection;
	}
}

void GUI::NODE::PORT::Exec_I_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->setPen(Qt::white);
	painter->drawRoundedRect(rect, 2, 2);

	painter->drawText(rect.bottomRight() + QPointF(10, 0), label);
}

GUI::NODE::PORT::Exec_O_Port::Exec_O_Port(Node* parent, const uint16& slot_id, const QString& label) :
	Port(parent)
{
	this->type = CLASS::NODE::PORT::Type::EXEC_O;
	this->slot_id = slot_id;
	this->label = label;

	this->connection = nullptr;
	rect = QRectF(node->rect.topRight().x() - 5, node->rect.topLeft().y() + 35 + slot_id * 20, 10, 10);
	color = QColor(250, 250, 250);
}

void GUI::NODE::PORT::Exec_O_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->setPen(Qt::white);

	painter->drawRoundedRect(rect, 2, 2);
	const QFontMetrics fontMetrics(painter->font());
	painter->drawText(rect.bottomLeft() - QPointF(10 + fontMetrics.horizontalAdvance(label), 0), label);
}

GUI::NODE::PORT::Exec_O_Port::~Exec_O_Port() {
	if (connection) {
		if (Exec_I_Port* port = dynamic_cast<Exec_I_Port*>(connection->port_r)) {
			for (auto it = port->incoming_connections.begin(); it != port->incoming_connections.end(); ++it) {
				if (*it == connection) {
					port->incoming_connections.erase(it);
					break;
				}
			}
		}
		delete connection;
		connection = nullptr;
	}
}

GUI::NODE::Connection::Connection(Port* port_l, Port* port_r) :
	QGraphicsItem(port_l),
	port_l(port_l),
	port_r(port_r)
{
	setZValue(3);

	pos_l = mapFromItem(port_l, port_l->boundingRect().center());
	pos_r = mapFromItem(port_l, port_l->boundingRect().center());

	data_type = CLASS::NODE::DATA::Type::NONE;
	color = QColor(255, 255, 255);


	if (port_l and !port_r) {
		if (auto port_l_d = dynamic_cast<PORT::Data_O_Port*>(port_l)) {
			if (port_l->color != typeColor(CLASS::NODE::DATA::Type::ANY)) {
				data_type = port_l_d->data_type;
			}
			else {
				data_type = CLASS::NODE::DATA::Type::ANY;
			}
			color = port_l->color;
		}
	}
	else {
		if (auto port_l_d = dynamic_cast<PORT::Data_O_Port*>(port_l)) {
			if (port_l->color != typeColor(CLASS::NODE::DATA::Type::ANY)) {
				data_type = port_l_d->data_type;
			}
			else {
				data_type = CLASS::NODE::DATA::Type::ANY;
			}
			color = port_l->color;
		}
		else if (auto port_r_d = dynamic_cast<PORT::Data_I_Port*>(port_r)) {
			if (port_r->color != typeColor(CLASS::NODE::DATA::Type::ANY)) {
				data_type = port_r_d->data_type;
			}
			else {
				data_type = CLASS::NODE::DATA::Type::ANY;
			}
			color = port_r->color;
		}
		//port_l->onConnect(this);
		//port_r->onConnect(this);
		pos_r = mapFromItem(port_r, port_r->boundingRect().center());
	}
}

QRectF GUI::NODE::Connection::boundingRect() const {
	const QPointF top_left = QPointF(min(pos_l.x(), pos_r.x()), min(pos_l.y(), pos_r.y()));
	const QPointF bottom_right = QPointF(max(pos_l.x(), pos_r.x()), max(pos_l.y(), pos_r.y()));
	return QRectF(top_left, bottom_right);
}

void GUI::NODE::Connection::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setPen(QPen(color, 2.0));
	if (port_l and port_r) {
		pos_l = mapFromItem(port_l, port_l->boundingRect().center());
		pos_r = mapFromItem(port_r, port_r->boundingRect().center());

		QPainterPath path;
		path.moveTo(pos_l + QPointF(6, 0));
		path.lineTo(pos_l + QPointF(10, 0));
		path.lineTo(pos_r - QPointF(10, 0));
		path.lineTo(pos_r - QPointF(6, 0));

		painter->drawPath(path);
	}
	else {
		QPainterPath path;
		path.moveTo(pos_l);
		path.lineTo(pos_r);

		painter->drawPath(path);
	}
}

void GUI::NODE::Connection::updateL(const QPointF& point) {
	pos_l = mapToParent(point) - port_l->scenePos();
}

void GUI::NODE::Connection::updateR(const QPointF& point) {
	pos_r = mapToParent(point) - port_l->scenePos();
}