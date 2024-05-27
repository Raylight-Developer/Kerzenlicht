#include "Window.hpp"

App::App(int argc, char* argv[]) :
	GUI::Application(argc, argv)
{
	Window* window = new Window();
	exec();
}

Window::Window() :
	GUI::Window()
{
	Node_Editor* editor = new Node_Editor(this);
	setCentralWidget(editor);
	showMaximized();
}

Node_Editor::Node_Editor(QWidget* parent) :
	GUI::Graphics_View(parent)
{
	view_scale = 1.0;
	pan = false;
	moving = false;
	connecting = false;
	scene = new QGraphicsScene(this);

	setSceneRect(-width(), -height(), width()*2, height()*2);
	setScene(scene);

	scene->addItem(new EXEC_Node(QPoint(-300, 0)));
	scene->addItem(new SEQUENCE_Node(QPoint(0, 0)));
	scene->addItem(new TICK_COUNTER_Node(QPoint(300,-200)));
	scene->addItem(new TICK_COUNTER_Node(QPoint(300,0)));
	scene->addItem(new TICK_COUNTER_Node(QPoint(300,200)));
	scene->addItem(new ADD_Node(QPoint(-900, 100)));
	scene->addItem(new SUB_Node(QPoint(-900, -100)));
	scene->addItem(new VIEW_Node(QPoint(-600, 0)));
	scene->addItem(new SCRIPT_Node(QPoint(0, -200)));
}

