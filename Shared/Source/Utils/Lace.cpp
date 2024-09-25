#include "Utils/Lace.hpp"

KL::Lace::Lace() :
	character(" ")
{
	data = stringstream();
	current_tab = 0;
}

KL::Lace::Lace(const string& character) :
	character(character)
{
	data = stringstream();
	current_tab = 0;
}

KL::Lace& KL::Lace::operator<<(const KL::Lace_S& val) {
	uint16 count = val.count;
	while (count--)
		data << " ";
	return *this;
}

KL::Lace& KL::Lace::operator<<(const KL::Lace_NL& val) {
	uint16 count = val.count;
	uint16 tabs = current_tab;
	while (count--)
		data << "\n";
	if (val.count)
		while (tabs--)
			data << character;
	return *this;
}

KL::Lace& KL::Lace::operator<<(const KL::Lace_TAB& val) {
	uint16 count = current_tab + val.count;
	while (count--)
		data << "\t";
	return *this;
}

KL::Lace& KL::Lace::operator<<(const KL::Lace_CHR& val) {
	uint16 count = val.count;
	while (count--)
		data << character;
	return *this;
}

KL::Lace& KL::Lace::operator<<(const KL::Lace_DEL& val) {
	const string temp_data = data.str();
	data.clear();
	if (val.count < temp_data.size())
		data << temp_data.substr(val.count);
	return *this;
}

KL::Lace& KL::Lace::operator<<(const KL::Lace_POP& val) {
	const string temp_data = data.str();
	data.clear();
	if (val.count < temp_data.size())
		data << temp_data.substr(0, temp_data.size() - 1 - val.count);
	return *this;
}

KL::Lace& KL::Lace::operator<<(const KL::Lace& value) {
	data << value.data.str();
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const KL::Lace& value) {
	data << " " << value.data.str();
	return *this;
}

KL::Lace&  KL::Lace::operator+=(const uint16& value) {
	current_tab += value;
	return *this;
}

KL::Lace&  KL::Lace::operator-=(const uint16& value) {
	current_tab -= value;
	return *this;
}

KL::Lace&  KL::Lace::operator++(int) {
	current_tab++;
	return *this;
}

KL::Lace&  KL::Lace::operator--(int) {
	current_tab--;
	return *this;
}

KL::Lace& KL::Lace::clear() {
	data = stringstream();
	current_tab = 0;
	return *this;
}

string KL::Lace::str() const {
	return data.str();
}

KL::Lace&  KL::Lace::operator<< (const stringstream& value) {
	data << value.str();
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const bool& value) {
	if (value == true) data << "true";
	else data << "false";
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const string& value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const char* value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const int8& value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const int16& value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const int32& value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const int64& value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const uint8& value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const uint16& value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const uint32& value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const uint64& value) {
	data << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const ivec2& value) {
	data << value.x << " " << value.y;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const ivec3& value) {
	data << value.x << " " << value.y << " " << value.z;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const ivec4& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const uvec2& value) {
	data << value.x << " " << value.y;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const uvec3& value) {
	data << value.x << " " << value.y << " " << value.z;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const uvec4& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const vec1& value) {
	data << f_to_str(value);
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const vec2& value) {
	data << f_to_str(value.x) << " " << f_to_str(value.y);
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const vec3& value) {
	data << f_to_str(value.x) << " " << f_to_str(value.y) << " " << f_to_str(value.z);
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const vec4& value) {
	data << f_to_str(value.x) << " " << f_to_str(value.y) << " " << f_to_str(value.z) << " " << f_to_str(value.w);
	return *this;
}

