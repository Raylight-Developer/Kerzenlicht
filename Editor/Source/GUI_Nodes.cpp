#include "GUI_Nodes.hpp"

GUI::NODE::Node_Tree::Node_Tree() {
	nodes = {};

	references = {};
	variables = {};
}

GUI::NODE::Node_Tree::~Node_Tree() {
	for (Node* node : nodes) delete node;
}

GUI::NODE::Node::Node(QGraphicsItem* parent) :
	QGraphicsObject(parent)
{
	setZValue(1);
	setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
	type = KL::NODE::Type::NONE;
	sub_type = 0;
	outputs = {};
	inputs = {};
	node_pos = QPointF(0, 0);
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
	type = KL::NODE::PORT::Type::NONE;
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
		case KL::NODE::PORT::Type::DATA_I: {
			painter->drawEllipse(rect);

			painter->drawText(rect.bottomRight() + QPointF(10, 0), label);
			break;
		}
		case KL::NODE::PORT::Type::DATA_O: {
			painter->drawEllipse(rect);

			const QFontMetrics fontMetrics(painter->font());
			painter->drawText(rect.bottomLeft() - QPointF(10 + fontMetrics.horizontalAdvance(label), 0), label);
			break;
		}
		case KL::NODE::PORT::Type::EXEC_I: {
			painter->drawRoundedRect(rect, 5, 5);

			painter->drawText(rect.bottomRight() + QPointF(10, 0), label);
			break;
		}
		case KL::NODE::PORT::Type::EXEC_O: {
			painter->drawRoundedRect(rect, 5, 5);

			const QFontMetrics fontMetrics(painter->font());
			painter->drawText(rect.bottomLeft() - QPointF(10 + fontMetrics.horizontalAdvance(label), 0), label);
			break;
		}
	}
}

GUI::NODE::PORT::Data_I_Port::Data_I_Port(Node* parent, const uint16& slot_id, const QString& label, const KL::PROP::Type& type, const KL::PROP::Modifier& modifier) :
	Port(parent)
{
	this->type = KL::NODE::PORT::Type::DATA_I;
	this->modifier = modifier;
	this->slot_id = slot_id;
	this->data_type = type;
	this->label = label;

	this->any_data_type = KL::PROP::Type::NONE;
	this->connection = nullptr;

	rect = QRectF(node->rect.topLeft().x() - 5, node->rect.topLeft().y() + 30 + slot_id * 20, 25 + QFontMetrics(QApplication::font()).horizontalAdvance(label), 20);
	auto temp_color = typeColor(type);
	color = QColor(temp_color.x, temp_color.y, temp_color.z);
}

GUI::NODE::PORT::Data_I_Port::~Data_I_Port() {
	if (connection) {
		if (Data_O_Port* port = dynamic_cast<Data_O_Port*>(connection->port_l)) {
			port->outgoing_connections.erase(std::find(port->outgoing_connections.begin(), port->outgoing_connections.end(), connection));
		}
		delete connection;
		connection = nullptr;
	}
}

void GUI::NODE::PORT::Data_I_Port::setDataType(const KL::PROP::Type& type) {
	data_type = type;
	auto temp_color = typeColor(type);
	color = QColor(temp_color.x, temp_color.y, temp_color.z);
};

void GUI::NODE::PORT::Data_I_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->setPen(Qt::white);
	painter->drawEllipse(QRectF(rect.topLeft() + QPointF(0,5), QSizeF(10,10)));

	painter->drawText(rect.bottomLeft() + QPointF(20, -4), label);
}

GUI::NODE::PORT::Data_O_Port::Data_O_Port(Node* parent, const uint16& slot_id, const QString& label, const KL::PROP::Type& type, const KL::PROP::Modifier& modifier) :
	Port(parent)
{
	this->type = KL::NODE::PORT::Type::DATA_O;
	this->modifier = modifier;
	this->slot_id = slot_id;
	this->data_type = type;
	this->label = label;

	this->any_data_type = KL::PROP::Type::NONE;
	this->outgoing_connections = {};

	const qreal width = QFontMetrics(QApplication::font()).horizontalAdvance(label) + 25;
	rect = QRectF(node->rect.topRight().x() - width-5, node->rect.topLeft().y() + 30 + slot_id * 20, width + 10, 20);
	auto temp_color = typeColor(type);
	color = QColor(temp_color.x, temp_color.y, temp_color.z);
}

GUI::NODE::PORT::Data_O_Port::~Data_O_Port() {
	for (Connection* connection : outgoing_connections) {
		static_cast<Data_I_Port*>(connection->port_r)->connection = nullptr;
		delete connection;
	}
	outgoing_connections.clear();
}

void GUI::NODE::PORT::Data_O_Port::setDataType(const KL::PROP::Type& type) {
	data_type = type;
	auto temp_color = typeColor(type);
	color = QColor(temp_color.x, temp_color.y, temp_color.z);
};

void GUI::NODE::PORT::Data_O_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->setPen(Qt::white);

	painter->drawEllipse(QRectF(rect.topRight() + QPointF(-10,5), QSizeF(10,10)));
	const QFontMetrics fontMetrics(painter->font());
	painter->drawText(rect.bottomLeft() + QPointF(5, -4), label);
}

GUI::NODE::PORT::Exec_I_Port::Exec_I_Port(Node* parent, const uint16& slot_id, const QString& label) :
	Port(parent)
{
	this->type = KL::NODE::PORT::Type::EXEC_I;
	this->slot_id = slot_id;
	this->label = label;

	this->incoming_connections = {};
	rect = QRectF(node->rect.topLeft().x() - 5, node->rect.topLeft().y() + 30 + slot_id * 20, 25 + QFontMetrics(QApplication::font()).horizontalAdvance(label), 20);
	color = QColor(250, 250, 250);
}

