#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Core/Ops.hpp"
#include "Core/File.hpp"
#include "Core/Session.hpp"

#include "Core/Node_Def.hpp"

#include "Node_GUI.hpp"
#include "Node_GUI_Def.hpp"

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
		Also allow Visualization of GUI::Node_Tree during execution
		CHECK Circular dependencies
		PREVENT connection between different data_types;
		*/
		struct Node_Viewport : GUI::Graphics_View {
			Workspace_Node_Editor* parent;
			QGraphicsScene* scene;

			QPointF pan_pos;
			bool pan;

			bool moving;
			QPointF move_pos;
			GUI::NODE::Node* move_selection;

			bool connecting;
			GUI::NODE::Connection* connection;

			GUI::NODE::Node_Tree* active_node_tree;

			qreal view_scale;

			Node_Viewport(Workspace_Node_Editor* parent);
			~Node_Viewport();

			void f_objectChanged(CLASS::Object* object);

			void drawBackground(QPainter* painter, const QRectF& rect) override;
			void mouseReleaseEvent(QMouseEvent* event) override;
			void mousePressEvent(QMouseEvent* event) override;
			void mouseMoveEvent(QMouseEvent* event) override;
			void resizeEvent(QResizeEvent* event) override;
			void wheelEvent(QWheelEvent* event) override;

			void dragMoveEvent(QDragMoveEvent* event) override;
			void dropEvent(QDropEvent* event) override;
		};

		struct Node_Shelf : GUI::Tree {
			Node_Shelf(Workspace_Node_Editor* parent);

			void startDrag(Qt::DropActions actions) override; // TODO fix QPixmap is Null
		};
	}
}