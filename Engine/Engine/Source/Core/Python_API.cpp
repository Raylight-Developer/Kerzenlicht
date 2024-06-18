#include "Core/Python_API.hpp"

#include "Object/Object.hpp"
#include "Main_Window.hpp"
#include "Core/File.hpp"

using namespace CLASS;

PYBIND11_EMBEDDED_MODULE(KL, Module) {
	Py::class_<Lace>(Module, "Lace")
		.def(Py::init<>())
		.def("str", &Lace::str);

	Py::class_<Scene>(Module, "Scene")
		.def(Py::init<>())
		.def_readwrite("active_camera", & Scene::active_camera)
		.def_readwrite("current_frame", & Scene::current_frame)
		.def_readwrite("objects", & Scene::objects);

	Py::class_<File>(Module, "File")
		.def(Py::init<>())
		.def_readwrite("object_data", &File::object_data)
		.def_readwrite("materials", &File::materials)
		.def_readwrite("objects", &File::objects)
		.def_readwrite("scenes", &File::scenes)
		.def_readwrite("selected_objects", &File::selected_objects)
		.def_readwrite("active_object", &File::active_object)
		.def_readwrite("active_scene", &File::active_scene)
		.def_readwrite("version", &File::version)
		.def("f_printFile", &File::f_printFile)
		.def("f_saveFile", &File::f_saveFile);

	Py::class_<GUI::WORKSPACE::Main_Window>(Module, "Main_Window")
		.def_readwrite("file", &GUI::WORKSPACE::Main_Window::file)
		.def_readwrite("app", &GUI::WORKSPACE::Main_Window::app)
		.def_readwrite("log", &GUI::WORKSPACE::Main_Window::log);
}