GUI::NODE::PORT::Exec_I_Port::~Exec_I_Port() {
	for (Connection* connection : incoming_connections) {
		static_cast<Exec_O_Port*>(connection->port_l)->connection = nullptr;
		delete connection;
	}
	incoming_connections.clear();
}

void GUI::NODE::PORT::Exec_I_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->setPen(Qt::white);
	painter->drawRoundedRect(QRectF(rect.topLeft() + QPointF(0,5), QSizeF(10,10)), 2, 2);

	painter->drawText(rect.bottomLeft() + QPointF(20, -4), label);
}

GUI::NODE::PORT::Exec_O_Port::Exec_O_Port(Node* parent, const uint16& slot_id, const QString& label) :
	Port(parent)
{
	this->type = KL::NODE::PORT::Type::EXEC_O;
	this->slot_id = slot_id;
	this->label = label;

	this->connection = nullptr;

	const qreal width = QFontMetrics(QApplication::font()).horizontalAdvance(label) + 25;
	rect = QRectF(node->rect.topRight().x() - width - 5, node->rect.topLeft().y() + 30 + slot_id * 20, width + 10, 20);
	color = QColor(250, 250, 250);
}

void GUI::NODE::PORT::Exec_O_Port::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setBrush(color);
	painter->setPen(Qt::white);

	painter->drawRoundedRect(QRectF(rect.topRight() + QPointF(-10,5), QSizeF(10,10)), 2, 2);
	const QFontMetrics fontMetrics(painter->font());
	painter->drawText(rect.bottomLeft() + QPointF(5, -4), label);
}

GUI::NODE::PORT::Exec_O_Port::~Exec_O_Port() {
	if (connection) {
		if (Exec_I_Port* port = dynamic_cast<Exec_I_Port*>(connection->port_r)) {
			port->incoming_connections.erase(std::find(port->incoming_connections.begin(), port->incoming_connections.end(), connection));
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
	setZValue(5);

	pos_l = mapFromItem(port_l, port_l->boundingRect().topRight());
	pos_r = pos_l;

	data_type = KL::PROP::Type::NONE;
	color = QColor(255, 255, 255);

	auto temp_color = typeColor(KL::PROP::Type::ANY);
	auto any_color = QColor(temp_color.x, temp_color.y, temp_color.z);

	if (port_l and !port_r) {
		if (auto port_l_d = dynamic_cast<PORT::Data_O_Port*>(port_l)) {
			if (port_l->color != any_color) {
				data_type = port_l_d->data_type;
			}
			else {
				data_type = KL::PROP::Type::ANY;
			}
			color = port_l->color;
		}
	}
	else {
		if (auto port_l_d = dynamic_cast<PORT::Data_O_Port*>(port_l)) {
			if (port_l->color != any_color) {
				data_type = port_l_d->data_type;
			}
			else {
				data_type = KL::PROP::Type::ANY;
			}
			color = port_l->color;
		}
		else if (auto port_r_d = dynamic_cast<PORT::Data_I_Port*>(port_r)) {
			if (port_r->color != any_color) {
				data_type = port_r_d->data_type;
			}
			else {
				data_type = KL::PROP::Type::ANY;
			}
			color = port_r->color;
		}
		//port_l->onConnect(this);
		//port_r->onConnect(this);
		pos_r = mapFromItem(port_r, port_r->boundingRect().center());
	}
}

GUI::NODE::Connection::~Connection() {
	if (auto port = dynamic_cast<PORT::Exec_I_Port*>(port_r)) {
		auto it = std::find(port->incoming_connections.begin(), port->incoming_connections.end(), this);
		if (it != port->incoming_connections.end()) {
			port->incoming_connections.erase(it);
		}
	}
	if (auto port = dynamic_cast<PORT::Exec_O_Port*>(port_l)) {
		port->connection = nullptr;
	}
	if (auto port = dynamic_cast<PORT::Data_I_Port*>(port_r)) {
		port->connection = nullptr;
	}
	if (auto port = dynamic_cast<PORT::Data_O_Port*>(port_l)) {
		auto it = std::find(port->outgoing_connections.begin(), port->outgoing_connections.end(), this);
		if (it != port->outgoing_connections.end()) {
			port->outgoing_connections.erase(it);
		}
	}
}

QRectF GUI::NODE::Connection::boundingRect() const {
	const QPointF point_a = pos_l + QPointF(0, 15);
	const QPointF point_b = pos_r + QPointF(0, 15);

	qreal left   = qMin(point_a.x(), point_b.x());
	qreal top    = qMin(point_a.y(), point_b.y());
	qreal right  = qMax(point_a.x(), point_b.x());
	qreal bottom = qMax(point_a.y(), point_b.y());

	return QRectF(left, top, right - left, bottom - top);
}

void GUI::NODE::Connection::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setPen(QPen(color, 2.0));
	if (port_l and port_r) {
		pos_l = mapFromItem(port_l, port_l->boundingRect().topRight());
		pos_r = mapFromItem(port_r, port_r->boundingRect().topLeft());

		QPainterPath path;
		path.moveTo(pos_l + QPointF(0, 15));
		path.lineTo(pos_l + QPointF((pos_r.x() - pos_l.x())/2.0, 15));
		path.lineTo(pos_r + QPointF((pos_l.x() - pos_r.x())/2.0, 15));
		path.lineTo(pos_r + QPointF(0, 15));

		painter->drawPath(path);
	}
	else {
		QPainterPath path;
		path.moveTo(pos_l + QPointF(0, 15));
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