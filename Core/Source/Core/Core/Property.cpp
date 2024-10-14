#include "Core/Core/Property.hpp"

CORE::Prop::Prop() :
	data(nullptr),
	type(PROP::Type::NONE),
	modifier(PROP::Modifier::SINGLE)
{}

CORE::Prop::Prop(const any& data, const PROP::Type& type, const PROP::Modifier& modifier) :
	data(data),
	type(type),
	modifier(modifier)
{}

CORE::Prop::Prop(const string& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::STRING;
}
CORE::Prop::Prop(const dvec1& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::DOUBLE;
}
CORE::Prop::Prop(const bool& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::BOOL;
}
CORE::Prop::Prop(const uint64& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::UINT;
}
CORE::Prop::Prop(const int64& data) : data(data) {
	modifier = PROP::Modifier::SINGLE; type = PROP::Type::INT;
}
CORE::Prop::Prop(vector<dvec1> data) : data(data) {
	modifier = PROP::Modifier::VECTOR; type = PROP::Type::DOUBLE;
}

CORE::Prop CORE::Prop::operator+(const CORE::Prop & other) const {
	if (type == other.type) {
		switch (type) {
			case PROP::Type::DOUBLE: return CORE::Prop(getDouble() + other.getDouble(), type);
		}
	}
	switch (type) {
		case PROP::Type::DOUBLE: {
			switch (other.type) {
				case PROP::Type::INT: {
					return CORE::Prop(getDouble() + other.getInt(), type);
				}
				case PROP::Type::UINT: {
					return CORE::Prop(getDouble() + other.getUint(), type);
				}
			}
		}
	}
	return CORE::Prop();
}
CORE::Prop CORE::Prop::operator-(const CORE::Prop& other) const {
	if (type == other.type) {
		switch (type) {
			case PROP::Type::DOUBLE: return CORE::Prop(getDouble() - other.getDouble(), type);
		}
	}
	return CORE::Prop();
}

CORE::Prop CORE::Prop::operator*(const CORE::Prop& other) const {
	if (type == other.type) {
		switch (type) {
		case PROP::Type::DOUBLE: return CORE::Prop(getDouble() * other.getDouble(), type);
		}
	}
	return CORE::Prop();
}

CORE::Prop CORE::Prop::operator/(const CORE::Prop& other) const {
	if (type == other.type) {
		switch (type) {
			case PROP::Type::DOUBLE: return CORE::Prop(getDouble() / other.getDouble(), type);
		}
	}
	return CORE::Prop();
}

CORE::Prop CORE::Prop::pow(const Prop& other) const {
	if (type == other.type) {
		switch (type) {
			case PROP::Type::DOUBLE: return CORE::Prop(std::pow(getDouble(), other.getDouble()), type);
		}
	}
	return CORE::Prop();
}

int64 CORE::Prop::getInt() const {
	return any_cast<int64>(data);
}

uint64 CORE::Prop::getUint() const {
	return any_cast<uint64>(data);
}

dvec1 CORE::Prop::getDouble() const {
	switch (type) {
	case PROP::Type::DOUBLE: return any_cast<dvec1>(data);
	case PROP::Type::UINT: return static_cast<dvec1>(any_cast<uint64>(data));
	case PROP::Type::INT: return static_cast<dvec1>(any_cast<int64>(data));
	}
	return 0.0;
}

string CORE::Prop::getStr() const {
	return any_cast<string>(data);
}

vector<dvec1> CORE::Prop::getDoubleVector() const {
	return any_cast<vector<dvec1>>(data);
}

string CORE::Prop::to_string() const {
	switch (type) {
	case PROP::Type::DOUBLE:    return std::to_string(any_cast<dvec1>(data));
	case PROP::Type::UINT:      return std::to_string(static_cast<dvec1>(any_cast<uint64>(data)));
	case PROP::Type::INT:       return std::to_string(static_cast<dvec1>(any_cast<int64>(data)));
	}
	return "";
}