#pragma once

#include "Shared.hpp"

void loadDLL(HINSTANCE& dynlib);
void recompileDLL(HINSTANCE& dynlib);
void unloadDLL(HINSTANCE& dynlib);