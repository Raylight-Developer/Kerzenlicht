#include "Workspaces/Node_Editor.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_Node_Editor::Workspace_Node_Editor(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	viewport = new Node_Viewport(this);
	shelf = new Node_Shelf(this);

	auto splitter = new GUI::Splitter(this);
	splitter->addWidget(shelf);
	splitter->addWidget(viewport);

	auto header = new GUI::Linear_Contents(this);
	auto compile = new GUI::Button(this);
	compile->setText("Compile");
	header->addWidget(compile);

	addWidget(header);
	addWidget(splitter);

	FILE->active_object->addCallback(this, [this]() { viewport->f_objectChanged(file->active_object->ptr); });
	connect(compile, &GUI::Button::pressed, [this, parent]() {
		if (viewport->active_node_tree and file->active_object->ptr) {
			Lace log_msg;
			log_msg << ENDL << HTML_MAGENTA << "[Compilation]" << HTML_RESET << " Compiling Nodes...";
			*LOG << log_msg;

			file->nodes.erase(std::find(file->nodes.begin(), file->nodes.end(), file->active_object->ptr->nodes));

			auto it = file->node_map.find(file->active_object->ptr->nodes);
			if (it != file->node_map.end()) {
				file->node_map.erase(it);
			}

			delete file->active_object->ptr->nodes;

			auto node = new CLASS::Node_Tree(viewport->active_node_tree);
			file->active_object->ptr->nodes = node;
			file->nodes.push_back(node);
			file->node_map[node] = viewport->active_node_tree;

			log_msg.clear() << ENDL << HTML_GREEN << "[Compilation]" << HTML_RESET << " Compiled Nodes";
			*LOG << log_msg;
		}
	});
}

GUI::WORKSPACE::Node_Viewport::Node_Viewport(Workspace_Node_Editor* parent) :
	GUI::Graphics_View(parent),
	parent(parent)
{
	view_scale = 1.0;
	pan = false;
	moving = false;
	connecting = false;
	connection = nullptr;
	move_selection = nullptr;
	active_node_tree = nullptr;
	scene = new QGraphicsScene(this);

	setScene(scene);
	setSceneRect(-10000, -10000, 20000, 20000);
}

GUI::WORKSPACE::Node_Viewport::~Node_Viewport() {
	for (auto item : scene->items())
		if (dynamic_cast<GUI::NODE::Node*>(item)) scene->removeItem(item);
	parent->file->active_object->callbacks.erase(this);
}

void GUI::WORKSPACE::Node_Viewport::f_objectChanged(CLASS::Object* object) {
	for (auto item: scene->items())
		if (dynamic_cast<GUI::NODE::Node*>(item)) scene->removeItem(item);
	if (object) {
		active_node_tree = parent->file->node_map[object->nodes];
		for (auto node : parent->file->node_map[object->nodes]->nodes)
			scene->addItem(node);
	}
	else active_node_tree = nullptr;
}

void GUI::WORKSPACE::Node_Viewport::drawBackground(QPainter* painter, const QRectF& rect) {
	QGraphicsView::drawBackground(painter, rect);
	if (view_scale > 0.7) {
		painter->setPen(QPen(QColor(60, 60, 60), 1));
		const int gridSize = 10;
		const int xOffset = int(rect.left()) - (int(rect.left()) % gridSize);
		const int yOffset = int(rect.top()) - (int(rect.top()) % gridSize);

		for (int x = xOffset; x < int(rect.right()); x += gridSize)
			painter->drawLine(x, int(rect.top()), x, int(rect.bottom()));
		for (int y = yOffset; y < int(rect.bottom()); y += gridSize)
			painter->drawLine(int(rect.left()), y, int(rect.right()), y);
	}
	else if (view_scale > 0.35) {
		painter->setPen(QPen(QColor(60, 60, 60), 3));
		const int gridSize = 40;
		const int xOffset = int(rect.left()) - (int(rect.left()) % gridSize);
		const int yOffset = int(rect.top()) - (int(rect.top()) % gridSize);

		for (int x = xOffset; x < int(rect.right()); x += gridSize)
			painter->drawLine(x, int(rect.top()), x, int(rect.bottom()));
		for (int y = yOffset; y < int(rect.bottom()); y += gridSize)
			painter->drawLine(int(rect.left()), y, int(rect.right()), y);
	}
	else {
		painter->setPen(QPen(QColor(60, 60, 60), 5));
		const int gridSize = 80;
		const int xOffset = int(rect.left()) - (int(rect.left()) % gridSize);
		const int yOffset = int(rect.top()) - (int(rect.top()) % gridSize);

		for (int x = xOffset; x < int(rect.right()); x += gridSize)
			painter->drawLine(x, int(rect.top()), x, int(rect.bottom()));
		for (int y = yOffset; y < int(rect.bottom()); y += gridSize)
			painter->drawLine(int(rect.left()), y, int(rect.right()), y);
	}
}

