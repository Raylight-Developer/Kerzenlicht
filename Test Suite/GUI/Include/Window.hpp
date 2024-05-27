#pragma once

#include "Include.hpp"
#include "QT.hpp"

#include "Nodes.hpp"
#include "Node_Def.hpp"

struct App : GUI::Application {
	App(int argc, char* argv[]);
};

struct Window : GUI::Window {
	Window();
};

struct Node_Editor : GUI::Graphics_View {
	QGraphicsScene* scene;

	Node_Editor(QWidget* parent);


	QPointF pan_pos;
	bool pan;

	bool moving;
	QPointF move_pos;
	Node* move_selection;

	bool connecting;
	Connection* connection;

	qreal view_scale;

	void drawBackground(QPainter* painter, const QRectF& rect) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

	void wheelEvent(QWheelEvent* event) override;

	void resizeEvent(QResizeEvent* event) override;
};