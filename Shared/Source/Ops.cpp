#include "Ops.hpp"

vector<string> f_split(const string& input) {
	vector<string> result;
	istringstream iss(input);
	string token;
	while (iss >> token) {
		result.push_back(token);
	}
	return result;
}

vector<string> f_split(const string& input, const string& delimiter) {
	vector<string> tokens;
	size_t start = 0, end = 0;
	while ((end = input.find(delimiter, start)) != string::npos) {
		tokens.push_back(input.substr(start, end - start));
		start = end + delimiter.length();
	}
	tokens.push_back(input.substr(start));
	return tokens;
}

string f_join(const vector<string>& tokens, const string& join, const size_t& start, const size_t& end) {
	return accumulate(
		tokens.begin() + start, tokens.end() - end, string(),
		[join](const string& accumulator, const string& current) {
			return accumulator.empty() ? current : accumulator + join + current;
		}
	);
}

string f_str(const vector<string>& tokens) {
	return accumulate(
		tokens.begin(), tokens.end(), string(),
		[](const string& accumulator, const string& current) {
			return accumulator.empty() ? current : accumulator + " " + current;
		}
	);
}

string f_remove(const string& input, const string& remove) {
	string copy = input;
	size_t pos = copy.find(remove);
	while (pos != string::npos) {
		copy.erase(pos, remove.length());
		pos = copy.find(remove);
	}
	return copy;
}

string f_replace(const string& input, const string& old_str, const string& new_str) {
	string copy = input;
	size_t pos = 0;
	while ((pos = copy.find(old_str, pos)) != string::npos) {
		copy.replace(pos, old_str.length(), new_str);
		pos += new_str.length();
	}
	return copy;
}

string f_strip(const string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	size_t last = str.find_last_not_of(" \t\n\r");
	if (first == string::npos)
		return "";
	return str.substr(first, last - first + 1);
}