void GUI::WORKSPACE::Node_Viewport::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::MouseButton::MiddleButton)
		pan = false;
	if (event->button() == Qt::MouseButton::RightButton)
		moving = false;
	if (event->button() == Qt::MouseButton::LeftButton) {
		if (connecting) {
			if (auto item = scene->itemAt(mapToScene(event->pos()), transform())) {
				if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>(item)) {
					if (!drop_port->connection) {
						if (auto source_port = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(connection->port_l)) {
							if (drop_port->connection)
								delete drop_port->connection;
							drop_port->connection = new GUI::NODE::Connection(source_port, drop_port);
							source_port->outgoing_connections.push_back(drop_port->connection);
						}
					}
				}
				else if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(item)) {
					bool exists = false;
					for (auto conn : drop_port->outgoing_connections)
						if (conn->port_l == connection->port_l)
							exists = true;
					if (!exists) {
						if (auto source_port = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>(connection->port_l)) {
							if (source_port->connection)
								delete source_port->connection;
							source_port->connection = new GUI::NODE::Connection(drop_port, source_port);
							drop_port->outgoing_connections.push_back(source_port->connection);
						}
					}
				}
				else if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(item)) {
					bool exists = false;
					for (auto conn : drop_port->incoming_connections)
						if (conn->port_l == connection->port_l)
							exists = true;
					if (!exists) {
						if (auto source_port = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>(connection->port_l)) {
							if (source_port->connection)
								delete source_port->connection;
							source_port->connection = new GUI::NODE::Connection(source_port, drop_port);
							drop_port->incoming_connections.push_back(source_port->connection);
						}
					}
				}
				else if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>(item)) {
					if (!drop_port->connection) {
						if (auto source_port = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(connection->port_l)) {
							if (drop_port->connection)
								delete drop_port->connection;
							drop_port->connection = new GUI::NODE::Connection(drop_port, source_port);
							source_port->incoming_connections.push_back(drop_port->connection);
						}
					}
				}
			}
			delete connection;
			connection = nullptr;
		};
		connecting = false;
	}
	QGraphicsView::mouseReleaseEvent(event);
}

void GUI::WORKSPACE::Node_Viewport::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::MouseButton::MiddleButton) {
		pan = true;
		pan_pos = event->pos();
	}
	if (event->button() == Qt::MouseButton::LeftButton) {
		if (auto item = scene->itemAt(mapToScene(event->pos()), transform())) {
			if (GUI::NODE::Port* port = dynamic_cast<GUI::NODE::Port*>(item)) {
				connecting = true;
				connection = new GUI::NODE::Connection(port, nullptr);
			}
		}
	}
	if (event->button() == Qt::MouseButton::RightButton) {
		if (auto item = scene->itemAt(mapToScene(event->pos()), transform())) {
			if (GUI::NODE::Node* node = dynamic_cast<GUI::NODE::Node*>(item)) {
				moving = true;
				move_selection = node;
				move_pos = mapToScene(event->pos()) - move_selection->scenePos();
			}
			else if (GUI::NODE::Port* port = dynamic_cast<GUI::NODE::Port*>(item)) {
				moving = true;
				move_selection = port->node;
				move_pos = mapToScene(event->pos()) - move_selection->scenePos();
			}
		}
	}
	QGraphicsView::mousePressEvent(event);
}

