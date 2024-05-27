#include "Core/Lace.hpp"

Lace::Lace() :
	character("\t")
{
	data = stringstream();
	current_tab = 0;
}

Lace::Lace(const string& character) :
	character(character)
{
	data = stringstream();
	current_tab = 0;
}

Lace& operator<<(Lace& lace, const S& val) {
	uint16 count = val.count;
	while (count--)
		lace << " ";
	return lace;
}

Lace& operator<<(Lace& lace, const NL& val) {
	uint16 count = val.count;
	uint16 tabs = lace.current_tab;
	while (count--)
		lace << "\n";
	if (val.count)
		while (tabs--)
			lace << lace.character;
	return lace;
}

Lace& operator<<(Lace& lace, const TAB& val) {
	uint16 count = lace.current_tab + val.count;
	while (count--)
		lace << "\t";
	return lace;
}

Lace& operator<<(Lace& lace, const CHR& val) {
	uint16 count = val.count;
	while (count--)
		lace << lace.character;
	return lace;
}

Lace& operator<<(Lace& lace, const DEL& val) {
	const string data = lace.data.str();
	lace.data.clear();
	if (val.count < data.size())
		lace.data << data.substr(val.count);
	return lace;
}

Lace& operator<<(Lace& lace, const POP& val) {
	const string data = lace.data.str();
	lace.data.clear();
	if (val.count < data.size())
		lace.data << data.substr(0, data.size() - 1 - val.count);
	return lace;
}

Lace& Lace::operator<<(const Lace& value) {
	data << value.data.str();
	return *this;
}

Lace& Lace::operator>>(const Lace& value) {
	data << " " << value.data.str();
	return *this;
}

Lace& Lace::operator+=(const uint16& value) {
	current_tab += value;
	return *this;
}

Lace& Lace::operator-=(const uint16& value) {
	current_tab -= value;
	return *this;
}

string Lace::str() const {
	return data.str();
}

Lace& Lace::operator<< (const stringstream& value) {
	data << value.str();
	return *this;
}

Lace& Lace::operator<< (const bool& value) {
	if (value == true) data << "true";
	else data << "false";
	return *this;
}

Lace& Lace::operator<< (const string& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const char* value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const int8& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const int16& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const int32& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const int64& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const uint8& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const uint16& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const uint32& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const uint64& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const ivec2& value) {
	data << value.x << " " << value.y;
	return *this;
}

Lace& Lace::operator<< (const ivec3& value) {
	data << value.x << " " << value.y << " " << value.z;
	return *this;
}

Lace& Lace::operator<< (const ivec4& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

Lace& Lace::operator<< (const uvec2& value) {
	data << value.x << " " << value.y;
	return *this;
}

Lace& Lace::operator<< (const uvec3& value) {
	data << value.x << " " << value.y << " " << value.z;
	return *this;
}

Lace& Lace::operator<< (const uvec4& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

Lace& Lace::operator<< (const vec1& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const vec2& value) {
	data << value.x << " " << value.y;
	return *this;
}

Lace& Lace::operator<< (const vec3& value) {
	data << value.x << " " << value.y << " " << value.z;
	return *this;
}

Lace& Lace::operator<< (const vec4& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

Lace& Lace::operator<<(const quat& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

Lace& Lace::operator<<(const mat2& value) {
	data << value[0][0] << " " << value[0][1] << " "
		 << value[1][0] << " " << value[1][1];
	return *this;
}

Lace& Lace::operator<<(const mat3& value) {
	data << value[0][0] << " " << value[0][1] << " " << value[0][2] << " "
		 << value[1][0] << " " << value[1][1] << " " << value[1][2] << " "
		 << value[2][0] << " " << value[2][1] << " " << value[2][2];
	return *this;
}

Lace& Lace::operator<<(const mat4& value) {
	data << value[0][0] << " " << value[0][1] << " " << value[0][2] << " " << value[0][3] << " "
		 << value[1][0] << " " << value[1][1] << " " << value[1][2] << " " << value[1][3] << " "
		 << value[2][0] << " " << value[2][1] << " " << value[2][2] << " " << value[2][3] << " "
		 << value[3][0] << " " << value[3][1] << " " << value[3][2] << " " << value[3][3];
	return *this;
}

Lace& Lace::operator<< (const dvec1& value) {
	data << value;
	return *this;
}

Lace& Lace::operator<< (const dvec2& value) {
	data << value.x << " " << value.y;
	return *this;
}

Lace& Lace::operator<< (const dvec3& value) {
	data << value.x << " " << value.y << " " << value.z;
	return *this;
}

Lace& Lace::operator<< (const dvec4& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

Lace& Lace::operator<<(const dquat& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

Lace& Lace::operator<<(const dmat2& value) {
	data << value[0][0] << " " << value[0][1] << " "
		 << value[1][0] << " " << value[1][1];
	return *this;
}

Lace& Lace::operator<<(const dmat3& value) {
	data << value[0][0] << " " << value[0][1] << " " << value[0][2] << " "
		 << value[1][0] << " " << value[1][1] << " " << value[1][2] << " "
		 << value[2][0] << " " << value[2][1] << " " << value[2][2];
	return *this;
}

Lace& Lace::operator<<(const dmat4& value) {
	data << value[0][0] << " " << value[0][1] << " " << value[0][2] << " " << value[0][3] << " "
		 << value[1][0] << " " << value[1][1] << " " << value[1][2] << " " << value[1][3] << " "
		 << value[2][0] << " " << value[2][1] << " " << value[2][2] << " " << value[2][3] << " "
		 << value[3][0] << " " << value[3][1] << " " << value[3][2] << " " << value[3][3];
	return *this;
}

Lace& Lace::operator<<(const QPointF& value) {
	data << value.x() << " " << value.y();
	return *this;
}

Lace& Lace::operator>> (const bool& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>> (const char* value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const float& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const double& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const int8& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const int16& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const int32& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const int64& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const uint8& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const uint16& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const uint32& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator>>(const uint64& value) {
	data << " " << value;
	return *this;
}

Lace& Lace::operator<<(const vector<string>& value) {
	for (string val : value)
		data << val << " ";
	return *this;
}