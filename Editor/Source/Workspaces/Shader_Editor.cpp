#include "Workspaces/Shader_Editor.hpp"

#include "Workspaces/Manager.hpp"

GUI::WORKSPACE::Workspace_Shader_Editor::Workspace_Shader_Editor(Workspace_Manager* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent),
	active_shader(nullptr)
{
	shelf = new Shader_Shelf(this);

	glsl_editor = new Shader_Code_Editor(this);
	shader_inputs = new Shader_Inputs(this);

	auto save_button = new Button(this);
	save_button->setText("Save");

	auto header = new GUI::Linear_Contents(this);
	header->setFixedHeight(30);
	header->addWidget(save_button);

	auto splitter = new GUI::Splitter(this);
	splitter->addWidget(shelf);
	splitter->addWidget(glsl_editor);
	splitter->addWidget(shader_inputs);

	auto header_splitter = new GUI::Splitter(this, true);
	header_splitter->addWidget(header);
	header_splitter->addWidget(splitter);

	addWidget(header_splitter);

	connect(save_button, &Button::clicked, [this]() {
		glsl_editor->save();
	});
}

GUI::WORKSPACE::Shader_Shelf::Shader_Shelf(Workspace_Shader_Editor* parent) :
	parent(parent),
	Tree(parent)
{
	for (KL::Shader* shader : FILE->shaders) {
		auto item = new Tree_Item(this, QString::fromStdString(shader->name), 1);
		item->setData(0, 1000, uptr(shader));
	}

	QObject::connect(this, &GUI::Tree::itemDoubleClicked, [this](QTreeWidgetItem* item) {
		this->parent->glsl_editor->load(ptr<KL::Shader*>(item->data(0,1000).toULongLong()));
		this->parent->shader_inputs->load(ptr<KL::Shader*>(item->data(0,1000).toULongLong()));
	});
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
	if (parent->active_shader) {
		if (parent->active_shader->shader_code != toPlainText().toStdString()) {
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Save Changes", "Save Changes:", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
			if (reply == QMessageBox::Yes) {
				save();
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

void GUI::WORKSPACE::Shader_Code_Editor::save() {
	if (parent->active_shader != nullptr) {
		if (parent->active_shader->shader_code != toPlainText().toStdString()) {
			auto pointer = new KL::SHADER_CMD::Shader_Code(parent->active_shader, toPlainText().toStdString());

			KL::Session::getInstance().getHistory()->execute(
				pointer
			);
		}
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

GUI::WORKSPACE::Shader_Inputs::Shader_Inputs(Workspace_Shader_Editor* parent) :
	parent(parent)
{

}

void GUI::WORKSPACE::Shader_Inputs::load(KL::Shader* shader) {
	clear();
	for (const Prop& data : shader->inputs) {
		QListWidgetItem* item = new QListWidgetItem(this);
		item->setData(1000, uptr(shader));
		switch (data.type) {
			case PROP::Type::TEXTURE: {
				item->setText("TEXTURE | " + QString::fromStdString(data.getTexture()->name));
				break;
			}
		}
		addItem(item);
	}
}