#include "QT.hpp"

#include "Ops.hpp"

GUI::Application::Application(int argc, char* argv[]) :
	QApplication(argc, argv)
{
	setStyleSheet(QString::fromStdString(loadFromFile("./Resources/QStylesheet.css")));
}

GUI::Button::Button(QWidget* parent) :
	QPushButton(parent)
{
	setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Dock::Dock(QWidget* parent) :
	QDockWidget(parent)
{
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setAllowedAreas(Qt::AllDockWidgetAreas);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Form::Form(QWidget* parent) :
	QWidget(parent),
	layout(new QFormLayout(this))
{
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Graphics_View::Graphics_View(QWidget* parent) :
	QGraphicsView(parent)
{
	setViewportUpdateMode(QGraphicsView::ViewportUpdateMode::FullViewportUpdate);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAttribute(Qt::WidgetAttribute::WA_NativeWindow);
	setResizeAnchor(QGraphicsView::AnchorUnderMouse);

	setRenderHint(QPainter::RenderHint::TextAntialiasing);
	setRenderHint(QPainter::RenderHint::Antialiasing);

	setContentsMargins(0, 0, 0, 0);

	QSurfaceFormat format;
	format.setSwapInterval(1);
	QSurfaceFormat::setDefaultFormat(format);
}

GUI::Graphics_Scene::Graphics_Scene(QWidget* parent) :
	QGraphicsScene(parent)
{

}

GUI::Label::Label(QWidget* parent, const QString& label) :
	QLabel(label)
{
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Linear_Layout::Linear_Layout(QWidget* parent, const QBoxLayout::Direction& direction) :
	QBoxLayout(direction, parent)
{
	setContentsMargins(0, 0, 0, 0);
	setSpacing(1);
	switch (direction) {
		case QBoxLayout::Direction::LeftToRight: {
			setAlignment(Qt::AlignmentFlag::AlignLeft);
			break;
		}
		case QBoxLayout::Direction::RightToLeft: {
			setAlignment(Qt::AlignmentFlag::AlignRight);
			break;
		}
		case QBoxLayout::Direction::TopToBottom: {
			setAlignment(Qt::AlignmentFlag::AlignTop);
			break;
		}
		case QBoxLayout::Direction::BottomToTop: {
			setAlignment(Qt::AlignmentFlag::AlignBottom);
			break;
		}
	}
}

GUI::Linear_Layout::Linear_Layout(QWidget* parent, const QBoxLayout::Direction& direction, const Qt::AlignmentFlag& alignment) :
	QBoxLayout(direction, parent)
{
	setContentsMargins(0, 0, 0, 0);
	setAlignment(alignment);
	setSpacing(1);
}

void GUI::Linear_Layout::clear() {
	while (count() > 0) {
		itemAt(0)->widget()->hide();
		itemAt(0)->widget()->deleteLater();
	}
}

GUI::Linear_Contents::Linear_Contents(QWidget* parent, const QBoxLayout::Direction& direction) :
	QWidget(parent),
	layout(new GUI::Linear_Layout(this, direction))
{
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Linear_Contents::Linear_Contents(QWidget* parent, const QBoxLayout::Direction& direction, const Qt::AlignmentFlag& alignment) :
	QWidget(parent),
	layout(new GUI::Linear_Layout(this, direction))
{
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

void GUI::Linear_Contents::addWidget(QWidget* widget) {
	layout->addWidget(widget);
}

GUI::List::List(QWidget* parent, const bool& icon_list) :
	QListWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setMovement(QListView::Movement::Static);
	verticalScrollBar()->setSingleStep(50);
	setResizeMode(QListView::Adjust);
	setContentsMargins(0, 0, 0, 0);
	setUniformItemSizes(true);
	setDragEnabled(false);
	setSpacing(0);

	if (icon_list) {
		setSelectionMode(QListWidget::SelectionMode::ExtendedSelection);
		setViewMode(QListWidget::ViewMode::IconMode);
		setIconSize(QSize(256, 256));
	}
}

bool GUI::List::itemExists(const QString& label) const {
	for (int i = 0; i < count(); ++i) {
		QListWidgetItem* item = this->item(i);
		if (item->text() == label) {
			return true;
		}
	}
	return false;
}

int GUI::List::itemRow(const QString& label) const {
	for (int i = 0; i < count(); ++i) {
		QListWidgetItem* item = this->item(i);
		if (item->text() == label) {
			return row(item);
		}
	}
	return -1;
}

vector<int> GUI::List::visibleRange(const int& padding) const {
	int a = indexAt(viewport()->rect().topLeft() + QPoint(10, 10)).row();
	int b = indexAt(viewport()->rect().bottomLeft() + QPoint(10, -10)).row();
	if (a < 0) a = 0;
	if (b < 0) b = count();

	if (b + padding > count()) b = count();
	else b += padding;
	if (a < padding) a = 0;
	else a -= padding;

	vector<int> range;
	for (int i = a; i < b; ++i)
		range.push_back(i);

	return range;
}

GUI::Menu::Menu(QWidget* parent) :
	QMenu(parent)
{
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Value_Input::Value_Input(QWidget* parent) :
	QLineEdit(parent)
{
	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Slider::Slider(QWidget* parent) :
	QSlider(Qt::Orientation::Horizontal, parent)
{
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

void GUI::Slider::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::MouseButton::LeftButton) {
		QStyleOptionSlider option = QStyleOptionSlider();
		initStyleOption(&option);
		const QRect slider_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderHandle, this);
		const QRect handle_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderGroove, this);
		const QPoint center = slider_size.center() - slider_size.topLeft();
		const QPoint pos = event->pos() - center;
		const int32 min = handle_size.x();
		const int32 max = handle_size.right() - slider_size.width() + 1;
		setSliderPosition(style()->sliderValueFromPosition(minimum(), maximum(), pos.x() - min, max - min));
	}
	QSlider::mousePressEvent(event);
}

GUI::Splitter::Splitter(QWidget* parent, const bool& vertical) :
	QSplitter(parent)
{
	setOrientation(vertical ? Qt::Orientation::Vertical : Qt::Orientation::Horizontal);
	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Tabs::Tabs(QWidget* parent) :
	QTabWidget(parent)
{
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setTabPosition(QTabWidget::TabPosition::West);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Text_Edit::Text_Edit(QWidget* parent) :
	QPlainTextEdit(parent)
{
	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Text_Stream::Text_Stream(QWidget* parent) :
	QTextEdit(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
	setTabStopDistance(40);
}

GUI::ToolBar::ToolBar(QWidget* parent) :
	QToolBar(parent)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);

	setContextMenuPolicy(Qt::PreventContextMenu);
	setFloatable(false);
	setMovable(false);

	layout()->setAlignment(Qt::AlignmentFlag::AlignLeft);
	layout()->setContentsMargins(0, 0, 0, 0);
	layout()->setSpacing(1);
}

GUI::Tree_Item::Tree_Item(GUI::Tree* parent, const QString& label, const uint& level) :
	QTreeWidgetItem(parent)
{
	setText(0, label);
	setData(0, 500, level);
}

GUI::Tree_Item::Tree_Item(Tree_Item* parent, const QString& label, const uint& level, const map<uint, QString>& data) :
	QTreeWidgetItem(parent)
{
	setText(0, label);
	setData(0, 500, level);
	for (const auto& [key, value] : data)
		setData(0, key, value);
}

GUI::Tree_Item::~Tree_Item() {
	while (childCount() > 0) {
		QTreeWidgetItem* item = takeChild(0);
		delete item;
	}
}

void GUI::Tree_Item::clear() {
	while (childCount() > 0) {
		QTreeWidgetItem* item = takeChild(0);
		delete item;
	}
}

GUI::Tree::Tree(QWidget* parent) :
	QTreeWidget(parent),
	expand_active(false),
	drag_press_item(nullptr)
{
	setSelectionMode(QTreeWidget::SelectionMode::SingleSelection);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setItemDelegate(new BranchDelegate(this));
	verticalScrollBar()->setSingleStep(10);
	setContentsMargins(0, 0, 0, 0);
	setHeaderHidden(true);
	setIndentation(0);
}

GUI::Tree::~Tree() {
	for (int i = 0; i < topLevelItemCount(); i++) {
		QTreeWidgetItem* item = topLevelItem(i);
		delete item;
	}
}

void GUI::Tree::mouseReleaseEvent(QMouseEvent* event) {
	QTreeWidget::mouseReleaseEvent(event);
	setSelectionMode(QTreeWidget::SelectionMode::SingleSelection);
	clearSelection();
	drag_press_list = QList<QTreeWidgetItem*>();
	expand_active = false;
}

void GUI::Tree::mousePressEvent(QMouseEvent* event) {
	QTreeWidget::mousePressEvent(event);
	QTreeWidgetItem* item = itemAt(event->pos());

	if (item) {
		drag_press_item = currentItem();
		setSelectionMode(QTreeWidget::SelectionMode::SingleSelection);
		drag_press_item->setExpanded(!drag_press_item->isExpanded());
		drag_press_list.append(item);
		expand_active = true;
	}
}

void GUI::Tree::mouseMoveEvent(QMouseEvent* event) {
	QTreeWidget::mouseMoveEvent(event);
	QTreeWidgetItem* item = itemAt(event->pos());

	if (item && drag_press_item && expand_active && !drag_press_list.contains(item)) {
		item->setExpanded(drag_press_item->isExpanded());
		drag_press_list.append(item);
	}
}

int GUI::Tree::childCount() const {
	int count = topLevelItemCount();
	for (int i = 0; i < topLevelItemCount(); i++) {
		count += topLevelItem(i)->childCount();
	}
	return count;
}

void GUI::Tree::clearChildren() {
	for (int i = topLevelItemCount() - 1; i >= 0; --i) {
		topLevelItem(i)->takeChildren();
	}
}

GUI::Window::Window(QWidget* parent) :
	QMainWindow(parent)
{
	setDockOptions(QMainWindow::DockOption::AllowTabbedDocks);
	setDockOptions(QMainWindow::DockOption::AllowNestedDocks);
	setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
	setContentsMargins(0, 0, 0, 0);
}

GUI::Widget_List::Widget_List(QWidget* parent) :
	QScrollArea(parent),
	contents(new GUI::Linear_Contents(this))
{
	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
	setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
	setWidgetResizable(true);
	setWidget(contents);
}

GUI::Toggle::Toggle(QWidget* parent) :
	GUI::Button(parent)
{
	setCheckable(true);
	setChecked(false);
}

GUI::Floating_Toggle::Floating_Toggle(QWidget* parent) :
	GUI::Toggle(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	drag_pos = QPoint(0, 0);
}

void GUI::Floating_Toggle::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::RightButton)
		drag_pos = event->pos();
	GUI::Toggle::mousePressEvent(event);
}

void GUI::Floating_Toggle::mouseMoveEvent(QMouseEvent* event) {
	if (event->buttons() & Qt::RightButton)
		move(mapToParent(event->pos() - drag_pos));
	GUI::Toggle::mouseMoveEvent(event);
}

void BranchDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOptionViewItem opt = option;

	opt.rect.adjust(opt.rect.height() - 6 + 8 * index.data(500).toInt(), 0, 0, 0);
	QStyledItemDelegate::paint(painter, opt, index);

	QStyleOptionViewItem branch;
	branch.rect = QRect(8 * index.data(500).toInt(), option.rect.y() + 2 , option.rect.height()-4, option.rect.height()-4);
	branch.state = option.state;
	const QWidget* widget = option.widget;
	QStyle* style = widget ? widget->style() : static_cast<QApplication*>(QCoreApplication::instance())->style();
	style->drawPrimitive(QStyle::PE_IndicatorBranch, &branch, painter, widget);
}