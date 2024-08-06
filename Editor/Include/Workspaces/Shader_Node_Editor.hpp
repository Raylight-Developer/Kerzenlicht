#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Ops.hpp"
#include "Core/File.hpp"
#include "Core/Session.hpp"

#include "Shading/Nodes/Shader_Nodes.hpp"

#include "GUI_Nodes.hpp"
#include "GUI_Shader_Nodes.hpp"

// FWD DECL OTHER
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;
	}
}

// FWD DECL THIS
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Shader_Node_Editor;
		struct Shader_Node_Viewport;
		struct Shader_Node_Shelf;
	}
}

// DECL
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Shader_Node_Editor : GUI::Linear_Contents {
			Workspace_Manager* parent;

			Shader_Node_Viewport* viewport;
			Shader_Node_Shelf* shelf;

			Workspace_Shader_Node_Editor(Workspace_Manager* parent);
		};
		/* TODO:
			Allow Visualization of GUI::Node_Tree during execution
			CHECK Circular dependencies
			PREVENT connection between different data_types
			Alt-Drag deselects
			Alt-Click deselects
		*/
		struct Shader_Node_Viewport : GUI::Graphics_View {
			Workspace_Shader_Node_Editor* parent;
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

			Shader_Node_Viewport(Workspace_Shader_Node_Editor* parent);
			~Shader_Node_Viewport();

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

		struct Shader_Node_Shelf : GUI::Tree {
			Shader_Node_Shelf(Workspace_Shader_Node_Editor* parent);

			void startDrag(Qt::DropActions actions) override; // TODO fix QPixmap is Null
		};
	}
}