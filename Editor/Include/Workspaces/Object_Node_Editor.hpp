#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/Editor_File.hpp"

#include "Object/Nodes/Object_Nodes.hpp"

#include "GUI_Nodes.hpp"
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
		struct Workspace_Object_Node_Editor;
		struct Object_Node_Viewport;
		struct Object_Node_Shelf;
	}
}

// DECL
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Object_Node_Editor : GUI::Linear_Contents {
			Workspace_Manager* parent;

			Object_Node_Viewport* viewport;
			Object_Node_Shelf* shelf;

			Workspace_Object_Node_Editor(Workspace_Manager* parent);
		};
		/* TODO:
			Allow Visualization of GUI::Node_Tree during execution
			CHECK Circular dependencies
			PREVENT connection between different data_types
			Alt-Drag deselects
			Alt-Click deselects
		*/
		struct Object_Node_Viewport : GUI::Graphics_View {
			Workspace_Object_Node_Editor* parent;
			QGraphicsScene* scene;

			bool moving;
			bool selecting;
			bool connecting;

			QPointF move_pos;
			QPointF selection_start;
			QGraphicsRectItem* selection_rect;
			vector<GUI::NODE::Node*> selection;

			GUI::NODE::Connection* connection;
			GUI::NODE::Node_Tree* active_node_tree;

			Object_Node_Viewport(Workspace_Object_Node_Editor* parent);
			~Object_Node_Viewport();

			void f_objectChanged(KL::Object* object);
			void loadNodes();

			void drawBackground(QPainter* painter, const QRectF& rect) override;

			//void mouseDoubleClickEvent(QMouseEvent* event) override {}
			void mouseReleaseEvent(QMouseEvent* event) override;
			void mousePressEvent(QMouseEvent* event) override; // TODO Fix sticky mouse, double clicks and odd non-registering behaviors
			void mouseMoveEvent(QMouseEvent* event) override;
			void keyPressEvent(QKeyEvent* event) override;

			void dragMoveEvent(QDragMoveEvent* event) override;
			//void resizeEvent(QResizeEvent* event) override;
			//void wheelEvent(QWheelEvent* event) override;
			void dropEvent(QDropEvent* event) override;
		};

		struct Object_Node_Shelf : GUI::Tree {
			Object_Node_Shelf(Workspace_Object_Node_Editor* parent);

			void startDrag(Qt::DropActions actions) override; // TODO fix QPixmap is Null
		};
	}
}