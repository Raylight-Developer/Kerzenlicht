#pragma once

#include "Include.hpp"

void loadDLL(HINSTANCE& dynlib, const string& libfile);

void recompileDLL(HINSTANCE& dynlib, const string& libfile);

void unloadDLL(HINSTANCE& dynlib);