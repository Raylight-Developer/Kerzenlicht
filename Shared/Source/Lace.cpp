#include "Lace.hpp"

#if defined(COMPILE_EDITOR) || defined(COMPILE_SCRIPTING)
	Lace& Lace::operator<<(const QPointF& value) {
		data << value.x() << " " << value.y();
		return *this;
	}

	Lace& Lace::operator<<(const QString& value) {
		data << " " << value.toStdString();
		return *this;
	}
#endif

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

Lace& Lace::clear() {
	data = stringstream();
	current_tab = 0;
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
	data << f_to_str(value);
	return *this;
}

Lace& Lace::operator<< (const vec2& value) {
	data << f_to_str(value.x) << " " << f_to_str(value.y);
	return *this;
}

Lace& Lace::operator<< (const vec3& value) {
	data << f_to_str(value.x) << " " << f_to_str(value.y) << " " << f_to_str(value.z);
	return *this;
}

Lace& Lace::operator<< (const vec4& value) {
	data << f_to_str(value.x) << " " << f_to_str(value.y) << " " << f_to_str(value.z) << " " << f_to_str(value.w);
	return *this;
}

Lace& Lace::operator<<(const quat& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

Lace& Lace::operator<<(const mat2& value) {
	data << f_to_str(value[0][0]) << " " << f_to_str(value[0][1]) << " "
		 << f_to_str(value[1][0]) << " " << f_to_str(value[1][1]);
	return *this;
}

Lace& Lace::operator<<(const mat3& value) {
	data << f_to_str(value[0][0]) << " " << f_to_str(value[0][1]) << " " << f_to_str(value[0][2]) << " "
		 << f_to_str(value[1][0]) << " " << f_to_str(value[1][1]) << " " << f_to_str(value[1][2]) << " "
		 << f_to_str(value[2][0]) << " " << f_to_str(value[2][1]) << " " << f_to_str(value[2][2]);
	return *this;
}

Lace& Lace::operator<<(const mat4& value) {
	data << f_to_str(value[0][0]) << " " << f_to_str(value[0][1]) << " " << f_to_str(value[0][2]) << " " << f_to_str(value[0][3]) << " "
		 << f_to_str(value[1][0]) << " " << f_to_str(value[1][1]) << " " << f_to_str(value[1][2]) << " " << f_to_str(value[1][3]) << " "
		 << f_to_str(value[2][0]) << " " << f_to_str(value[2][1]) << " " << f_to_str(value[2][2]) << " " << f_to_str(value[2][3]) << " "
		 << f_to_str(value[3][0]) << " " << f_to_str(value[3][1]) << " " << f_to_str(value[3][2]) << " " << f_to_str(value[3][3]);
	return *this;
}

Lace& Lace::operator<< (const dvec1& value) {
	data << d_to_str(value);
	return *this;
}

Lace& Lace::operator<< (const dvec2& value) {
	data << d_to_str(value.x) << " " << d_to_str(value.y);
	return *this;
}

Lace& Lace::operator<< (const dvec3& value) {
	data << d_to_str(value.x) << " " << d_to_str(value.y) << " " << d_to_str(value.z);
	return *this;
}

Lace& Lace::operator<< (const dvec4& value) {
	data << d_to_str(value.x) << " " << d_to_str(value.y) << " " << d_to_str(value.z) << " " << d_to_str(value.w);
	return *this;
}

Lace& Lace::operator<<(const dquat& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

Lace& Lace::operator<<(const dmat2& value) {
	data << d_to_str(value[0][0]) << " " << d_to_str(value[0][1]) << " "
		 << d_to_str(value[1][0]) << " " << d_to_str(value[1][1]);
	return *this;
}

Lace& Lace::operator<<(const dmat3& value) {
	data << d_to_str(value[0][0]) << " " << d_to_str(value[0][1]) << " " << d_to_str(value[0][2]) << " "
		 << d_to_str(value[1][0]) << " " << d_to_str(value[1][1]) << " " << d_to_str(value[1][2]) << " "
		 << d_to_str(value[2][0]) << " " << d_to_str(value[2][1]) << " " << d_to_str(value[2][2]);
	return *this;
}

Lace& Lace::operator<<(const dmat4& value) {
	data << d_to_str(value[0][0]) << " " << d_to_str(value[0][1]) << " " << d_to_str(value[0][2]) << " " << d_to_str(value[0][3]) << " "
		 << d_to_str(value[1][0]) << " " << d_to_str(value[1][1]) << " " << d_to_str(value[1][2]) << " " << d_to_str(value[1][3]) << " "
		 << d_to_str(value[2][0]) << " " << d_to_str(value[2][1]) << " " << d_to_str(value[2][2]) << " " << d_to_str(value[2][3]) << " "
		 << d_to_str(value[3][0]) << " " << d_to_str(value[3][1]) << " " << d_to_str(value[3][2]) << " " << d_to_str(value[3][3]);
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

string d_to_str(const dvec1& value) {
	ostringstream oss;
	oss << fixed;
	oss << setprecision(15) << value;

	string result = oss.str();

	if (result.find('.') != string::npos) {
		result.erase(result.find_last_not_of('0') + 1, string::npos);

		if (result.back() == '.') {
			result.pop_back();
		}
	}

	if (result.find('.') == string::npos) {
		result += ".0";
	}

	return result;
}

string f_to_str(const vec1& value) {
	ostringstream oss;
	oss << fixed;
	oss << setprecision(8) << value;

	string result = oss.str();

	if (result.find('.') != string::npos) {
		result.erase(result.find_last_not_of('0') + 1, string::npos);

		if (result.back() == '.') {
			result.pop_back();
		}
	}

	if (result.find('.') == string::npos) {
		result += ".0";
	}

	return result;
}