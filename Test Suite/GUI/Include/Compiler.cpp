#include "Compiler.hpp"

void loadDLL(HINSTANCE& dynlib, const string& libfile) {
	wstring stemp = wstring(libfile.begin(), libfile.end());
	LPCWSTR file_path = stemp.c_str();
	cout << "Loading [" << libfile << "]..." << endl;
	dynlib = LoadLibrary(file_path);
	if (!dynlib) {
		const DWORD errorCode = GetLastError();
		cerr << "LoadLibrary failed with error code:  " << errorCode << endl;
		exit(EXIT_FAILURE);
	}
}

void recompileDLL(HINSTANCE& dynlib, const string& libfile) {
	if (dynlib != NULL) FreeLibrary(dynlib);
	string rebuildCommand = "C:/Programs/Coding/Microsoft/VisualStudio/Community/MSBuild/Current/Bin/MSBuild.exe \"D:/Kerzenlicht Renderer/Test Suite/GUI/Test Suite.sln\" /t:DLL:Rebuild /p:Configuration=Release";
	int result = std::system(rebuildCommand.c_str());
	loadDLL(dynlib, libfile);
}

void unloadDLL(HINSTANCE& dynlib) {
	if (dynlib != NULL) FreeLibrary(dynlib);
}