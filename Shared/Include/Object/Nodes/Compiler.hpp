#pragma once

#include "Include.hpp"

// FWD DECL OTHER

// FWD DECL THIS

// DECL
void loadDLL(HINSTANCE& dynlib, const bool& gui = false);
void recompileDLL(HINSTANCE& dynlib, const bool& gui = false);
void unloadDLL(HINSTANCE& dynlib);