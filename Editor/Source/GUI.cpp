#include "GUI.hpp"

#include "Ops.hpp"

GUI::Int_Slider::Int_Slider(QWidget* parent) :
	Slider(parent)
{
	time_out_active = false;
	time = 175;
	value = 5;
	min = 0;
	max = 10;
	step = 1;

	setRange(min, max);

	connect(this, &Slider::valueChanged, [this](int val) { value = val; });
}

void GUI::Int_Slider::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::MouseButton::LeftButton) {
		time_out_active = false;
		time_out.stop();
	}
}

void GUI::Int_Slider::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::MouseButton::LeftButton) {
		time_out.start(time);
		time_out_active = false;
		time_out_pos = event->pos();
		connect(&time_out, &QTimer::timeout, [this]() {
			QStyleOptionSlider option = QStyleOptionSlider();
			initStyleOption(&option);
			const QRect slider_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderHandle, this);

			const QRect handle_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderGroove, this);
			const QPoint center = slider_size.center() - slider_size.topLeft();
			const QPoint pos = time_out_pos - center;
			const int32 length = slider_size.width();
			const int32 min = handle_size.x();
			const int32 max = handle_size.right() - length + 1;
			setSliderPosition(style()->sliderValueFromPosition(minimum(), maximum(), pos.x() - min, max - min));
			time_out.stop();
			time_out_active = true;
		});
	}
}

void GUI::Int_Slider::mouseMoveEvent(QMouseEvent* event) {
	if (time_out_active and underMouse()) {
		QStyleOptionSlider option = QStyleOptionSlider();
		initStyleOption(&option);
		const QRect slider_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderHandle, this);

		const QRect handle_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderGroove, this);
		const QPoint center = slider_size.center() - slider_size.topLeft();
		const QPoint pos = event->pos() - center;
		const int32 length = slider_size.width();
		const int32 min = handle_size.x();
		const int32 max = handle_size.right() - length + 1;
		setSliderPosition(style()->sliderValueFromPosition(minimum(), maximum(), pos.x() - min, max - min));
	}
}

void GUI::Int_Slider::paintEvent(QPaintEvent* event) {
	QPainter painter = QPainter(this);
	QStyleOptionSlider option = QStyleOptionSlider();

	painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	QRect rect = QRect(0, 0, width(), height());
	dvec1 center = f_map(i_to_d(min), i_to_d(max), 0.0, 1.0, i_to_d(value));
	QLinearGradient gradient(rect.topLeft(), rect.topRight());
	if (center <= 0.9999 and center >= 0.0001) {
		gradient.setColorAt(0, QColor(250, 145, 25));
		gradient.setColorAt(center - 0.0001, QColor(250, 145, 25));
		gradient.setColorAt(center + 0.0001, Qt::GlobalColor::transparent);
		gradient.setColorAt(1, Qt::GlobalColor::transparent);
	}
	else if (center >= 0.9999) {
		gradient.setColorAt(0, QColor(250, 145, 25));
		gradient.setColorAt(1, QColor(250, 145, 25));
	}
	else {
		gradient.setColorAt(0, Qt::GlobalColor::transparent);
		gradient.setColorAt(1, Qt::GlobalColor::transparent);
	}

	painter.setPen(QPen(QColor(210, 165, 255), 1.0));
	painter.setBrush(gradient);
	painter.drawRoundedRect(rect, 5, 5);

	QPainterPath painterPath = QPainterPath();
	painterPath.addText(QPointF(geometry().width() / 2 - QFontMetrics(font()).horizontalAdvance(QString::number(value)) / 2, geometry().height() * 0.75), font(), QString::number(value));
	painter.strokePath(painterPath, QPen(QColor(250, 250, 250), 0.5));
	painter.fillPath(painterPath, QColor(250, 250, 250));
}

GUI::Double_Slider::Double_Slider(QWidget* parent) :
	Slider(parent)
{
	time_out_active = false;
	time = 175;
	value = 0.5;
	min = 0.0;
	max = 1.0;
	step = 0.05;
	precision = 3;


	setRange(MIN_INT32, MAX_INT32);
	connect(this, &Slider::valueChanged, [this](int val) {
		value = f_map<dvec1>(i_to_d(MIN_INT32), i_to_d(MAX_INT32), min, max, i_to_d(val));
	});
}

void GUI::Double_Slider::setValue(const double& value) {
	this->value = value;
	Slider::setValue(d_to_i(f_map<dvec1>(min, max, i_to_d(MIN_INT32), i_to_d(MAX_INT32), value)));
}

void GUI::Double_Slider::mouseReleaseEvent(QMouseEvent* event) {
	if (event->button() == Qt::MouseButton::LeftButton) {
		time_out.stop();
		if (!time_out_active) {
			emit manualValue();
		}
		time_out_active = false;
	}
}

