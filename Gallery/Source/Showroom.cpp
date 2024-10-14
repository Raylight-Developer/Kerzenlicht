#include "Showroom.hpp"

QPixmap f_showroomThumbnail(const string& file_path, const string& name, const dvec2& pos, const dvec2& scale, const ShowFromat& format) {
	TIMER_START;
	auto pixmap = QPixmap(QString::fromStdString(file_path));
	QPixmap overlay;
	switch (format) {
		case ShowFromat::SQUARE: {
			overlay = QPixmap("./Resources/Square.png");
			break;
		}
		case ShowFromat::PORTRAIT: {
			overlay = QPixmap("./Resources/Portrait.png");
			break;
		}
		case ShowFromat::LANDSCAPE: {
			overlay = QPixmap("./Resources/Landscape.png");
			break;
		}
	}

	auto transform = QTransform();
	transform.translate(pos.x, pos.y);
	transform.scale(scale.x, scale.y);
	pixmap = pixmap.transformed(transform);
	overlay = overlay.transformed(transform);

	auto rect = QRectF(0, 0, pixmap.width(), pixmap.height());

	auto result_pixmap = QPixmap(pixmap.size());
	result_pixmap.fill(Qt::transparent);

	auto painter = QPainter(&result_pixmap);
	painter.setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	auto path = QPainterPath();
	path.addRoundedRect(rect, 80, 80);

	painter.setClipPath(path);
	painter.fillRect(rect, pixmap);
	painter.fillRect(rect, overlay);

	auto font = QFont();
	font.setPointSize(100);
	painter.setFont(font);

	QPainterPath painterPath = QPainterPath();
	painterPath.addText(QPointF(100, 150), font, QString::fromStdString(name));
	painter.strokePath(painterPath, QPen(QColor(0, 0, 0), 10.0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
	painter.fillPath(painterPath, QColor(255, 255, 255));

	painter.end();
	TIMER_END;
	return result_pixmap;
}

Showroom::Showroom(QWidget* parent) :
	parent(parent),
	GUI::Linear_Contents(parent, QBoxLayout::Direction::TopToBottom)
{
	header = new GUI::Header(this);
	search_bar = new GUI::Value_Input(this);
	header->addWidget(search_bar);

	list = new GUI::List(this);
	addWidget(header);
	addWidget(list);
}