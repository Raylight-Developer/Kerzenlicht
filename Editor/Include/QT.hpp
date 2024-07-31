#pragma once

#include "Shared.hpp"

#undef FILE

#include <QtOpenGL/qopenglfunctions_4_5_core.h>
#include <QtOpenGLWidgets>
#include <QtWidgets>
#include <QtCore>
#include <QtGui>

namespace GUI {
	// Base Classes
	struct Application;
	struct Button;
	struct Dock;
	struct Form;
	struct Graphics_View;
	struct Label;
	struct Linear_Layout;
	struct Linear_Contents;
	struct List;
	struct Menu;
	struct Splitter;
	struct Tabs;
	struct Text_Edit;
	struct Text_Stream;
	struct ToolBar;
	struct Tree_Item;
	struct Tree;
	struct Value_Input;
	struct Widget_List;
	struct Window;

	struct Toggle;
	struct Floating_Toggle;
}

struct BranchDelegate : QStyledItemDelegate {
	BranchDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

namespace GUI {
	struct Application : QApplication {
		Application(int argc, char* argv[]);
		~Application() {};
	};

	struct Button : QPushButton {
		Button(QWidget* parent = nullptr);
		~Button() {};
	};

	struct Dock : QDockWidget {
		Dock(QWidget* parent = nullptr);
		~Dock() {};
	};

	struct Form : QWidget {
		QFormLayout* layout;

		Form(QWidget* parent = nullptr);
		~Form() { delete layout; };
	};

	struct Graphics_View : QGraphicsView {
		Graphics_View(QWidget* parent = nullptr);
	};

	struct Graphics_Scene : QGraphicsScene {
		Graphics_Scene(QWidget* parent = nullptr);
	};

	struct Label : QLabel {
		Label(QWidget* parent = nullptr, const QString& label = "");
		~Label() {};
	};

	struct Linear_Layout : QBoxLayout {
		Linear_Layout(QWidget* parent = nullptr, const QBoxLayout::Direction& direction = QBoxLayout::LeftToRight);
		Linear_Layout(QWidget* parent, const QBoxLayout::Direction& direction, const Qt::AlignmentFlag& alignment);
		~Linear_Layout() {};

		void clear();
	};

	struct Linear_Contents : QWidget {
		Linear_Layout* layout;

		Linear_Contents(QWidget* parent = nullptr, const QBoxLayout::Direction& direction = QBoxLayout::LeftToRight);
		Linear_Contents(QWidget* parent, const QBoxLayout::Direction& direction, const Qt::AlignmentFlag& alignment);
		~Linear_Contents() { delete layout; };

		void addWidget(QWidget* widget);
	};

	struct List : QListWidget {
		List(QWidget* parent = nullptr, const bool& icon_list = false);
		~List() {};

		bool itemExists(const QString& label) const;
		int itemRow(const QString& label) const;
		vector<int> visibleRange(const int& padding = 14) const;
	};

	struct Menu : QMenu {
		Menu(QWidget* parent = nullptr);
		~Menu() {};
	};

	struct Value_Input : QLineEdit {
		Value_Input(QWidget* parent = nullptr);
		~Value_Input() {};
	};

	struct Slider : QSlider {
		Slider(QWidget* parent = nullptr);
		~Slider() {};

		void mousePressEvent(QMouseEvent* event) override;
	};

	struct Splitter : QSplitter {
		Splitter(QWidget* parent = nullptr, const bool& vertical = false);
		~Splitter() {};
	};

	struct Tabs : QTabWidget {
		Tabs(QWidget* parent = nullptr);
		~Tabs() {};
	};

	struct Text_Edit : QPlainTextEdit {
		Text_Edit(QWidget* parent = nullptr);
		~Text_Edit() {};
	};

	struct Text_Stream : QTextEdit {
		Text_Stream(QWidget* parent = nullptr);
		~Text_Stream() {};
	};

	struct ToolBar : QToolBar {
		ToolBar(QWidget* parent = nullptr);
		~ToolBar() {};
	};

	struct Tree_Item : QTreeWidgetItem {
		Tree_Item(Tree_Item* parent, const QString& label, const uint& level = 1, const map<uint, QString>& data = {});
		Tree_Item(Tree* parent = nullptr, const QString& label = "", const uint& level = 0);
		~Tree_Item();

		void clear();
	};

	struct Tree : QTreeWidget {
		Tree(QWidget* parent = nullptr);
		~Tree();

		void mouseReleaseEvent(QMouseEvent* event);
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);

		void clearChildren();
		int childCount() const;

	private:
		bool expand_active;
		QTreeWidgetItem* drag_press_item;
		QList<QTreeWidgetItem*> drag_press_list;
	};

	struct Widget_List : QScrollArea {
		Linear_Contents* contents;

		Widget_List(QWidget* parent = nullptr);
		~Widget_List() { delete contents; };
	};
	struct Window : QMainWindow {
		Window(QWidget* parent = nullptr);
		~Window() {};
	};

	struct Toggle : Button {
		Toggle(QWidget* parent = nullptr);
		~Toggle() {};
	};

	struct Floating_Toggle : Toggle {
		QPoint drag_pos;

		Floating_Toggle(QWidget* parent = nullptr);
		~Floating_Toggle() {};

		void mousePressEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
	};
};

inline ivec2 p_to_i(const QPoint& val) { return ivec2(val.x(), val.y()); }
inline dvec2 p_to_d(const QPointF& val) { return dvec2(val.x(), val.y()); }