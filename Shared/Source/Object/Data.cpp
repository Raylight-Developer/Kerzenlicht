#include "Object/Data.hpp"

CLASS::OBJECT::Data::Data() :
	name("New Data"),
	type(CLASS::OBJECT::DATA::Type::NONE),
	data(nullptr)
{}

CLASS::OBJECT::Data::Data(const CLASS::OBJECT::DATA::Type& type, void* data) :
	name("New Data"),
	type(type),
	data(data)
{}