#pragma once

#include "Shared.hpp"

void loadDLL(HINSTANCE& dynlib, const bool& gui = false);
void recompileDLL(HINSTANCE& dynlib, const bool& gui = false);
void unloadDLL(HINSTANCE& dynlib);