void GUI::WORKSPACE::Node_Viewport::mouseMoveEvent(QMouseEvent* event) {
	if (event->modifiers() & Qt::KeyboardModifier::AltModifier) {
		if (auto item = scene->itemAt(mapToScene(event->pos()), transform())) {
			if (auto port_r = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>(item)) {
				if (port_r->connection) {
					auto port_l = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(port_r->connection->port_l);
					port_l->outgoing_connections.erase(std::find(port_l->outgoing_connections.begin(), port_l->outgoing_connections.end(), port_r->connection));

					delete port_r->connection;
					port_r->connection = nullptr;
				}
			}
			else if (auto port_l = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(item)) {
				for (auto conn = port_l->outgoing_connections.begin(); conn != port_l->outgoing_connections.end();) {
					auto port_r = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>((*conn)->port_r);
					conn = port_l->outgoing_connections.erase(conn);
		
					delete port_r->connection;
					port_r->connection = nullptr;
				}
			}
			else if (auto port_r = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(item)) {
				for (auto conn = port_r->incoming_connections.begin(); conn != port_r->incoming_connections.end();) {
					auto port_l = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>((*conn)->port_l);
					conn = port_r->incoming_connections.erase(conn);
		
					delete port_l->connection;
					port_l->connection = nullptr;
				}
			}
			else if (auto port_l = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>(item)) {
				if (port_l->connection) {
					auto port_r = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(port_l->connection->port_r);
					port_r->incoming_connections.erase(std::find(port_r->incoming_connections.begin(), port_r->incoming_connections.end(), port_l->connection));
		
					delete port_l->connection;
					port_l->connection = nullptr;
				}
			}
		}
	}
	else {
		if (pan) {
			const QPointF delta = event->pos() - pan_pos;
			horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
			verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
			pan_pos = event->pos();
		}
		if (moving) {
			const QPointF delta = mapToScene(event->pos()) - move_pos;
			move_selection->setPos(f_roundToNearest(delta.x(), 10.0), f_roundToNearest(delta.y(), 10.0));
		}
		if (connecting) {
			if (connection->port_l)
				connection->updateR(mapToScene(event->pos()));
			else
				connection->updateL(mapToScene(event->pos()));
			connection->update();
		}
	}
	QGraphicsView::mouseMoveEvent(event);
}

void GUI::WORKSPACE::Node_Viewport::resizeEvent(QResizeEvent* event) {
	GUI::Graphics_View::resizeEvent(event);
	setSceneRect(-width(), -height(), width() * 2, height() * 2);
}

void GUI::WORKSPACE::Node_Viewport::wheelEvent(QWheelEvent* event) {
	const qreal zoomFactor = 1.25;

	QPointF oldPos = mapToScene(event->position().toPoint());
	if (event->angleDelta().y() > 0) {
		if (view_scale < 2.0) {
			scale(zoomFactor, zoomFactor);
			view_scale *= zoomFactor;
		}
	}
	else if (event->angleDelta().y() < 0) {
		if (view_scale > 0.2) {
			scale(1.0 / zoomFactor, 1.0 / zoomFactor);
			view_scale /= zoomFactor;
		}
	}

	const QPointF newPos = mapToScene(event->position().toPoint());
	const QPointF delta = newPos - oldPos;
	translate(delta.x(), delta.y());
}

void GUI::WORKSPACE::Node_Viewport::dropEvent(QDropEvent* event) {
	const dvec2 drop_pos = f_roundToNearest(p_to_d(mapToScene(event->position().toPoint())), 10.0);
	if (event->mimeData()->hasText() and active_node_tree) {
		if (event->mimeData()->text() == "NODE") {
			QByteArray itemDataType = event->mimeData()->data("Type");
			QDataStream dataStreamType(&itemDataType, QIODevice::ReadOnly);
			QString type;
			dataStreamType >> type;

			QByteArray itemDataSubType = event->mimeData()->data("SubType");
			QDataStream dataStreamSubType(&itemDataSubType, QIODevice::ReadOnly);
			QString sub_type;
			dataStreamSubType >> sub_type;

			if (type == "EXEC") {
				if (sub_type == "Counter") {
					event->acceptProposedAction();
					auto node = new GUI::NODE::EXEC::Counter(drop_pos);
					active_node_tree->nodes.push_back(node);
					scene->addItem(node);
					return;
				}
				if (sub_type == "Script") {
					event->acceptProposedAction();
					auto node = new GUI::NODE::EXEC::Script(drop_pos);
					active_node_tree->nodes.push_back(node);
					scene->addItem(node);
					return;
				}
				if (sub_type == "Tick") {
					event->acceptProposedAction();
					auto node = new GUI::NODE::EXEC::Tick(drop_pos);
					active_node_tree->nodes.push_back(node);
					scene->addItem(node);
					return;
				}
			}
			if (type == "MATH") {
				if (sub_type == "Add") {
					event->acceptProposedAction();
					auto node = new GUI::NODE::MATH::Add(drop_pos);
					active_node_tree->nodes.push_back(node);
					scene->addItem(node);
					return;
				}
				if (sub_type == "Sub") {
					event->acceptProposedAction();
					auto node = new GUI::NODE::MATH::Sub(drop_pos);
					active_node_tree->nodes.push_back(node);
					scene->addItem(node);
					return;
				}
				if (sub_type == "Mul") {
					event->acceptProposedAction();
					auto node = new GUI::NODE::MATH::Mul(drop_pos);
					active_node_tree->nodes.push_back(node);
					scene->addItem(node);
					return;
				}
				if (sub_type == "Div") {
					event->acceptProposedAction();
					auto node = new GUI::NODE::MATH::Div(drop_pos);
					active_node_tree->nodes.push_back(node);
					scene->addItem(node);
					return;
				}
			}
			if (type == "LINK") {
				if (sub_type == "SET_Transform_Euler_Rot_X") {
					event->acceptProposedAction();
					auto node = new GUI::NODE::LINK::SET::Euler_Rotation_X(drop_pos);
					active_node_tree->nodes.push_back(node);
					scene->addItem(node);
					return;
				}
			}
		}
		else if (event->mimeData()->text() == "OBJECT") {
			event->acceptProposedAction();

			QByteArray byteArray = event->mimeData()->data("OBJECT");
			QDataStream stream(&byteArray, QIODevice::ReadOnly);
			qulonglong ptr;
			stream >> ptr;
			
			auto object = reinterpret_cast<CLASS::Object*>(ptr);

			auto node = new GUI::NODE::LINK::Pointer(drop_pos);
			node->pointer = object;
			node->pointer_type = CLASS::NODE::DATA::Type::OBJECT;
			active_node_tree->nodes.push_back(node);
			scene->addItem(node);
			return;
		}
	}
}

