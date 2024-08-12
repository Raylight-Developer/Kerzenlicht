#include "Object/Data.hpp"

KL::OBJECT::Data::Data() :
	name("New Data"),
	type(KL::OBJECT::DATA::Type::NONE),
	data(nullptr)
{}

KL::OBJECT::Data::Data(const KL::OBJECT::DATA::Type& type, void* data) :
	name("New Data"),
	type(type),
	data(data)
{}