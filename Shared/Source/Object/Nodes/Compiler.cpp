#include "Object/Nodes/Compiler.hpp"

#define DLL_PATH "D:/Kerzenlicht/x64/Debug/Scripting.dll"
#define DLL_GUI_PATH "D:/Kerzenlicht/x64/Debug/GUI-Scripting.dll"

void loadDLL(HINSTANCE& dynlib, const bool& gui) {
	string libfile = "";
	if (gui) libfile = DLL_GUI_PATH;
	else libfile = DLL_PATH;
	wstring stemp = wstring(libfile.begin(), libfile.end());
	LPCWSTR file_path = stemp.c_str();
	LOG ENDL ANSI_PURPLE << "[DLL Loading]" ANSI_RESET << " Loading: " << libfile << "..."; FLUSH;
	dynlib = LoadLibrary(file_path);
	if (!dynlib) {
		const uint32 errorCode = static_cast<uint32>(GetLastError());
		LOG ENDL ANSI_R << "[DLL Loading]" ANSI_RESET << " LoadLibrary failed with error code:  " << errorCode; FLUSH;
		exit(EXIT_FAILURE);
	}
	LOG ENDL ANSI_G << "[DLL Loading]" ANSI_RESET << " Loaded"; FLUSH;
}

void recompileDLL(HINSTANCE& dynlib, const bool& gui) {
	if (dynlib != NULL) {
		FreeLibrary(dynlib);
	}
	string rebuildCommand = "C:/Programs/Coding/Microsoft/VisualStudio/Community/MSBuild/Current/Bin/MSBuild.exe /p:Project=Scripting /t:Build \"D:/Kerzenlicht/Kerzenlicht.sln\"";
	const int result = std::system(rebuildCommand.c_str());
	if (result == 0) {
		LOG ENDL ANSI_G << "[DLL Compilation]" ANSI_RESET << " Succesfully Compiled"; FLUSH;
	}
	loadDLL(dynlib);
}

void unloadDLL(HINSTANCE& dynlib) {
	if (dynlib != NULL) {
		FreeLibrary(dynlib);
	}
}