#include "Workspaces/Shader_Editor.hpp"

#include "Workspaces/Manager.hpp"

#include "Object/Nodes/Compiler.hpp"

GUI::WORKSPACE::Workspace_Shader_Editor::Workspace_Shader_Editor(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent),
	active_shader(nullptr)
{
	parent->setMaximumWidth(800);
	//viewport = new Shader_Node_Viewport(this);
	shelf = new Shader_Node_Shelf(this);

	glsl_editor = new Shader_Code_Editor(this);

	auto splitter = new GUI::Splitter(this);
	splitter->addWidget(glsl_editor);
	splitter->addWidget(shelf);

	addWidget(splitter);
}

GUI::WORKSPACE::Shader_Code_Editor::Shader_Code_Editor(Workspace_Shader_Editor* parent) :
	parent(parent),
	Text_Edit(parent)
{
	completer = new QCompleter(this);
	highlighter = new SHADER_EDITOR::Syntax_Highlighter(this->document());
	//connect(this, &QPlainTextEdit::textChanged, this, &Shader_Code_Editor::onTextChanged);
}

void GUI::WORKSPACE::Shader_Code_Editor::load(KL::Shader* shader) {
	if (parent->active_shader != nullptr) {
		const string editor_code = toPlainText().toStdString();
		if (parent->active_shader->shader_code != editor_code) {
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Save Changes", "Save Changes:", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
			if (reply == QMessageBox::Yes) {
				parent->active_shader->shader_code = editor_code;
			}
			else if (reply == QMessageBox::Cancel) {
				return;
			}
		}
	}
	if (shader->type == KL::SHADER::Type::CODE) {
		parent->active_shader = shader;
		setPlainText(QString::fromStdString(shader->shader_code));
	}
}

void GUI::WORKSPACE::Shader_Code_Editor::keyPressEvent(QKeyEvent* event) {
	if (completer && completer->popup()->isVisible()) {
		switch (event->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Tab:
			event->ignore();
			return;
		default:
			break;
		}
	}
	QPlainTextEdit::keyPressEvent(event);
}

void GUI::WORKSPACE::Shader_Code_Editor::onTextChanged() {
	QTextCursor cursor = textCursor();
	cursor.select(QTextCursor::WordUnderCursor);
	QString word = cursor.selectedText();
	if (word.endsWith('.')) {
		QStringList suggestions = { "member1", "member2", "method1", "method2" };
		completer->setModel(new QStringListModel(suggestions, completer));
		completer->complete();
	}
}

GUI::WORKSPACE::Shader_Node_Shelf::Shader_Node_Shelf(Workspace_Shader_Editor* parent) :
	parent(parent),
	Tree(parent)
{
	setMaximumWidth(100);

	for (KL::Shader* shader : FILE->shaders) {
		auto item = new Tree_Item(this, QString::fromStdString(shader->name), 1);
		item->setData(0, 1000, ptr(shader));
	}

	QObject::connect(this, &GUI::Tree::itemDoubleClicked, [this](QTreeWidgetItem* item) {
		this->parent->glsl_editor->load(reinterpret_cast<KL::Shader*>(item->data(0,1000).toULongLong()));
	});
}

GUI::WORKSPACE::SHADER_EDITOR::Syntax_Highlighter::Syntax_Highlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
	HighlightingRule rule;

	user_keywords.setForeground(QColor(86,156,214));
	QStringList keywordPatterns;
	keywordPatterns <<
		"float" << "vec2"  << "vec3"  << "vec4"  <<
		"int"   << "ivec2" << "ivec3" << "ivec4" <<
		"uint"  << "uvec2" << "uvec3" << "uvec4" <<
		"mat2" << "mat3" << "mat4" << "bool" << "struct" << "layout" << "buffer" << "#define" << "in" << "out" << "inout" << "const";
	foreach(const QString & pattern, keywordPatterns) {
		rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
		rule.format = user_keywords;
		highlightingRules.append(rule);
	}

	control.setForeground(QColor(216,160,223));
	QStringList operationPatterns;
	operationPatterns << "if" << "else" << "else if" << "while" << "for" << "return" << "switch" << "case" << "break" << "continue";
	foreach(const QString & pattern, operationPatterns) {
		rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
		rule.format = control;
		highlightingRules.append(rule);
	}

	variables.setForeground(QColor(200,200,200));
	QStringList builtinPatterns;
	builtinPatterns << "hit_data" << "ray";
	foreach(const QString & pattern, builtinPatterns) {
		rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
		rule.format = variables;
		highlightingRules.append(rule);
	}
	
	macros.setForeground(QColor(190,183,255));
	QStringList constantsPatterns;
	constantsPatterns << "TWO_PI" << "PI" << "HALF_PI" << "INV_PI" << "DEG_RAD" << "RAD_DEG" << "MAX_DIST" << "EPSILON" << "MAX_UINT";
	foreach(const QString & pattern, constantsPatterns) {
		rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
		rule.format = macros;
		highlightingRules.append(rule);
	}

	comments.setForeground(QColor(87,166,74));
	rule.pattern = QRegularExpression("//[^\n]*");
	rule.format = comments;
	highlightingRules.append(rule);

	integers.setForeground(QColor(181,206,168));
	rule.pattern = QRegularExpression("\\b[-+]?[0-9]+[uU]?\\b");
	rule.format = integers;
	highlightingRules.append(rule);

	floats.setForeground(QColor(185,225,172));
	rule.pattern = QRegularExpression("\\b[-+]?([0-9]*\\.[0-9]+|[0-9]+\\.)([eE][-+]?[0-9]+)?[fF]?\\b");
	rule.format = floats;
	highlightingRules.append(rule);
}

void GUI::WORKSPACE::SHADER_EDITOR::Syntax_Highlighter::highlightBlock(const QString& text) {
	foreach(const HighlightingRule & rule, highlightingRules) {
		QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			setFormat(match.capturedStart(), match.capturedLength(), rule.format);
		}
	}
}