#include "Core/Compiler.hpp"

#include "Log_Console.hpp"
#include "Core/Session.hpp"

void loadDLL(HINSTANCE& dynlib, const string& libfile) {
	wstring stemp = wstring(libfile.begin(), libfile.end());
	LPCWSTR file_path = stemp.c_str();
	*LOG << ENDL << HTML_MAGENTA << "[DLL]" << HTML_RESET << " Loading: " << libfile << "..."; FLUSH
	dynlib = LoadLibrary(file_path);
	if (!dynlib) {
		const uint32 errorCode = static_cast<uint32>(GetLastError());
		*LOG << ENDL << HTML_RED << "[DLL]" << HTML_RESET << "LoadLibrary failed with error code:  " << errorCode; FLUSH
		exit(EXIT_FAILURE);
	}
	*LOG << ENDL << HTML_GREEN << "[DLL]" << HTML_RESET << " Loaded"; FLUSH
}

void recompileDLL(HINSTANCE& dynlib, const string& libfile) {
	if (dynlib != NULL) FreeLibrary(dynlib);
	string rebuildCommand = "C:/Programs/Coding/Microsoft/VisualStudio/Community/MSBuild/Current/Bin/MSBuild.exe \"./Kerzenlicht Renderer.sln\" /t:Runtime:Build /p:Configuration=Release";
	int result = std::system(rebuildCommand.c_str());
	loadDLL(dynlib, libfile);
}

void unloadDLL(HINSTANCE& dynlib) {
	if (dynlib != NULL) FreeLibrary(dynlib);
}