#pragma once

#include "QT.hpp"
#include "Threading.hpp"

enum struct ShowFromat {
	SQUARE,
	PORTRAIT,
	LANDSCAPE
};
QPixmap f_showroomThumbnail(const string& file_path, const string& name, const dvec2& pos, const dvec2& scale, const ShowFromat& format);
QPixmap f_showroomFull(const string& file_path, const string& name, const dvec2& pos, const dvec2& scale, const ShowFromat& format);

struct Showroom : GUI::Linear_Contents {
	QWidget* parent;
	GUI::Header* header;
	GUI::Value_Input* search_bar;
	GUI::List* list;

	Showroom(QWidget* parent = nullptr);
	void loadThumbnails();
};