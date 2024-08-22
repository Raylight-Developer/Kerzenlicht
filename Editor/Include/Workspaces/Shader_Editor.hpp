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
		struct Shader_Shelf;
		struct Shader_Inputs;
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

			Shader_Shelf* shelf;

			Shader_Code_Editor* glsl_editor;
			Shader_Inputs* shader_inputs;

			KL::Shader* active_shader;

			Workspace_Shader_Editor(Workspace_Manager* parent);
		};

		struct Shader_Shelf : Tree {
			Workspace_Shader_Editor* parent;
			Shader_Shelf(Workspace_Shader_Editor* parent);
		};

		struct Shader_Code_Editor : Text_Edit {
			Workspace_Shader_Editor* parent;
			QCompleter* completer;
			SHADER_EDITOR::Syntax_Highlighter* highlighter;

			Shader_Code_Editor(Workspace_Shader_Editor* parent);
			
			void load(KL::Shader* shader);

			void keyPressEvent(QKeyEvent *event) override;
			void onTextChanged();
		};

		struct Shader_Inputs : List {
			Workspace_Shader_Editor* parent;
			Shader_Inputs(Workspace_Shader_Editor* parent);

			void load(KL::Shader* shader);
		};
	}
}