KL::Lace&  KL::Lace::operator<<(const quat& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

KL::Lace&  KL::Lace::operator<<(const mat2& value) {
	data << f_to_str(value[0][0]) << " " << f_to_str(value[0][1]) << " "
		 << f_to_str(value[1][0]) << " " << f_to_str(value[1][1]);
	return *this;
}

KL::Lace&  KL::Lace::operator<<(const mat3& value) {
	data << f_to_str(value[0][0]) << " " << f_to_str(value[0][1]) << " " << f_to_str(value[0][2]) << " "
		 << f_to_str(value[1][0]) << " " << f_to_str(value[1][1]) << " " << f_to_str(value[1][2]) << " "
		 << f_to_str(value[2][0]) << " " << f_to_str(value[2][1]) << " " << f_to_str(value[2][2]);
	return *this;
}

KL::Lace&  KL::Lace::operator<<(const mat4& value) {
	data << f_to_str(value[0][0]) << " " << f_to_str(value[0][1]) << " " << f_to_str(value[0][2]) << " " << f_to_str(value[0][3]) << " "
		 << f_to_str(value[1][0]) << " " << f_to_str(value[1][1]) << " " << f_to_str(value[1][2]) << " " << f_to_str(value[1][3]) << " "
		 << f_to_str(value[2][0]) << " " << f_to_str(value[2][1]) << " " << f_to_str(value[2][2]) << " " << f_to_str(value[2][3]) << " "
		 << f_to_str(value[3][0]) << " " << f_to_str(value[3][1]) << " " << f_to_str(value[3][2]) << " " << f_to_str(value[3][3]);
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const dvec1& value) {
	data << d_to_str(value);
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const dvec2& value) {
	data << d_to_str(value.x) << " " << d_to_str(value.y);
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const dvec3& value) {
	data << d_to_str(value.x) << " " << d_to_str(value.y) << " " << d_to_str(value.z);
	return *this;
}

KL::Lace&  KL::Lace::operator<< (const dvec4& value) {
	data << d_to_str(value.x) << " " << d_to_str(value.y) << " " << d_to_str(value.z) << " " << d_to_str(value.w);
	return *this;
}

KL::Lace&  KL::Lace::operator<<(const dquat& value) {
	data << value.x << " " << value.y << " " << value.z << " " << value.w;
	return *this;
}

KL::Lace&  KL::Lace::operator<<(const dmat2& value) {
	data << d_to_str(value[0][0]) << " " << d_to_str(value[0][1]) << " "
		 << d_to_str(value[1][0]) << " " << d_to_str(value[1][1]);
	return *this;
}

KL::Lace&  KL::Lace::operator<<(const dmat3& value) {
	data << d_to_str(value[0][0]) << " " << d_to_str(value[0][1]) << " " << d_to_str(value[0][2]) << " "
		 << d_to_str(value[1][0]) << " " << d_to_str(value[1][1]) << " " << d_to_str(value[1][2]) << " "
		 << d_to_str(value[2][0]) << " " << d_to_str(value[2][1]) << " " << d_to_str(value[2][2]);
	return *this;
}

KL::Lace&  KL::Lace::operator<<(const dmat4& value) {
	data << d_to_str(value[0][0]) << " " << d_to_str(value[0][1]) << " " << d_to_str(value[0][2]) << " " << d_to_str(value[0][3]) << " "
		 << d_to_str(value[1][0]) << " " << d_to_str(value[1][1]) << " " << d_to_str(value[1][2]) << " " << d_to_str(value[1][3]) << " "
		 << d_to_str(value[2][0]) << " " << d_to_str(value[2][1]) << " " << d_to_str(value[2][2]) << " " << d_to_str(value[2][3]) << " "
		 << d_to_str(value[3][0]) << " " << d_to_str(value[3][1]) << " " << d_to_str(value[3][2]) << " " << d_to_str(value[3][3]);
	return *this;
}

KL::Lace&  KL::Lace::operator>> (const bool& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>> (const char* value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const float& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const double& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const int8& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const int16& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const int32& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const int64& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const uint8& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const uint16& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const uint32& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator>>(const uint64& value) {
	data << " " << value;
	return *this;
}

KL::Lace&  KL::Lace::operator<<(const vector<string>& value) {
	for (string val : value)
		data << val << " ";
	return *this;
}

string KL::d_to_str(const dvec1& value) {
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

string KL::f_to_str(const vec1& value) {
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

template<>
string KL::f_readBinary<string>(const vector<std::byte>& data, const uint64& start, const uint64& size) {
	return string(reinterpret_cast<const char*>(&data[start]), size);
}

template<>
vector<Byte> KL::f_toBinary<string>(const string& value) {
	vector<Byte> byteVector;
	byteVector.reserve(value.size());
	for (char ch : value) {
		byteVector.push_back(static_cast<Byte>(ch));
	}
	return byteVector;
}

KL::Bin_Lace::Bin_Lace() {
	data = {};
}

KL::Bin_Lace& KL::Bin_Lace::operator<<(const KL::Bin_Lace& value) {
	data.reserve(value.data.size() + data.size());
	data.insert(data.end(), value.data.begin(), value.data.end());
	return *this;
}