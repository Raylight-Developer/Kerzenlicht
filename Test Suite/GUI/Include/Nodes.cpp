#include "Nodes.hpp"

Node::Node(const QPoint& pos) :
	QGraphicsItem()
{
	setZValue(1);

	label = "Node";
	rect = QRectF(-100, -20, 200, 40);
	rect.moveTo(pos);
}

Node::~Node() {
	for (Port* port : inputs) delete port;
	for (Port* port : outputs) delete port;
}

void Node::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(QColor(40, 40, 40));
	painter->drawRoundedRect(rect, 5, 5);

	painter->setBrush(QColor(25, 25, 25));
	painter->drawRoundedRect(QRectF(rect.topLeft(), QSize(rect.width(), 20)), 5, 5);

	painter->setPen(Qt::white);
	painter->drawText(QRectF(rect.topLeft(), QSize(rect.width(), 20)), Qt::AlignCenter, label);
}

void Node::updateRect() {
	rect.setHeight(40 + max(inputs.size(), outputs.size()) * 20);
}

Port::Port(Node* node) :
	QGraphicsItem(node),
	node(node)
{
	setZValue(10);
}

Data_I_Port::Data_I_Port(Node* node, const uint16& i, const string& slot_id, const QString& label) :
	Port(node)
{
	this->node = node;
	this->label = label;
	this->slot_id = slot_id;
	this->connection = nullptr;
	rect = QRectF(node->boundingRect().topLeft().x() - 5, node->boundingRect().topLeft().y() + 35 + i * 20, 10, 10);
}

Data_I_Port::~Data_I_Port() {
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
	}
}

void Data_I_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->drawEllipse(rect);

	painter->setPen(Qt::white);
	painter->drawText(rect.bottomRight() + QPointF(10, 0), label);
}

any Data_I_Port::getData() const {
	if (connection) return connection->port_r->getData();
	return default_value;
}

Data_O_Port::Data_O_Port(Node* node, const uint16& i, const string& slot_id, const QString& label) :
	Port(node)
{
	this->node = node;
	this->label = label;
	this->slot_id = slot_id;
	rect = QRectF(node->boundingRect().topRight().x() - 5, node->boundingRect().topRight().y() + 35 + i * 20, 10, 10);
}

Data_O_Port::~Data_O_Port() {
	for (Connection* connection : outgoing_connections) {
		if (Data_I_Port* port = dynamic_cast<Data_I_Port*>(connection->port_l)) {
			port->connection = nullptr;
		}
		delete connection;
	}
}

void Data_O_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->drawEllipse(rect);

	painter->setPen(Qt::white);
	const QFontMetrics fontMetrics(painter->font());
	painter->drawText(rect.bottomLeft() - QPointF(10 + fontMetrics.horizontalAdvance(label), 0), label);
}

any Data_O_Port::getData() const {
	return node->getData(slot_id);
}

Exec_I_Port::Exec_I_Port(Node* node, const uint16& i, const string& slot_id, const QString& label) :
	Port(node)
{
	this->node = node;
	this->label = label;
	this->slot_id = slot_id;
	rect = QRectF(node->boundingRect().topLeft().x() - 5, node->boundingRect().topLeft().y() + 35 + i * 20, 10, 10);
}

Exec_I_Port::~Exec_I_Port() {
	for (Connection* connection : incoming_connections) {
		if (Exec_O_Port* port = dynamic_cast<Exec_O_Port*>(connection->port_r)) {
			port->connection = nullptr;
		}
		delete connection;
	}
}

void Exec_I_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->drawRoundedRect(rect, 2, 2);

	painter->setPen(Qt::white);
	painter->drawText(rect.bottomRight() + QPointF(10, 0), label);
}

void Exec_I_Port::exec() {
	node->exec(slot_id);
}

Exec_O_Port::Exec_O_Port(Node* node, const uint16& i, const string& slot_id, const QString& label) :
	Port(node)
{
	this->node = node;
	this->label = label;
	this->slot_id = slot_id;
	connection = nullptr;
	rect = QRectF(node->boundingRect().topRight().x() - 5, node->boundingRect().topRight().y() + 35 + i * 20, 10, 10);
}

Exec_O_Port::~Exec_O_Port() {
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
	}
}

void Exec_O_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->drawRoundedRect(rect, 2, 2);

	painter->setPen(Qt::white);
	const QFontMetrics fontMetrics(painter->font());
	painter->drawText(rect.bottomLeft() - QPointF(10 + fontMetrics.horizontalAdvance(label), 0), label);
}

void Exec_O_Port::exec() {
	if (connection) connection->port_l->exec();
}

Connection::Connection(Port* parent) :
	QGraphicsItem(parent),
	port_l(parent),
	port_r(nullptr)
{
	setZValue(2);
	pos_l = mapFromItem(port_l, port_l->boundingRect().center());
	pos_r = mapFromItem(port_l, port_l->boundingRect().center());
}

Connection::Connection(Port* in, Port* out) :
	QGraphicsItem(in),
	port_l(in),
	port_r(out)
{
	setZValue(2);
}

void Connection::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	if (port_l and port_r) {
		pos_l = mapFromItem(port_r, port_r->boundingRect().center());
		pos_r = mapFromParent(port_l->boundingRect().center());

		QPainterPath path;
		path.moveTo(pos_l + QPointF(6, 0));
		path.lineTo(pos_l + QPointF(10, 0));
		path.lineTo(pos_r - QPointF(10, 0));
		path.lineTo(pos_r - QPointF(6, 0));

		painter->setPen(QPen(Qt::white, 2.0));
		painter->drawPath(path);
	}
	else {
		QPainterPath path;
		path.moveTo(pos_l);
		path.lineTo(pos_r);

		painter->setPen(QPen(Qt::white, 2.0));
		painter->drawPath(path);
	}
}

QRectF Connection::boundingRect() const {
	const QPointF top_left = QPointF(min(pos_l.x(), pos_r.x()), min(pos_l.y(), pos_r.y()));
	const QPointF bottom_right = QPointF(max(pos_l.x(), pos_r.x()), max(pos_l.y(), pos_r.y()));
	return QRectF(top_left, bottom_right);
}

void Connection::updateEnd() {
	pos_r = mapFromParent(port_l->boundingRect().center());
}

void Connection::updateStart() {
	pos_l = mapFromParent(port_r->boundingRect().center());
}

void Connection::updateEnd(const QPointF& point) {
	pos_r = mapToParent(point) - port_r->scenePos();
}

void Connection::updateStart(const QPointF& point) {
	pos_l = mapToParent(point) - port_l->scenePos();
}