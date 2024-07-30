#include "Workspaces/Node_Editor.hpp"

#include "Workspaces/Manager.hpp"
#include "Core/Compiler.hpp"

GUI::WORKSPACE::Workspace_Node_Editor::Workspace_Node_Editor(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	parent->setMaximumWidth(800);
	viewport = new Node_Viewport(this);
	shelf = new Node_Shelf(this);

	auto splitter = new GUI::Splitter(this);
	splitter->addWidget(shelf);
	splitter->addWidget(viewport);

	auto header = new GUI::Linear_Contents(this);
	auto compile = new GUI::Button(this);
	compile->setText("Compile");
	header->addWidget(compile);

	auto load_in = new GUI::Button(this);
	load_in->setText("Load To View");
	header->addWidget(load_in);

	addWidget(header);
	addWidget(splitter);

	FILE->active_object->addCallback(this, [this]() { viewport->f_objectChanged(FILE->active_object->ptr); });
	connect(load_in, &GUI::Button::pressed, [this]() {
		viewport->loadNodes();
	});
	connect(compile, &GUI::Button::pressed, [this]() { // CAN CRASH. [xmemory 1335 and exec()] STOP Executing nodes before
		if (viewport->active_node_tree and FILE->active_object->ptr) {
			*LOG << ENDL << HTML_MAGENTA << "[DLL Compilation]" << HTML_RESET << " Compiling Solution..."; FLUSH

			auto node_tree = FILE->active_object->ptr->node_tree;
			auto gui_node_tree = FILE->nodetree_map[node_tree];
			
			HINSTANCE dynlib;
			recompileDLL(dynlib);

			for (auto node : node_tree->nodes) {
				if (node->type == CLASS::NODE::Type::EXEC and node->sub_type == e_to_u(CLASS::NODE::EXEC::Type::SCRIPT)) {
					static_cast<CLASS::NODE::EXEC::Script*>(node)->recompile(dynlib);
				}
			}
			for (auto node : gui_node_tree->nodes) {
				if (node->type == CLASS::NODE::Type::EXEC and node->sub_type == e_to_u(CLASS::NODE::EXEC::Type::SCRIPT)) {
					static_cast<GUI::NODE::EXEC::Script*>(node)->recompile(dynlib);
				}
			}

			*LOG << ENDL << HTML_GREEN << "[DLL Compilation]" << HTML_RESET << " Compiled Solution"; FLUSH
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
	selecting = false;
	connecting = false;
	connection = nullptr;
	selection = {};
	active_node_tree = nullptr;
	scene = new QGraphicsScene(this);
	selection_rect = new QGraphicsRectItem(QRectF(0, 0, 0, 0));
	selection_rect->setBrush(QColor(255, 135, 25, 50));
	selection_rect->setPen(QPen(QColor(255, 135, 25, 200), 2.5));
	selection_rect->setZValue(10);
	selection_rect->hide();

	setScene(scene);
	scene->addItem(selection_rect);
	setSceneRect(-10000, -10000, 20000, 20000);
}

GUI::WORKSPACE::Node_Viewport::~Node_Viewport() {
	for (auto item : scene->items()) {
		if (dynamic_cast<GUI::NODE::Node*>(item)) {
			scene->removeItem(item);
		}
	}
	FILE->active_object->callbacks.erase(this);

	delete connection;
	delete selection_rect;
}

void GUI::WORKSPACE::Node_Viewport::f_objectChanged(CLASS::Object* object) {
	for (auto item: scene->items()) {
		if (dynamic_cast<GUI::NODE::Node*>(item)) {
			scene->removeItem(item);
		}
	}
	if (object and object->node_tree) {
		active_node_tree = FILE->nodetree_map[object->node_tree];
		for (auto node : FILE->nodetree_map[object->node_tree]->nodes) {
			scene->addItem(node);
			node->setPos(node->load_pos);
		}
	}
	else active_node_tree = nullptr;
}

void GUI::WORKSPACE::Node_Viewport::loadNodes() {
	if (active_node_tree and FILE->active_object->ptr) {
		*LOG << ENDL << HTML_MAGENTA << "[Translation]" << HTML_RESET << " Compiling Nodes..."; FLUSH

		auto ptr = FILE->active_object->ptr->node_tree;
		for (auto node : ptr->nodes) {
			auto it = FILE->node_map.find(node);
			if (it != FILE->node_map.end()) {
				FILE->node_map.erase(it);
			}
		}
		auto it = FILE->nodetree_map.find(ptr);
		if (it != FILE->nodetree_map.end()) {
			FILE->nodetree_map.erase(it);
		}
		FILE->node_trees.erase(std::find(FILE->node_trees.begin(), FILE->node_trees.end(), ptr));

		auto node_tree = new CLASS::Node_Tree(active_node_tree);
		FILE->active_object->ptr->node_tree = node_tree;
		FILE->node_trees.push_back(node_tree);
		FILE->nodetree_map[node_tree] = active_node_tree;

		// TODO memory leak?
		// delete ptr; // WILL CRASH ON SECOND ATTEMPT. [xmemory 1335] STOP Executing nodes before
		#ifdef LOG1
			cout << endl << "NT Map Size: " << FILE->nodetree_map.size();
			cout << endl << "N  Map Size: " << FILE->node_map.size();
			cout << endl << "NT Vec Size: " << FILE->node_trees.size();
		#endif

		*LOG << ENDL << HTML_GREEN << "[Translation]" << HTML_RESET << " Compiled Nodes"; FLUSH
	}
}

void GUI::WORKSPACE::Node_Viewport::drawBackground(QPainter* painter, const QRectF& rect) {
	QGraphicsView::drawBackground(painter, rect);
	if (view_scale > 0.7) {
		painter->setPen(QPen(QColor(60, 60, 60), 1));
		const int gridSize = 10;
		const int xOffset = int(rect.left()) - (int(rect.left()) % gridSize);
		const int yOffset = int(rect.top()) - (int(rect.top()) % gridSize);

		for (int x = xOffset; x < int(rect.right()); x += gridSize) {
			painter->drawLine(x, int(rect.top()), x, int(rect.bottom()));
		}
		for (int y = yOffset; y < int(rect.bottom()); y += gridSize) {
			painter->drawLine(int(rect.left()), y, int(rect.right()), y);
		}
	}
	else if (view_scale > 0.35) {
		painter->setPen(QPen(QColor(60, 60, 60), 3));
		const int gridSize = 40;
		const int xOffset = int(rect.left()) - (int(rect.left()) % gridSize);
		const int yOffset = int(rect.top()) - (int(rect.top()) % gridSize);

		for (int x = xOffset; x < int(rect.right()); x += gridSize) {
			painter->drawLine(x, int(rect.top()), x, int(rect.bottom()));
		}
		for (int y = yOffset; y < int(rect.bottom()); y += gridSize) {
			painter->drawLine(int(rect.left()), y, int(rect.right()), y);
		}
	}
	else {
		painter->setPen(QPen(QColor(60, 60, 60), 5));
		const int gridSize = 80;
		const int xOffset = int(rect.left()) - (int(rect.left()) % gridSize);
		const int yOffset = int(rect.top()) - (int(rect.top()) % gridSize);

		for (int x = xOffset; x < int(rect.right()); x += gridSize) {
			painter->drawLine(x, int(rect.top()), x, int(rect.bottom()));
		}
		for (int y = yOffset; y < int(rect.bottom()); y += gridSize) {
			painter->drawLine(int(rect.left()), y, int(rect.right()), y);
		}
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
		moving = false;
		if (selecting) {
			for (auto item : scene->items(selection_rect->rect())) {
				if (GUI::NODE::Node* node = dynamic_cast<GUI::NODE::Node*>(item)) {
					if (!node->isSelected()) {
						node->setSelected(true);
						node->real_pos = node->pos();
						selection.push_back(node);
					}
				}
			}
			selecting = false;
			selection_rect->hide();
		}
		if (connecting) {
			if (auto item = scene->itemAt(mapToScene(event->pos()), transform())) {
				if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>(item)) {
					if (auto source_port = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(connection->port_l)) {
						if (drop_port->connection) {
							delete drop_port->connection;
						}
						drop_port->connection = new GUI::NODE::Connection(source_port, drop_port);
						source_port->outgoing_connections.push_back(drop_port->connection);
						loadNodes();
					}
				}
				else if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(item)) {
					if (auto source_port = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>(connection->port_l)) {
						if (source_port->connection) {
							delete source_port->connection;
						}
						source_port->connection = new GUI::NODE::Connection(drop_port, source_port);
						drop_port->outgoing_connections.push_back(source_port->connection);
						loadNodes();
					}
				}
				else if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(item)) {
					if (auto source_port = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>(connection->port_l)) {
						if (source_port->connection) {
							delete source_port->connection;
						}
						source_port->connection = new GUI::NODE::Connection(source_port, drop_port);
						drop_port->incoming_connections.push_back(source_port->connection);
						loadNodes();
					}
				}
				else if (auto drop_port = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>(item)) {
					if (auto source_port = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(connection->port_l)) {
						if (drop_port->connection) {
							delete drop_port->connection;
						}
						drop_port->connection = new GUI::NODE::Connection(drop_port, source_port);
						source_port->incoming_connections.push_back(drop_port->connection);
						loadNodes();
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
			else if (GUI::NODE::Node* node = dynamic_cast<GUI::NODE::Node*>(item)) {
				moving = true;
				if (event->modifiers() & Qt::KeyboardModifier::ShiftModifier) {
					if (!node->isSelected()) {
						node->setSelected(true);
						node->real_pos = node->pos();
						for (auto subnode: selection) {
							subnode->real_pos = subnode->pos();
						}
						selection.push_back(node);
					}
					else {
						node->setSelected(false);
					}
				}
				else {
					if (!node->isSelected()) {
						for (auto item : selection) {
							item->setSelected(false);
						}
						selection.clear();
						node->setSelected(true);
						node->real_pos = node->pos();
						selection.push_back(node);
					}
				}
				move_pos = mapToScene(event->pos());
			}
			else {
				for (auto item : selection) {
					item->setSelected(false);
				}
				selection.clear();
			}
		}
		else {
			for (auto item : selection) {
				item->setSelected(false);
			}
			selection.clear();

			selecting = true;
			selection_start = mapToScene(event->pos());
			selection_rect->setRect(QRectF(selection_start, QSizeF(0,0)));
			selection_rect->show();
		}
	}
}

void GUI::WORKSPACE::Node_Viewport::mouseMoveEvent(QMouseEvent* event) {
	if (event->modifiers() & Qt::KeyboardModifier::AltModifier) {
		if (auto item = scene->itemAt(mapToScene(event->pos()), transform())) {
			if (auto port_r = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>(item)) {
				if (port_r->connection) {
					auto port_l = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(port_r->connection->port_l);
					delete port_r->connection;
					loadNodes();
				}
			}
			else if (auto port_l = dynamic_cast<GUI::NODE::PORT::Data_O_Port*>(item)) {
				for (auto conn = port_l->outgoing_connections.begin(); conn != port_l->outgoing_connections.end();) {
					auto port_r = dynamic_cast<GUI::NODE::PORT::Data_I_Port*>((*conn)->port_r);
					delete port_r->connection;
					loadNodes();
				}
			}
			else if (auto port_r = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(item)) {
				for (auto conn = port_r->incoming_connections.begin(); conn != port_r->incoming_connections.end();) {
					auto port_l = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>((*conn)->port_l);
					delete port_l->connection;
					loadNodes();
				}
			}
			else if (auto port_l = dynamic_cast<GUI::NODE::PORT::Exec_O_Port*>(item)) {
				if (port_l->connection) {
					auto port_r = dynamic_cast<GUI::NODE::PORT::Exec_I_Port*>(port_l->connection->port_r);
					delete port_l->connection;
					loadNodes();
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
			for (auto& node : selection) {
				const QPointF delta = node->real_pos + mapToScene(event->pos()) - move_pos;
				node->setPos(f_roundToNearest(delta.x(), 10.0), f_roundToNearest(delta.y(), 10.0));
				node->real_pos = delta;
				node->load_pos = node->scenePos();
			}
			move_pos = mapToScene(event->pos());
		}
		if (selecting) {
			const QRectF newRect(selection_start, mapToScene(event->pos()));
			selection_rect->setRect(newRect.normalized());
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

void GUI::WORKSPACE::Node_Viewport::keyPressEvent(QKeyEvent* event) {
	Graphics_View::keyPressEvent(event);
	if (event->key() == Qt::Key::Key_Delete) {
		for (NODE::Node* node : selection) {
			scene->removeItem(node);
			active_node_tree->nodes.erase(std::find(active_node_tree->nodes.begin(), active_node_tree->nodes.end(), node));
			delete node;
		}
		selection.clear();
		loadNodes();
	}
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
	const ivec2 drop_pos = d_to_i(f_roundToNearest(p_to_d(mapToScene(event->position().toPoint())), 10.0));
	if (event->mimeData()->hasText() and active_node_tree) {
		GUI::NODE::Node* node = nullptr;

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
					node = new GUI::NODE::EXEC::Counter(drop_pos);
				}
				else if (sub_type == "Script") {
					node = new GUI::NODE::EXEC::Script(drop_pos);
				}
				else if (sub_type == "Tick") {
					node = new GUI::NODE::EXEC::Tick(drop_pos);
				}
			}
			else if (type == "MATH") {
				if (sub_type == "Add") {
					node = new GUI::NODE::MATH::Add(drop_pos);
				}
				else if (sub_type == "Sub") {
					node = new GUI::NODE::MATH::Sub(drop_pos);
				}
				else if (sub_type == "Mul") {
					node = new GUI::NODE::MATH::Mul(drop_pos);
				}
				else if (sub_type == "Div") {
					node = new GUI::NODE::MATH::Div(drop_pos);
				}
			}
			else if (type == "LINK") {
				if (sub_type == "LINK_POINTER_SCENE") {
					auto t_node = new GUI::NODE::LINK::Pointer(drop_pos, CLASS::NODE::DATA::Type::SCENE);
					t_node->pointer = FILE->active_scene->ptr;
					node = t_node;
				}
				else if (sub_type == "LINK_GET_FIELD") {
					node = new GUI::NODE::LINK::GET::Field(drop_pos);
				}
				else if (sub_type == "SET_Transform_Euler_Rot_X") {
					node = new GUI::NODE::LINK::SET::Euler_Rotation_X(drop_pos);
				}
			}
			else if (type == "UTIL") {
				if (sub_type == "Print") {
					node = new GUI::NODE::UTIL::Print(drop_pos);
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

			auto t_node = new GUI::NODE::LINK::Pointer(drop_pos, CLASS::NODE::DATA::Type::OBJECT);
			t_node->pointer = object;
			node = t_node;
			return;
		}
		if (node) {
			active_node_tree->nodes.push_back(node);
			scene->addItem(node);
			node->setPos(node->load_pos);
			event->acceptProposedAction();
		}
	}
}

GUI::WORKSPACE::Node_Shelf::Node_Shelf(Workspace_Node_Editor* parent) :
	Tree(parent)
{
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);

	auto tree_constraint = new Tree_Item(this, "Constraint");
	auto tree_generate   = new Tree_Item(this, "Generate"  );
	auto tree_physics    = new Tree_Item(this, "Physics"   );
	auto tree_modify     = new Tree_Item(this, "Modify"    );
	auto tree_exec       = new Tree_Item(this, "Exec"      );
	auto tree_math       = new Tree_Item(this, "Math"      );
	auto tree_link       = new Tree_Item(this, "Link"      );
	auto tree_util       = new Tree_Item(this, "Util"      );

	auto tree_exec_0     = new Tree_Item(tree_exec    , "Sequence"      , 1, { { 1000, "EXEC" }, { 1001, "Sequence"                  } });
	auto tree_exec_1     = new Tree_Item(tree_exec    , "Counter"       , 1, { { 1000, "EXEC" }, { 1001, "Counter"                   } });
	auto tree_exec_2     = new Tree_Item(tree_exec    , "Script"        , 1, { { 1000, "EXEC" }, { 1001, "Script"                    } });
	auto tree_exec_4     = new Tree_Item(tree_exec    , "Timer"         , 1, { { 1000, "EXEC" }, { 1001, "Timer"                     } });
	auto tree_exec_5     = new Tree_Item(tree_exec    , "Tick"          , 1, { { 1000, "EXEC" }, { 1001, "Tick"                      } });
	auto tree_exec_6     = new Tree_Item(tree_exec    , "For"           , 1, { { 1000, "EXEC" }, { 1001, "For"                       } });
	auto tree_exec_3     = new Tree_Item(tree_exec    , "Start"         , 1, { { 1000, "EXEC" }, { 1001, "Start"                     } });
	
	auto tree_math_0     = new Tree_Item(tree_math    , "Add"           , 1, { { 1000, "MATH" }, { 1001, "Add"                       } });
	auto tree_math_1     = new Tree_Item(tree_math    , "Sub"           , 1, { { 1000, "MATH" }, { 1001, "Sub"                       } });
	auto tree_math_2     = new Tree_Item(tree_math    , "Mul"           , 1, { { 1000, "MATH" }, { 1001, "Mul"                       } });
	auto tree_math_3     = new Tree_Item(tree_math    , "Div"           , 1, { { 1000, "MATH" }, { 1001, "Div"                       } });

	auto tree_link_0     = new Tree_Item(tree_link    , "Pointer"       , 1, { { 1000, "LINK" }, { 1001, "Pointer"                   } });
	auto tree_link_1     = new Tree_Item(tree_link    , "Set"           , 1, { { 1000, "LINK" }, { 1001, "SET_Field"                 } });
	auto tree_link_2     = new Tree_Item(tree_link    , "Get"           , 1, { { 1000, "LINK" }, { 1001, "GET_Field"                 } });

	auto tree_link_00    = new Tree_Item(tree_link_0  , "Scene"         , 1, { { 1000, "LINK" }, { 1001, "LINK_POINTER_SCENE"        } });

	auto tree_link_10    = new Tree_Item(tree_link_1  , "Field"         , 1, { { 1000, "LINK" }, { 1001, "SET_Field"                 } });
	auto tree_link_11    = new Tree_Item(tree_link_1  , "Transform"     , 2, { { 1000, "LINK" }, { 1001, "SET_Transform"             } });
	auto tree_link_110   = new Tree_Item(tree_link_11 , "Position"      , 3, { { 1000, "LINK" }, { 1001, "SET_Transform_Position"    } });
	auto tree_link_1100  = new Tree_Item(tree_link_110, "X"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Position_X"  } });
	auto tree_link_1101  = new Tree_Item(tree_link_110, "Y"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Position_Y"  } });
	auto tree_link_1102  = new Tree_Item(tree_link_110, "Z"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Position_Z"  } });
	auto tree_link_111   = new Tree_Item(tree_link_11 , "Euler Rotation", 3, { { 1000, "LINK" }, { 1001, "SET_Transform_Euler_Rot"   } });
	auto tree_link_1110  = new Tree_Item(tree_link_111, "X"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Euler_Rot_X" } });
	auto tree_link_1111  = new Tree_Item(tree_link_111, "Y"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Euler_Rot_Y" } });
	auto tree_link_1112  = new Tree_Item(tree_link_111, "Z"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Euler_Rot_Z" } });
	auto tree_link_112   = new Tree_Item(tree_link_11 , "Scale"         , 3, { { 1000, "LINK" }, { 1001, "SET_Transform_Scale"       } });
	auto tree_link_1120  = new Tree_Item(tree_link_112, "X"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Scale_X"     } });
	auto tree_link_1121  = new Tree_Item(tree_link_112, "Y"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Scale_Y"     } });
	auto tree_link_1122  = new Tree_Item(tree_link_112, "Z"             , 4, { { 1000, "LINK" }, { 1001, "SET_Transform_Scale_Z"     } });

	auto tree_link_20    = new Tree_Item(tree_link_2  , "Field"         , 1, { { 1000, "LINK" }, { 1001, "LINK_GET_FIELD"            } });
	auto tree_link_21    = new Tree_Item(tree_link_2  , "Transform"     , 2, { { 1000, "LINK" }, { 1001, "GET_Transform"             } });
	auto tree_link_210   = new Tree_Item(tree_link_21 , "Position"      , 3, { { 1000, "LINK" }, { 1001, "GET_Transform_Position"    } });
	auto tree_link_2100  = new Tree_Item(tree_link_210, "X"             , 4, { { 1000, "LINK" }, { 1001, "GET_Transform_Position_X"  } });
	auto tree_link_2101  = new Tree_Item(tree_link_210, "Y"             , 4, { { 1000, "LINK" }, { 1001, "GET_Transform_Position_Y"  } });
	auto tree_link_2102  = new Tree_Item(tree_link_210, "Z"             , 4, { { 1000, "LINK" }, { 1001, "GET_Transform_Position_Z"  } });
	auto tree_link_211   = new Tree_Item(tree_link_21 , "Euler Rotation", 3, { { 1000, "LINK" }, { 1001, "GET_Transform_Euler_Rot"   } });
	auto tree_link_2110  = new Tree_Item(tree_link_211, "X"             , 4, { { 1000, "LINK" }, { 1001, "GET_Transform_Euler_Rot_X" } });
	auto tree_link_2111  = new Tree_Item(tree_link_211, "Y"             , 4, { { 1000, "LINK" }, { 1001, "GET_Transform_Euler_Rot_Y" } });
	auto tree_link_2112  = new Tree_Item(tree_link_211, "Z"             , 4, { { 1000, "LINK" }, { 1001, "GET_Transform_Euler_Rot_Z" } });
	auto tree_link_212   = new Tree_Item(tree_link_21 , "Scale"         , 3, { { 1000, "LINK" }, { 1001, "GET_Transform_Scale"       } });
	auto tree_link_2120  = new Tree_Item(tree_link_212, "X"             , 4, { { 1000, "LINK" }, { 1001, "GET_Transform_Scale_X"     } });
	auto tree_link_2121  = new Tree_Item(tree_link_212, "Y"             , 4, { { 1000, "LINK" }, { 1001, "GET_Transform_Scale_Y"     } });
	auto tree_link_2122  = new Tree_Item(tree_link_212, "Z"             , 4, { { 1000, "LINK" }, { 1001, "GET_Transform_Scale_Z"     } });

	auto tree_util_0     = new Tree_Item(tree_util     ,"Print"         , 1, { { 1000, "UTIL" }, { 1001, "Print"                     } });
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