#pragma once

#include "Include.hpp"

struct GPU_Scene;

struct Session {
	static Session& getInstance();

	void setScene(GPU_Scene* ptr);
	GPU_Scene* getScene();

private:
	Session() : scene(nullptr) {}
	~Session() = default;

	Session(const Session&) = delete;
	Session& operator=(const Session&) = delete;

	GPU_Scene* scene;
};

#define COUT_S Lace lace; lace <<
#define COUT_E ; cout << lace.str() << endl;