string f_loadFromFile(const string& file_path) {
	ifstream in(file_path, ios::binary);
	if (in) {
		string contents;
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw runtime_error(to_string(errno));
}

string f_processSubShader(const string& file_path) {
	ifstream in(file_path, ios::binary);
	if (in) {
		std::stringstream output;
		std::string line;

		while (std::getline(in, line)) {
			size_t includePos = line.find("#include");
			size_t versionPos = line.find("#version");
			size_t extensionPos = line.find("#extension");
			if (includePos == std::string::npos && versionPos == std::string::npos && extensionPos == std::string::npos) {
				const string stripped = f_strip(line);
				if (stripped.substr(0, 2) != "//" and line.size() > 0)
					output << line << endl;
			}
		}
		return output.str();
	}
	throw runtime_error(to_string(errno));
}

string f_preprocessShader(const string& file_path) {
	ifstream in(file_path, ios::binary);
	if (in) {
		std::stringstream output;
		std::string line;

		while (std::getline(in, line)) {
			size_t includePos = line.find("#include");
			if (includePos != std::string::npos) {
				size_t quotePos = line.find("\"", includePos);
				if (quotePos != std::string::npos) {
					size_t endQuotePos = line.find("\"", quotePos + 1);
					if (endQuotePos != std::string::npos) {
						std::string includeFilename = line.substr(quotePos + 1, endQuotePos - quotePos - 1);
						output << f_processSubShader("./Resources/Shaders/" + includeFilename) << endl;
						continue;
					}
				}
			}

			const string stripped = f_strip(line);
			if (stripped.substr(0, 2) != "//" and line.size() > 0)
				output << line << endl;
		}
		return output.str();
	}
	throw runtime_error(to_string(errno));
}

CLASS::Transform::Transform(const dvec3& position, const dvec3& rotation, const dvec3& scale, const Rotation_Type& type) :
	rotation_type(type),
	position(position),
	euler_rotation(rotation),
	scale(scale)
{
	quat_rotation = dquat(1.0, 0.0, 0.0, 0.0);
	axis_rotation = dvec3(0.0);
}

CLASS::Transform::Transform(const dvec3& position, const dvec3& axis, const dvec3& rotation, const dvec3& scale, const Rotation_Type& type) :
	rotation_type(type),
	position(position),
	euler_rotation(rotation),
	scale(scale),
	axis_rotation(axis)
{
	quat_rotation = dquat(1.0, 0.0, 0.0, 0.0);
}

CLASS::Transform::Transform(const dvec3& position, const dquat& rotation, const dvec3& scale, const Rotation_Type& type) :
	rotation_type(type),
	position(position),
	quat_rotation(rotation),
	scale(scale)
{
	euler_rotation = dvec3(0.0);
	axis_rotation = dvec3(0.0);
}
// TODO account for different rotation modes
CLASS::Transform CLASS::Transform::operator+(const Transform& other) const {
	Transform result;
	result.position       = position       + other.position;
	result.euler_rotation = euler_rotation + other.euler_rotation;
	result.axis_rotation  = axis_rotation  + other.axis_rotation;
	//result.quat_rotation  = quat_rotation  + other.quat_rotation;
	result.scale          = scale          + other.scale;
	return result;
}
CLASS::Transform CLASS::Transform::operator-(const Transform& other) const {
	Transform result;
	result.position       = position       - other.position;
	result.euler_rotation = euler_rotation - other.euler_rotation;
	result.axis_rotation  = axis_rotation  - other.axis_rotation;
	//result.quat_rotation  = quat_rotation  - other.quat_rotation;
	result.scale          = scale          - other.scale;
	return result;
}
CLASS::Transform CLASS::Transform::operator*(const Transform& other) const {
	Transform result;
	result.position       = position       * other.position;
	result.euler_rotation = euler_rotation * other.euler_rotation;
	result.axis_rotation  = axis_rotation  * other.axis_rotation;
	//result.quat_rotation  = quat_rotation  * other.quat_rotation;
	result.scale          = scale          * other.scale;
	return result;
}
CLASS::Transform CLASS::Transform::operator/(const Transform& other) const {
	Transform result;
	result.position       = position       / other.position;
	result.euler_rotation = euler_rotation / other.euler_rotation;
	result.axis_rotation  = axis_rotation  / other.axis_rotation;
	//result.quat_rotation  = quat_rotation  / other.quat_rotation;
	result.scale          = scale          / other.scale;
	return result;
}

CLASS::Transform CLASS::Transform::operator*(const dvec1& other) const {
	Transform result;
	result.position       = position       * other;
	result.euler_rotation = euler_rotation * other;
	result.axis_rotation  = axis_rotation  * other;
	//result.quat_rotation  = quat_rotation  * other;
	result.scale          = scale          * other;
	return result;
}

dmat4 CLASS::Transform::f_compile() const {
	const dmat4 translation_matrix = glm::translate(dmat4(1.0), position);
	const dmat4 scale_matrix = glm::scale(dmat4(1.0), scale);
	dmat4 rotation_matrix = dmat4(1.0);

	switch (rotation_type) {
		case CLASS::Rotation_Type::QUATERNION: {
			rotation_matrix = glm::toMat4(quat_rotation);
			break;
		}
		case CLASS::Rotation_Type::XYZ: {
			const dmat4 rotationX = glm::rotate(dmat4(1.0), euler_rotation.x * DEG_RAD, dvec3(1.0, 0.0, 0.0));
			const dmat4 rotationY = glm::rotate(dmat4(1.0), euler_rotation.y * DEG_RAD, dvec3(0.0, 1.0, 0.0));
			const dmat4 rotationZ = glm::rotate(dmat4(1.0), euler_rotation.z * DEG_RAD, dvec3(0.0, 0.0, 1.0));
			rotation_matrix = rotationX * rotationY * rotationZ;
			break;
		}
	}
	return translation_matrix * rotation_matrix * scale_matrix;
}