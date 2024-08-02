#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Ops.hpp"
#include "Core/File.hpp"
#include "Core/Session.hpp"

#include "Object/Nodes/Object_Nodes.hpp"

#include "GUI_Object_Node.hpp"
#include "GUI_Object_Nodes.hpp"

#include "Object/Object.hpp"

// FWD DECL OTHER
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;
	}
}

// FWD DECL THIS
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Node_Editor;
		struct Node_Viewport;
		struct Node_Shelf;

		struct Node;
	}
}

// DECL
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Node_Editor : GUI::Linear_Contents {
			Workspace_Manager* parent;

			Node_Viewport* viewport;
			Node_Shelf* shelf;

			Workspace_Node_Editor(Workspace_Manager* parent);
		};
		/* TODO:
			Allow Visualization of GUI::Node_Tree during execution
			CHECK Circular dependencies
			PREVENT connection between different data_types
			Alt-Drag deselects
			Alt-Click deselects
		*/
		struct Node_Viewport : GUI::Graphics_View {
			Workspace_Node_Editor* parent;
			QGraphicsScene* scene;

			bool pan;
			bool moving;
			bool selecting;
			bool connecting;

			QPointF pan_pos;
			QPointF move_pos;
			qreal  view_scale;
			QPointF selection_start;
			QGraphicsRectItem* selection_rect;
			vector<GUI::NODE::Node*> selection;

			GUI::NODE::Connection* connection;
			GUI::NODE::Node_Tree* active_node_tree;

			Node_Viewport(Workspace_Node_Editor* parent);
			~Node_Viewport();

			void f_objectChanged(CLASS::Object* object);
			void loadNodes();

			void drawBackground(QPainter* painter, const QRectF& rect) override;

			//void mouseDoubleClickEvent(QMouseEvent* event) override {}
			void mouseReleaseEvent(QMouseEvent* event) override;
			void mousePressEvent(QMouseEvent* event) override; // TODO Fix sticky mouse, double clicks and odd non-registering behaviors
			void mouseMoveEvent(QMouseEvent* event) override;
			void keyPressEvent(QKeyEvent* event) override;

			void dragMoveEvent(QDragMoveEvent* event) override;
			void resizeEvent(QResizeEvent* event) override;
			void wheelEvent(QWheelEvent* event) override;
			void dropEvent(QDropEvent* event) override;
		};

		struct Node_Shelf : GUI::Tree {
			Node_Shelf(Workspace_Node_Editor* parent);

			void startDrag(Qt::DropActions actions) override; // TODO fix QPixmap is Null
		};
	}
}