void Node_Editor::drawBackground(QPainter* painter, const QRectF& rect) {
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

void Node_Editor::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::MouseButton::MiddleButton) {
		pan = false;
	}
	if (event->button() == Qt::MouseButton::RightButton) {
		moving = false;
	}
	if (event->button() == Qt::MouseButton::LeftButton) {
		if (connecting) {
			QList<QGraphicsItem*> itemsUnderMouse = scene->items(mapToScene(event->pos()), Qt::ItemSelectionMode::IntersectsItemBoundingRect, Qt::SortOrder::DescendingOrder, transform());
			if (!itemsUnderMouse.isEmpty()) {
				QGraphicsItem* item = nullptr;
				qreal highestZ = MIN_DVEC1;
				for (QGraphicsItem* check_item : itemsUnderMouse) {
					if (check_item->zValue() > highestZ) {
						highestZ = check_item->zValue();
						item = check_item;
					}
				}
				if (item) {
					if (Data_I_Port* drop_port = dynamic_cast<Data_I_Port*>(item)) {
						if (Data_O_Port* source_port = dynamic_cast<Data_O_Port*>(connection->port_l)) {
							if (drop_port->connection) delete drop_port->connection;
							drop_port->connection = new Connection(drop_port, source_port);
							source_port->outgoing_connections.push_back(drop_port->connection);
						}
					}
					else if (Data_O_Port* drop_port = dynamic_cast<Data_O_Port*>(item)) {
						if (Data_I_Port* source_port = dynamic_cast<Data_I_Port*>(connection->port_l)) {
							if (source_port->connection) delete source_port->connection;
							source_port->connection = new Connection(source_port, drop_port);
							drop_port->outgoing_connections.push_back(source_port->connection);
						}
					}
					else if (Exec_I_Port* drop_port = dynamic_cast<Exec_I_Port*>(item)) {
						if (Exec_O_Port* source_port = dynamic_cast<Exec_O_Port*>(connection->port_l)) {
							if (source_port->connection) delete source_port->connection;
							source_port->connection = new Connection(drop_port, source_port);
							drop_port->incoming_connections.push_back(source_port->connection);
						}
					}
					else if (Exec_O_Port* drop_port = dynamic_cast<Exec_O_Port*>(item)) {
						if (Exec_I_Port* source_port = dynamic_cast<Exec_I_Port*>(connection->port_l)) {
							if (drop_port->connection) delete drop_port->connection;
							drop_port->connection = new Connection(source_port, drop_port);
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

void Node_Editor::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::MouseButton::MiddleButton) {
		pan = true;
		pan_pos = event->pos();
	}
	if (event->button() == Qt::MouseButton::LeftButton) {
		QList<QGraphicsItem*> itemsUnderMouse = scene->items(mapToScene(event->pos()), Qt::ItemSelectionMode::IntersectsItemBoundingRect, Qt::SortOrder::DescendingOrder, transform());
		if (!itemsUnderMouse.isEmpty()) {
			QGraphicsItem* item = nullptr;
			qreal highestZ = MIN_DVEC1;
			for (QGraphicsItem* check_item : itemsUnderMouse) {
				if (check_item->zValue() > highestZ) {
					highestZ = check_item->zValue();
					item = check_item;
				}
			}
			if (item) {
				if      (Data_I_Port* port = dynamic_cast<Data_I_Port*>(item)) {
					connecting = true;
					connection = new Connection(port);
				}
				else if (Data_O_Port* port = dynamic_cast<Data_O_Port*>(item)) {
					connecting = true;
					connection = new Connection(port);
				}
				else if (Exec_I_Port* port = dynamic_cast<Exec_I_Port*>(item)) {
					connecting = true;
					connection = new Connection(port);
				}
				else if (Exec_O_Port* port = dynamic_cast<Exec_O_Port*>(item)) {
					connecting = true;
					connection = new Connection(port);
				}
			}
		}
	}
	if (event->button() == Qt::MouseButton::RightButton) {
		QList<QGraphicsItem*> itemsUnderMouse = scene->items(mapToScene(event->pos()), Qt::ItemSelectionMode::IntersectsItemBoundingRect, Qt::SortOrder::DescendingOrder, transform());
		if (!itemsUnderMouse.isEmpty()) {
			QGraphicsItem* item = nullptr;
			qreal highestZ = MIN_DVEC1;
			for (QGraphicsItem* check_item : itemsUnderMouse) {
				if (check_item->zValue() > highestZ) {
					highestZ = check_item->zValue();
					item = check_item;
				}
			}
			if (item) {
				if (Node* node = dynamic_cast<Node*>(item)) {
					moving = true;
					move_selection = node;
					move_pos = mapToScene(event->pos()) - move_selection->scenePos();
				}
				else if (Data_I_Port* port = dynamic_cast<Data_I_Port*>(item)) {
					moving = true;
					move_selection = port->node;
					move_pos = mapToScene(event->pos()) - move_selection->scenePos();
				}
				else if (Data_O_Port* port = dynamic_cast<Data_O_Port*>(item)) {
					moving = true;
					move_selection = port->node;
					move_pos = mapToScene(event->pos()) - move_selection->scenePos();
				}
				else if (Exec_I_Port* port = dynamic_cast<Exec_I_Port*>(item)) {
					moving = true;
					move_selection = port->node;
					move_pos = mapToScene(event->pos()) - move_selection->scenePos();
				}
				else if (Exec_O_Port* port = dynamic_cast<Exec_O_Port*>(item)) {
					moving = true;
					move_selection = port->node;
					move_pos = mapToScene(event->pos()) - move_selection->scenePos();
				}
			}
		}
	}
	QGraphicsView::mousePressEvent(event);
}

void Node_Editor::mouseMoveEvent(QMouseEvent* event) {
	if (event->modifiers() & Qt::KeyboardModifier::AltModifier) {
		QList<QGraphicsItem*> itemsUnderMouse = scene->items(mapToScene(event->pos()), Qt::ItemSelectionMode::IntersectsItemBoundingRect, Qt::SortOrder::DescendingOrder, transform());
		if (!itemsUnderMouse.isEmpty()) {
			QGraphicsItem* item = nullptr;
			qreal highestZ = MIN_DVEC1;
			for (QGraphicsItem* check_item : itemsUnderMouse) {
				if (check_item->zValue() > highestZ) {
					highestZ = check_item->zValue();
					item = check_item;
				}
			}
			if (item) {
				if (Data_I_Port* port = dynamic_cast<Data_I_Port*>(item)) {
					if (port->connection) {
						Data_O_Port* oport = dynamic_cast<Data_O_Port*>(port->connection->port_r);
						oport->outgoing_connections.erase(std::find(oport->outgoing_connections.begin(), oport->outgoing_connections.end(), port->connection));
						delete port->connection;
					}
					port->connection = nullptr;
				}
				else if (Data_O_Port* port = dynamic_cast<Data_O_Port*>(item)) {
					for (auto conn = port->outgoing_connections.begin(); conn != port->outgoing_connections.end();) {
						Data_I_Port* i_port = dynamic_cast<Data_I_Port*>((*conn)->port_l);
						delete i_port->connection;
						i_port->connection = nullptr;
						conn = port->outgoing_connections.erase(conn);
					}
				}
				else if (Exec_I_Port* port = dynamic_cast<Exec_I_Port*>(item)) {
					for (auto conn = port->incoming_connections.begin(); conn != port->incoming_connections.end();) {
						Exec_O_Port* o_port = dynamic_cast<Exec_O_Port*>((*conn)->port_r);
						delete o_port->connection;
						o_port->connection = nullptr;
						conn = port->incoming_connections.erase(conn);
					}
				}
				else if (Exec_O_Port* port = dynamic_cast<Exec_O_Port*>(item)) {
					if (port->connection) {
						Exec_I_Port* iport = dynamic_cast<Exec_I_Port*>(port->connection->port_l);
						iport->incoming_connections.erase(std::find(iport->incoming_connections.begin(), iport->incoming_connections.end(), port->connection));
						delete port->connection;
					}
					port->connection = nullptr;
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
			move_selection->setPos(roundToNearest(delta.x(), 10.0), roundToNearest(delta.y(), 10.0));
		}
		if (connecting) {
			if (connection->port_l) {
				connection->updateStart(mapToScene(event->pos()));
				connection->updateEnd();
			}
			else {
				connection->updateEnd(mapToScene(event->pos()));
				connection->updateStart();
			}
			connection->update();
		}
	}
	QGraphicsView::mouseMoveEvent(event);
}

void Node_Editor::wheelEvent(QWheelEvent* event) {
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

	QGraphicsView::wheelEvent(event);
}

void Node_Editor::resizeEvent(QResizeEvent* event) {
	GUI::Graphics_View::resizeEvent(event);
	setSceneRect(-width(), -height(), width() * 2, height() * 2);
}