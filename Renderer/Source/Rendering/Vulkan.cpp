#include "Rendering/Vulkan.hpp"

void compileSPIRVShader(const string& source_name, const string& compile_name) {
	string path_str = filesystem::current_path().string();
	std::replace(path_str.begin(), path_str.end(), '\\', '/');

	Lace command;
	command << "\"C:/Programs/Coding/VulkanSDK/1.3.261.1/Bin/glslc.exe\" \"" << path_str << "/" << source_name << "\" -o \"" << path_str << "/" << compile_name << "\"";
	cout << command.str() << endl;
	int returnCode = system(command.str().c_str());
}