void GUI::Double_Slider::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::MouseButton::LeftButton) {
		time_out.start(time);
		time_out_active = false;
		time_out_pos = event->pos();
		connect(&time_out, &QTimer::timeout, [this]() {
			QStyleOptionSlider option = QStyleOptionSlider();
			initStyleOption(&option);
			const QRect slider_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderHandle, this);

			const QRect handle_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderGroove, this);
			const QPoint center = slider_size.center() - slider_size.topLeft();
			const QPoint pos = time_out_pos - center;
			const int32 length = slider_size.width();
			const int32 min = handle_size.x();
			const int32 max = handle_size.right() - length + 1;
			setSliderPosition(style()->sliderValueFromPosition(minimum(), maximum(), pos.x() - min, max - min));
			time_out.stop();
			time_out_active = true;
		});
	}
}

void GUI::Double_Slider::mouseMoveEvent(QMouseEvent* event) {
	if (time_out_active and underMouse()) {
		QStyleOptionSlider option = QStyleOptionSlider();
		initStyleOption(&option);
		const QRect slider_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderHandle, this);

		const QRect handle_size = style()->subControlRect(QStyle::ComplexControl::CC_Slider, &option, QStyle::SubControl::SC_SliderGroove, this);
		const QPoint center = slider_size.center() - slider_size.topLeft();
		const QPoint pos = event->pos() - center;
		const int32 length = slider_size.width();
		const int32 min = handle_size.x();
		const int32 max = handle_size.right() - length + 1;
		setSliderPosition(style()->sliderValueFromPosition(minimum(), maximum(), pos.x() - min, max - min));
	}
}

void GUI::Double_Slider::paintEvent(QPaintEvent* event) {
	QPainter painter = QPainter(this);
	QStyleOptionSlider option = QStyleOptionSlider();

	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	
	QRect rect = QRect(0, 0, width(), height());
	dvec1 center = f_map(min, max, 0.0, 1.0, value);
	QLinearGradient gradient(rect.topLeft(), rect.topRight());
	if (center <= 0.9999 and center >= 0.0001) {
		gradient.setColorAt(0, QColor(250, 145, 25));
		gradient.setColorAt(center - 0.0001, QColor(250, 145, 25));
		gradient.setColorAt(center + 0.0001, Qt::GlobalColor::transparent);
		gradient.setColorAt(1, Qt::GlobalColor::transparent);
	}
	else if (center >= 0.9999) {
		gradient.setColorAt(0, QColor(250, 145, 25));
		gradient.setColorAt(1, QColor(250, 145, 25));
	}
	else {
		gradient.setColorAt(0, Qt::GlobalColor::transparent);
		gradient.setColorAt(1, Qt::GlobalColor::transparent);
	}

	painter.setPen(QPen(QColor(210, 165, 255), 1.0));
	painter.setBrush(gradient);
	painter.drawRoundedRect(rect, 5, 5);

	QPainterPath painterPath = QPainterPath();
	painterPath.addText(QPointF(geometry().width() / 2 - QFontMetrics(font()).horizontalAdvance(QString::number(value, 'f', precision)) / 2, geometry().height() * 0.75), font(), QString::number(value, 'f', precision));
	painter.strokePath(painterPath, QPen(QColor(250, 250, 250), 0.5));
	painter.fillPath(painterPath, QColor(250, 250, 250));
}

GUI::Double_Input::Double_Input(QWidget* parent) :
	Linear_Contents(parent)
{
	slider_input = new Double_Slider(this);

	manual_input = new Value_Input(this);
	QDoubleValidator* validator = new QDoubleValidator(slider_input->min, slider_input->max, 15, manual_input);
	manual_input->setValidator(validator);
	manual_input->hide();

	keyframe = new Toggle(this);
	keyframe->setObjectName("Keyframe_Toggle");

	addWidget(slider_input);
	addWidget(manual_input);
	addWidget(keyframe);

	manual_input->installEventFilter(this);
	connect(slider_input, &Double_Slider::manualValue, [this]() {
		slider_input->hide();
		manual_input->show();
		manual_input->setText(QString::number(slider_input->value));
		manual_input->selectAll();
		manual_input->setFocus();
	});
	connect(manual_input, &Value_Input::returnPressed, [this]() {
		slider_input->setValue(manual_input->text().toDouble());
		manual_input->clearFocus();
		manual_input->hide();
		slider_input->show();
	});
	connect(manual_input, &Value_Input::textChanged, [this](const QString& text) {
		if (text.toDouble() > slider_input->max)
			manual_input->setText(QString::number(slider_input->max));
		else if (text.toDouble() < slider_input->minimum())
			manual_input->setText(QString::number(slider_input->minimum()));
	});
}

bool GUI::Double_Input::eventFilter(QObject* watched, QEvent* event) {
	if (event->type() == QEvent::Type::FocusOut or (event->type() == QEvent::Type::KeyPress and static_cast<QKeyEvent*>(event)->key() == Qt::Key::Key_Escape)) {
		slider_input->show();
		manual_input->hide();
		manual_input->setText(QString::number(slider_input->value));
	}
	return false;
}