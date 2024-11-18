#include "Showroom.hpp"

QPixmap f_showroomThumbnail(const string& file_path, const string& name, const dvec2& pos, const dvec2& scale, const ShowFromat& format) {
	TIMER_START;
	auto pixmap = QPixmap(QString::fromStdString(file_path));
	QPixmap overlay;
	QPixmap result_pixmap;
	switch (format) {
		case ShowFromat::SQUARE: {
			overlay = QPixmap("./Resources/Square_Thumb.png");
			result_pixmap = QPixmap(QSize(300, 300));
			break;
		}
		case ShowFromat::PORTRAIT: {
			overlay = QPixmap("./Resources/Portrait_Thumb.png");
			result_pixmap = QPixmap(QSize(300, 450));
			break;
		}
		case ShowFromat::LANDSCAPE: {
			overlay = QPixmap("./Resources/Landscape_Thumb.png");
			result_pixmap = QPixmap(QSize(450, 300));
			break;
		}
	}

	auto transform = QTransform();
	transform.translate(pos.x, pos.y);
	transform.scale(scale.x, scale.y);
	pixmap = pixmap.transformed(transform);
	overlay = overlay.transformed(transform);

	auto rect = QRectF(0, 0, pixmap.width(), pixmap.height());

	result_pixmap.fill(Qt::transparent);

	auto painter = QPainter(&result_pixmap);
	painter.setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	auto path = QPainterPath();
	path.addRoundedRect(rect, 8, 8);

	painter.setClipPath(path);
	painter.fillRect(rect, pixmap);
	painter.fillRect(rect, overlay);
	painter.strokePath(path, QPen(QColor(255, 255, 255), 1.0));

	auto font = QFont();
	font.setPointSize(20);
	painter.setFont(font);

	QPainterPath painterPath = QPainterPath();
	painterPath.addText(QPointF(10, 30), font, QString::fromStdString(name));
	painter.strokePath(painterPath, QPen(QColor(0, 0, 0), 1.0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
	painter.fillPath(painterPath, QColor(255, 255, 255));

	painter.end();
	TIMER_END;
	return result_pixmap;
}

QPixmap f_showroomFull(const string& file_path, const string& name, const dvec2& pos, const dvec2& scale, const ShowFromat& format) {
	TIMER_START;
	auto pixmap = QPixmap(QString::fromStdString(file_path));
	QPixmap overlay;
	QPixmap result_pixmap;
	switch (format) {
		case ShowFromat::SQUARE: {
			overlay = QPixmap("./Resources/Square.png");
			result_pixmap = QPixmap(QSize(3000, 3000));
			break;
		}
		case ShowFromat::PORTRAIT: {
			overlay = QPixmap("./Resources/Portrait.png");
			result_pixmap = QPixmap(QSize(3000, 4500));
			break;
		}
		case ShowFromat::LANDSCAPE: {
			overlay = QPixmap("./Resources/Landscape.png");
			result_pixmap = QPixmap(QSize(4500, 3000));
			break;
		}
	}

	auto transform = QTransform();
	transform.translate(pos.x, pos.y);
	transform.scale(scale.x, scale.y);
	pixmap = pixmap.transformed(transform);
	overlay = overlay.transformed(transform);

	auto rect = QRectF(0, 0, pixmap.width(), pixmap.height());

	result_pixmap.fill(Qt::transparent);

	auto painter = QPainter(&result_pixmap);
	painter.setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	auto path = QPainterPath();
	path.addRoundedRect(rect, 80, 80);

	painter.setClipPath(path);
	painter.fillRect(rect, pixmap);
	painter.fillRect(rect, overlay);
	//painter.strokePath(path, QPen(QColor(255, 255, 255), 4.0));

	auto font = QFont();
	font.setPointSize(100);
	painter.setFont(font);

	QPainterPath painterPath = QPainterPath();
	painterPath.addText(QPointF(100, 140), font, QString::fromStdString(name));
	painter.strokePath(painterPath, QPen(QColor(0, 0, 0), 10.0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
	painter.fillPath(painterPath, QColor(255, 255, 255));

	painter.end();
	TIMER_END;
	return result_pixmap;
}

Showroom::Showroom(QWidget* parent) :
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom),
	parent(parent)
{
	header = new GUI::Header(this);
	search_bar = new GUI::Value_Input(this);
	header->addWidget(search_bar);

	list = new GUI::List(this);
	addWidget(header);
	addWidget(list);

	auto view = new GUI::Graphics_View(this);
	auto scene = new GUI::Graphics_Scene(this);
	view->setScene(scene);
	scene->addPixmap(f_showroomFull("./Resources/0.png", "Arona", dvec2(0), dvec2(1), ShowFromat::SQUARE));

	addWidget(view);
}

void Showroom::loadThumbnails() {
	const int columns = static_cast<int>(floor(list->width() / 256));
	const QListWidgetItem* item_a = list->itemAt(QPoint(5, 5));
	const QListWidgetItem* item_b = list->itemAt(QPoint(5, list->height() - 10));

	int index_a = 0;
	int index_b = 0;

	if (item_a != nullptr)
	index_a = list->row(item_a);
	if (item_b != nullptr)
	index_b = list->row(item_b);

	if ((index_a - columns) < 0)
		index_a = 0;
	else
	index_a -= columns;

	if ((index_b + 2 * columns) >= list->count())
	index_b = list->count();
	else index_b += (2 * columns);

	for (int i = 0; i < index_a; i++) {
		list->item(i)->setIcon(QIcon());
	}
	for (int i = index_a; i < index_b; i++) {
		Image_Thread* thread = new Image_Thread(list->item(i)->data(110).toString());
		thread->args["item"] = CORE::Prop(uptr(list->item(i)));
		//app->preview_threads.push_back(thread);
		connect(thread, &Image_Thread::result, [this](const unordered_map<string, CORE::Prop>& args, const QIcon& icon) {
			QListWidgetItem* item = ptr<QListWidgetItem*>(args.at("item").getUint());
				if (item) {
					item->setIcon(icon);
				}
			}
		);

	}
	for (int i = index_b + 1; i < list->count(); i++) {
		list->item(i)->setIcon(QIcon());
	}
}