GUI::WORKSPACE::Node_Shelf::Node_Shelf(Workspace_Node_Editor* parent) :
	Tree(parent)
{
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);

	auto tree_constraint = new Tree_Item(this, "Constraint");
	auto tree_generate   = new Tree_Item(this, "Generate");
	auto tree_physics    = new Tree_Item(this, "Physics");
	auto tree_modify     = new Tree_Item(this, "Modify");
	auto tree_exec       = new Tree_Item(this, "Exec");
	auto tree_math       = new Tree_Item(this, "Math");
	auto tree_link       = new Tree_Item(this, "Link");
	auto tree_util       = new Tree_Item(this, "Util");

	new Tree_Item(tree_exec, "Sequence", 1, { { 1000, "EXEC" }, { 1001, "Sequence" } });
	new Tree_Item(tree_exec, "Counter",  1, { { 1000, "EXEC" }, { 1001, "Counter" } });
	new Tree_Item(tree_exec, "Script",   1, { { 1000, "EXEC" }, { 1001, "Script" } });
	new Tree_Item(tree_exec, "Start",    1, { { 1000, "EXEC" }, { 1001, "Start" } });
	new Tree_Item(tree_exec, "Timer",    1, { { 1000, "EXEC" }, { 1001, "Timer" } });
	new Tree_Item(tree_exec, "Tick",     1, { { 1000, "EXEC" }, { 1001, "Tick" } });
	new Tree_Item(tree_exec, "For",      1, { { 1000, "EXEC" }, { 1001, "For" } });
	
	new Tree_Item(tree_math, "Add",  1, { { 1000, "MATH" }, { 1001, "Add" } });
	new Tree_Item(tree_math, "Sub",  1, { { 1000, "MATH" }, { 1001, "Sub" } });
	new Tree_Item(tree_math, "Mul",  1, { { 1000, "MATH" }, { 1001, "Mul" } });
	new Tree_Item(tree_math, "Div",  1, { { 1000, "MATH" }, { 1001, "Div" } });

	auto tree_link_set                       = new Tree_Item(tree_link                        , "Set"           , 1, { { 1000, "LINK" }, { 1001, "SET_Field" } });
	auto tree_link_set_transform             = new Tree_Item(tree_link_set                    , "Transform"     , 2, { { 1000, "LINK" }, { 1001, "SET_Transform" } });
	auto tree_link_set_transform_euler_rot   = new Tree_Item(tree_link_set_transform          , "Euler Rotation", 3, { { 1000, "LINK" }, { 1001, "SET_Transform_Euler_Rot" } });
	auto tree_link_set_transform_euler_rot_x = new Tree_Item(tree_link_set_transform_euler_rot, "X"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Euler_Rot_X" } });
}

void GUI::WORKSPACE::Node_Viewport::dragMoveEvent(QDragMoveEvent* event) {
	event->acceptProposedAction();
}

void GUI::WORKSPACE::Node_Shelf::startDrag(Qt::DropActions actions) {
	if (QTreeWidgetItem* temp = currentItem()) {

		QMimeData* mimeData = new QMimeData;
		mimeData->setText("NODE");

		QByteArray type;
		QDataStream dataStreamType(&type, QIODevice::WriteOnly);
		dataStreamType << temp->data(0, 1000).toString();
		mimeData->setData("Type", type);
		
		QByteArray subtype;
		QDataStream dataStreamSubtype(&subtype, QIODevice::WriteOnly);
		dataStreamSubtype << temp->data(0, 1001).toString();
		mimeData->setData("SubType", subtype);

		QDrag* drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->exec(Qt::MoveAction);
	}
}