#pragma once

#include "Shared.hpp"

#include "QT.hpp"

#include "Core/Editor_File.hpp"

#include "Shader/Nodes/Shader_Nodes.hpp"

//#include "GUI_Nodes.hpp"
//#include "GUI_Shader_Nodes.hpp"

// FWD DECL OTHER
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Manager;
	}
}

// FWD DECL THIS
namespace GUI {
	namespace WORKSPACE {
		struct Workspace_Shader_Editor;
		struct Shader_Node_Viewport;
		struct Shader_Node_Shelf;
		struct Shader_Code_Editor;
	}
}

// DECL
namespace GUI {
	namespace WORKSPACE {
		namespace SHADER_EDITOR {
			struct Syntax_Highlighter : QSyntaxHighlighter {
				Syntax_Highlighter(QTextDocument *parent = nullptr);

			protected:
				void highlightBlock(const QString &text) override;

			private:
				struct HighlightingRule {
					QRegularExpression pattern;
					QTextCharFormat format;
				};

				QVector<HighlightingRule> highlightingRules;
				QTextCharFormat user_keywords;
				QTextCharFormat control;
				QTextCharFormat variables;
				QTextCharFormat macros;
				QTextCharFormat floats;
				QTextCharFormat integers;
				QTextCharFormat comments;
			};
		}
		struct Workspace_Shader_Editor : GUI::Linear_Contents {
			Workspace_Manager* parent;

			Shader_Code_Editor* glsl_editor;
			Shader_Node_Shelf* shelf;

			KL::Shader* active_shader;

			//Shader_Node_Viewport* viewport;

			Workspace_Shader_Editor(Workspace_Manager* parent);
		};
		/* TODO:
			Allow Visualization of GUI::Node_Tree during execution
			CHECK Circular dependencies
			PREVENT connection between different data_types
			Alt-Drag deselects
			Alt-Click deselects
		*/
		struct Shader_Code_Editor : Text_Edit {
			Workspace_Shader_Editor* parent;
			QCompleter* completer;
			SHADER_EDITOR::Syntax_Highlighter* highlighter;

			Shader_Code_Editor(Workspace_Shader_Editor* parent);
			
			void load(KL::Shader* shader);

			void keyPressEvent(QKeyEvent *event) override;
			void onTextChanged();
		};
		//struct Shader_Node_Viewport : GUI::Graphics_View {
		//	Workspace_Shader_Node_Editor* parent;
		//	QGraphicsScene* scene;
		//
		//	bool pan;
		//	bool moving;
		//	bool selecting;
		//	bool connecting;
		//
		//	QPointF pan_pos;
		//	QPointF move_pos;
		//	qreal  view_scale;
		//	QPointF selection_start;
		//	QGraphicsRectItem* selection_rect;
		//	vector<GUI::NODE::Node*> selection;
		//
		//	GUI::NODE::Connection* connection;
		//	GUI::NODE::Node_Tree* active_node_tree;
		//
		//	Shader_Node_Viewport(Workspace_Shader_Node_Editor* parent);
		//	~Shader_Node_Viewport();
		//
		//	void f_objectChanged(KL::Object* object);
		//	void loadNodes();
		//
		//	void drawBackground(QPainter* painter, const QRectF& rect) override;
		//
		//	//void mouseDoubleClickEvent(QMouseEvent* event) override {}
		//	void mouseReleaseEvent(QMouseEvent* event) override;
		//	void mousePressEvent(QMouseEvent* event) override; // TODO Fix sticky mouse, double clicks and odd non-registering behaviors
		//	void mouseMoveEvent(QMouseEvent* event) override;
		//	void keyPressEvent(QKeyEvent* event) override;
		//
		//	void dragMoveEvent(QDragMoveEvent* event) override;
		//	void resizeEvent(QResizeEvent* event) override;
		//	void wheelEvent(QWheelEvent* event) override;
		//	void dropEvent(QDropEvent* event) override;
		//};
		//
		struct Shader_Node_Shelf : Tree {
			Workspace_Shader_Editor* parent;
			Shader_Node_Shelf(Workspace_Shader_Editor* parent);
		};
	}
}