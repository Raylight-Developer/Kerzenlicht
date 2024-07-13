#include "Core/Compiler.hpp"

#include "Log_Console.hpp"
#include "Core/Session.hpp"

#define DLL_PATH "D:/Kerzenlicht/x64/Debug/Scripting.dll"

void loadDLL(HINSTANCE& dynlib) {
	const string libfile = DLL_PATH;
	wstring stemp = wstring(libfile.begin(), libfile.end());
	LPCWSTR file_path = stemp.c_str();
	*LOG << ENDL << HTML_MAGENTA << "[DLL Loading]" << HTML_RESET << " Loading: " << libfile << "..."; FLUSH
	dynlib = LoadLibrary(file_path);
	if (!dynlib) {
		const uint32 errorCode = static_cast<uint32>(GetLastError());
		*LOG << ENDL << HTML_RED << "[DLL Loading]" << HTML_RESET << "LoadLibrary failed with error code:  " << errorCode; FLUSH
		exit(EXIT_FAILURE);
	}
	*LOG << ENDL << HTML_GREEN << "[DLL Loading]" << HTML_RESET << " Loaded"; FLUSH
}

void recompileDLL(HINSTANCE& dynlib) {
	if (dynlib != NULL) {
		FreeLibrary(dynlib);
	}
	string rebuildCommand = "C:/Programs/Coding/Microsoft/VisualStudio/Community/MSBuild/Current/Bin/MSBuild.exe \"./Kerzenlicht.sln\" /t:Scripting:Build /p:Configuration=Release";
	const int result = std::system(rebuildCommand.c_str());
	if (result == 0) {
		*LOG << ENDL << HTML_GREEN << "[DLL Compilation]" << HTML_RESET << " Succesfully Compiled"; FLUSH
	}
	loadDLL(dynlib);
}

void unloadDLL(HINSTANCE& dynlib) {
	if (dynlib != NULL) {
		FreeLibrary(dynlib);
	}
}