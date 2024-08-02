#pragma once

#include "Shared.hpp"
#include "QT.hpp"

namespace GUI {
	struct Double_Input;

	struct Int_Slider : Slider {
		QTimer time_out;
		bool time_out_active;
		QPoint time_out_pos;

		uint time;
		int32 value;
		int32 min;
		int32 max;
		int32 step;

		Int_Slider(QWidget* parent = nullptr);

		void mouseReleaseEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void paintEvent(QPaintEvent* event) override;
	};

	class Double_Slider : public Slider {
		Q_OBJECT
	public: 
		QTimer time_out;
		bool time_out_active;
		QPoint time_out_pos;

		uint time;
		dvec1 value;
		dvec1 min;
		dvec1 max;
		dvec1 step;
		uint precision;

		Double_Slider(QWidget* parent = nullptr);

		void setValue(const dvec1& value);

		void mouseReleaseEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void paintEvent(QPaintEvent* event) override;

	signals:
		void manualValue();
	};

	struct Double_Input : Linear_Contents {
		Double_Slider* slider_input;
		Value_Input* manual_input;
		Toggle* keyframe;

		Double_Input(QWidget* parent = nullptr);

		bool eventFilter(QObject* watched, QEvent* event) override;
	};
}