#pragma once

#include "Nodes.hpp"

struct MATH_Node : Node {
	MATH_Node(const QPoint& pos = QPoint(0, 0));

	void build() override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

struct ADD_Node : MATH_Node {
	ADD_Node(const QPoint& pos = QPoint(0, 0)) : MATH_Node(pos) { label = "Add"; };
	any getData(const string& slot_id) const override {
		return any_cast<double>(inputs[0]->getData()) + any_cast<double>(inputs[1]->getData());
	};
};

struct SUB_Node : MATH_Node {
	SUB_Node(const QPoint& pos = QPoint(0, 0)) : MATH_Node(pos) { label = "Sub"; };
	any getData(const string& slot_id) const override {
		return any_cast<double>(inputs[0]->getData()) - any_cast<double>(inputs[1]->getData());
	};
};

struct MUL_Node : MATH_Node {
	MUL_Node(const QPoint& pos = QPoint(0, 0)) : MATH_Node(pos) { label = "Mul"; };
	any getData(const string& slot_id) const override {
		return any_cast<double>(inputs[0]->getData()) * any_cast<double>(inputs[1]->getData());
	};
};

struct DIV_Node : MATH_Node {
	DIV_Node(const QPoint& pos = QPoint(0, 0)) : MATH_Node(pos) { label = "Div"; };
	any getData(const string& slot_id) const override {
		return any_cast<double>(inputs[0]->getData()) / any_cast<double>(inputs[1]->getData());
	};
};

struct VIEW_Node : Node {
	VIEW_Node(const QPoint& pos = QPoint(0, 0));

	void build() override;
	void exec(const string& slot_id) override;
	any getData(const string& slot_id) const override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

struct REROUTE_Node : Node {
	REROUTE_Node(const QPoint& pos = QPoint(0, 0));

	void build() override;
	any getData(const string& slot_id) const override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

struct EXEC_Node : Node {
	QTimer timer;

	EXEC_Node(const QPoint& pos = QPoint(0, 0));

	void build() override;
	void exec(const string& slot_id) override;
};

struct SEQUENCE_Node : Node {
	GUI::Button* add;
	uint8 count;

	SEQUENCE_Node(const QPoint& pos = QPoint(0, 0));

	void build() override;
	void exec(const string& slot_id) override;
};

struct TICK_COUNTER_Node : Node {
	TICK_COUNTER_Node(const QPoint& pos = QPoint(0, 0));

	uint64 count;

	void build() override;
	void exec(const string& slot_id) override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};

struct SCRIPT_Node : Node {
	SCRIPT_Node(const QPoint& pos = QPoint(0, 0));
	string script_id;
	map<string, any> internal_data;

	Exec_I_Port* in;
	Exec_O_Port* out;

	map<string, Data_I_Port*> data_inputs;
	map<string, Data_O_Port*> data_outputs;
	map<string, Exec_I_Port*> exec_inputs;
	map<string, Exec_O_Port*> exec_outputs;
	virtual any  getInputData (const string& slot_id) const;
	virtual void addDataInput (const string& slot_id, const string& label); // VIRTUAL required for functions used/called from DLL
	virtual void addDataOutput(const string& slot_id, const string& label);
	virtual void addExecInput (const string& slot_id, const string& label);
	virtual void addExecOutput(const string& slot_id, const string& label);
	virtual void clearIO();
	void reloadFunctions();
	void reloadDll();

private:
	GUI::Value_Input* script_identifier;
	GUI::Button* reload;

	uint8 count_in;
	uint8 count_out;

	HINSTANCE dynlib;

	void build() override;
	void exec(const string& slot_id) override;
	any getData(const string& slot_id) const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	any(*getDataFunc)(const SCRIPT_Node*, const string&); // (Inputs, this, Out port Request) -> data
	void (*execFunc)(SCRIPT_Node*); // (Inputs, this, Out port Request) -> data
	void (*buildFunc)(SCRIPT_Node*);
};