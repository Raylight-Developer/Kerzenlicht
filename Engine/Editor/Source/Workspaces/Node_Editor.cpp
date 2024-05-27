#include "Workspaces/Node_Editor.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_Node_Editor::Workspace_Node_Editor(Workspace_Manager* i_parent) :
	GUI::Linear_Contents(i_parent, QBoxLayout::Direction::TopToBottom),
	parent(i_parent),
	file(parent->file)
{
	parent->setMaximumWidth(400);

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

	parent->file->active_object->addCallback(this, [this]() { viewport->f_objectChanged(file->active_object->ptr); });
	connect(compile, &GUI::Button::pressed, [this]() {
		if (viewport->active_node_tree and file->active_object->ptr) {
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
			cout << "Compiled Nodes" << endl;
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
	for (auto item : scene->items()) if (dynamic_cast<GUI::NODE::Node*>(item)) scene->removeItem(item);
	parent->file->active_object->callbacks.erase(this);
}

void GUI::WORKSPACE::Node_Viewport::f_objectChanged(CLASS::Object* object) {
	for (auto item: scene->items()) if (dynamic_cast<GUI::NODE::Node*>(item)) scene->removeItem(item);
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
	if (event->button() == Qt::MouseButton::MiddleButton) {
		pan = false;
	}
	if (event->button() == Qt::MouseButton::RightButton) {
		moving = false;
	}
	if (event->button() == Qt::MouseButton::LeftButton) {
		if (connecting) {
			if (auto item = scene->itemAt(mapToScene(event->pos()), transform())) {
				if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>(item)) {
					if (!drop_port->connection) {
						if (auto source_port = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(connection->port_l)) {
							if (drop_port->connection) delete drop_port->connection;
							drop_port->connection = new GUI::NODE::Connection(source_port, drop_port);
							source_port->outgoing_connections.push_back(drop_port->connection);
							drop_port->onConnect(drop_port->connection);
							source_port->onConnect(drop_port->connection);
						}
					}
				}
				else if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(item)) {
					bool exists = false;
					for (auto conn : drop_port->outgoing_connections) {
						if (conn->port_l == connection->port_l)
							exists = true;
					}
					if (!exists) {
						if (auto source_port = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>(connection->port_l)) {
							if (source_port->connection) delete source_port->connection;
							source_port->connection = new GUI::NODE::Connection(drop_port, source_port);
							drop_port->outgoing_connections.push_back(source_port->connection);
							drop_port->onConnect(source_port->connection);
							source_port->onConnect(source_port->connection);
						}
					}
				}
				else if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(item)) {
					bool exists = false;
					for (auto conn : drop_port->incoming_connections) {
						if (conn->port_l == connection->port_l)
							exists = true;
					}
					if (!exists) {
						if (auto source_port = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>(connection->port_l)) {
							if (source_port->connection) delete source_port->connection;
							source_port->connection = new GUI::NODE::Connection(source_port, drop_port);
							drop_port->incoming_connections.push_back(source_port->connection);
							drop_port->onConnect(source_port->connection);
							source_port->onConnect(source_port->connection);
						}
					}
				}
				else if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>(item)) {
					if (!drop_port->connection) {
						if (auto source_port = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(connection->port_l)) {
							if (drop_port->connection) delete drop_port->connection;
							drop_port->connection = new GUI::NODE::Connection(drop_port, source_port);
							source_port->incoming_connections.push_back(drop_port->connection);
							drop_port->onConnect(drop_port->connection);
							source_port->onConnect(drop_port->connection);
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
					port_l->node->onPortDisconnect(port_l);
					port_r->node->onPortDisconnect(port_r);
				}
			}
			else if (auto port_l = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(item)) {
				for (auto conn = port_l->outgoing_connections.begin(); conn != port_l->outgoing_connections.end();) {
					auto port_r = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>((*conn)->port_r);
					conn = port_l->outgoing_connections.erase(conn);
		
					delete port_r->connection;
					port_r->connection = nullptr;
					port_l->node->onPortDisconnect(port_l);
					port_r->node->onPortDisconnect(port_r);
				}
			}
			else if (auto port_r = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(item)) {
				for (auto conn = port_r->incoming_connections.begin(); conn != port_r->incoming_connections.end();) {
					auto port_l = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>((*conn)->port_l);
					conn = port_r->incoming_connections.erase(conn);
		
					delete port_l->connection;
					port_l->connection = nullptr;
					port_l->node->onPortDisconnect(port_l);
					port_r->node->onPortDisconnect(port_r);
				}
			}
			else if (auto port_l = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>(item)) {
				if (port_l->connection) {
					auto port_r = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(port_l->connection->port_r);
					port_r->incoming_connections.erase(std::find(port_r->incoming_connections.begin(), port_r->incoming_connections.end(), port_l->connection));
		
					delete port_l->connection;
					port_l->connection = nullptr;
					port_l->node->onPortDisconnect(port_l);
					port_r->node->onPortDisconnect(port_r);
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
			if (connection->port_l) {
				connection->updateR(mapToScene(event->pos()));
			}
			else {
				connection->updateL(mapToScene(event->pos()));
			}
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
void GUI::WORKSPACE::Node_Viewport::dragMoveEvent(QDragMoveEvent* event) {
	if (event->mimeData()->hasFormat("NODE")) {
		event->acceptProposedAction();
	}
}

void GUI::WORKSPACE::Node_Viewport::dropEvent(QDropEvent* event) {
	if (event->mimeData()->hasText() and active_node_tree) {
		if (event->mimeData()->text() == "NODE") {
			QByteArray itemData = event->mimeData()->data("NODE");
			QDataStream dataStream(&itemData, QIODevice::ReadOnly);
			QString text;
			dataStream >> text;

			const dvec2 drop_pos = f_roundToNearest(p_to_d(mapToScene(event->position().toPoint())), 10.0);

			if (text == "Tick") {
				event->acceptProposedAction();
				auto node = new GUI::NODE::EXEC::Tick(drop_pos);
				active_node_tree->nodes.push_back(node);
				scene->addItem(node);
			}
			if (text == "Add") {
				event->acceptProposedAction();
				auto node = new GUI::NODE::MATH::Add(drop_pos);
				active_node_tree->nodes.push_back(node);
				scene->addItem(node);
			}
			if (text == "Sub") {
				event->acceptProposedAction();
				auto node = new GUI::NODE::MATH::Sub(drop_pos);
				active_node_tree->nodes.push_back(node);
				scene->addItem(node);
			}
			if (text == "Mul") {
				event->acceptProposedAction();
				auto node = new GUI::NODE::MATH::Mul(drop_pos);
				active_node_tree->nodes.push_back(node);
				scene->addItem(node);
			}
			if (text == "Div") {
				event->acceptProposedAction();
				auto node = new GUI::NODE::MATH::Div(drop_pos);
				active_node_tree->nodes.push_back(node);
				scene->addItem(node);
			}
		}
		else if (event->mimeData()->text() == "OBJECT") {
			QByteArray byteArray = event->mimeData()->data("OBJECT");
			QDataStream stream(&byteArray, QIODevice::ReadOnly);
			qulonglong ptr;
			stream >> ptr;
			
			auto object = reinterpret_cast<CLASS::Object*>(ptr);
			cout << object->name << endl;
		}
	}
}

GUI::WORKSPACE::Node_Shelf::Node_Shelf(Workspace_Node_Editor* parent) :
	Tree(parent)
{
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDropMode::DragOnly);

	setMinimumWidth(100);
	setMaximumWidth(200);

	
	auto tree_constraint = new Tree_Item(this, "Constraint");
	auto tree_generate   = new Tree_Item(this, "Generate");
	auto tree_physics    = new Tree_Item(this, "Physics");
	auto tree_modify     = new Tree_Item(this, "Modify");
	auto tree_exec       = new Tree_Item(this, "Exec");
	auto tree_math       = new Tree_Item(this, "Math");
	auto tree_link       = new Tree_Item(this, "Link");
	auto tree_util       = new Tree_Item(this, "Util");

	auto exec_sequence = new Tree_Item(tree_exec, "Sequence", 1);
	auto exec_counter  = new Tree_Item(tree_exec, "Counter",  1);
	auto exec_script   = new Tree_Item(tree_exec, "Script",   1);
	auto exec_start    = new Tree_Item(tree_exec, "Start",    1);
	auto exec_timer    = new Tree_Item(tree_exec, "Timer",    1);
	auto exec_tick     = new Tree_Item(tree_exec, "Tick",     1);
	auto exec_for      = new Tree_Item(tree_exec, "For",      1);
	
	auto math_add  = new Tree_Item(tree_math, "Add",  1);
	auto math_sub  = new Tree_Item(tree_math, "Sub",  1);
	auto math_mul  = new Tree_Item(tree_math, "Mul",  1);
	auto math_div  = new Tree_Item(tree_math, "Div",  1);
	auto math_pow  = new Tree_Item(tree_math, "Pow",  1);
	auto math_sqrt = new Tree_Item(tree_math, "Sqrt", 1);
}

void GUI::WORKSPACE::Node_Shelf::startDrag(Qt::DropActions actions) {
	QTreeWidgetItem* temp = currentItem();
	if (temp) {
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << temp->text(0);

		QMimeData* mimeData = new QMimeData;
		mimeData->setText("NODE");
		mimeData->setData("NODE", itemData);

		QDrag* drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->exec(Qt::MoveAction);
	}
}