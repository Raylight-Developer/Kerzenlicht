#include "Utils/Ops.hpp"

#include "Utils/Lace.hpp"

Tokens f_split(const string& input) {
	Tokens result;
	istringstream iss(input);
	string token;
	while (iss >> token) {
		result.push_back(token);
	}
	return result;
}

Tokens f_split(const string& input, const string& delimiter) {
	Tokens result;
	uint64 start = 0;
	uint64 end = input.find(delimiter);

	while (end != string::npos) {
		result.push_back(input.substr(start, end - start));
		start = end + delimiter.length();
		end = input.find(delimiter, start);
	}

	result.push_back(input.substr(start));
	return result;
}

Tokens f_closingPair(const Token_Array& tokens, const string& open, const string& close) {
	uint openCount = 0;
	bool collecting = false;
	Tokens result;

	for (const auto& tokenList : tokens) {
		Tokens line;
		for (const auto& token : tokenList) {
			if (token == open) {
				openCount++;
				if (openCount == 1) {
					collecting = true;
				}
			} 
			else if (token == close) {
				openCount--;
				if (openCount == 0) {
					collecting = false;
					return result;
				}
			}
			else if (collecting) {
				line.push_back(token);
			}
		}
		result.push_back(f_join(line));
	}
	return {};
}

string f_closingPair(const Tokens& lines, const string& open, const string& close) {
	uint openCount = 0;
	bool collecting = false;
	vector<string> result;

	for (const string& line : lines) {
		if (f_contains(line, open)) {
			openCount++;
			if (openCount == 1) {
				collecting = true;
			}
		} 
		else if (f_contains(line, close)) {
			openCount--;
			if (openCount == 0) {
				collecting = false;
				return f_join(result, "\n");
			}
		}
		else if (collecting) {
			result.push_back(line);
		}
	}
	return f_join(result, "\n");
}

string f_join(const Tokens& tokens, const string& join, const uint64& start, const uint64& end) {
	return accumulate(
		tokens.begin() + start, tokens.end() - end, string(),
		[join](const string& accumulator, const string& current) {
			return accumulator.empty() ? current : accumulator + join + current;
		}
	);
}

string f_join(const Tokens& tokens, const uint64& start, const uint64& end) {
	return accumulate(
		tokens.begin() + start, tokens.end() - end, string(),
		[](const string& accumulator, const string& current) {
			return accumulator.empty() ? current : accumulator + " " + current;
		}
	);
}

string f_addLinesToLine(const string& value, const string& character) {
	istringstream stream(value);
	ostringstream result;
	string line;

	while (getline(stream, line)) {
		result << character << line << "\n";
	}

	return result.str();
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

bool f_contains(const string& input, const std::string& substring) {
	return input.find(substring) != string::npos;
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

string loadFromFile(const string& file_path) {
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
	cout << endl << endl << "ERROR" << endl << file_path;
	throw runtime_error(to_string(errno));
}

Tokens loadFileLines(const string& file_path) {
	ifstream file(file_path, ios::binary);
	if (file.is_open()) {
		Tokens line_data = Tokens();
		string line;
		while (getline(file, line)) {
			Tokens tokens = f_split(line);
			if (!tokens.empty()) {
				line_data.push_back(line);
			}
		}
		return line_data;
	}
	cout << endl << endl << "ERROR" << endl << file_path;
	throw runtime_error(to_string(errno));
}

Token_Array loadFileTokens(const string& file_path) {
	ifstream file(file_path, ios::binary);

	if (file.is_open()) {
		Token_Array token_data = Token_Array();
		string line;
		while (getline(file, line)) {
			Tokens tokens = f_split(line);
			if (!tokens.empty()) {
				token_data.push_back(tokens);
			}
		}
		return token_data;
	}
	throw runtime_error(to_string(errno));
}

void writeToFile(const string& file_path, const string& content) {
	ofstream file(file_path, ios::binary);

	if (!file.is_open()) {
		cout << endl << endl << "ERROR" << endl << file_path;
		throw runtime_error("Unable to open file");
	}

	for (char c : content) {
		if (c == '\r') {
			continue;
		}
		else if (c == '\n') {
			file.put('\n');
		}
		else {
			file.put(c);
		}
	}
	file.close();
}

string processSubShader(const string& file_path) {
	ifstream in(file_path, ios::binary);
	if (in) {
		KL::Lace output;
		string line;

		while (getline(in, line)) {
			if (!line.empty()) {
				size_t includePos = line.find("#include");
				size_t versionPos = line.find("#version");
				size_t extensionPos = line.find("#extension");
				if (includePos == string::npos && versionPos == string::npos && extensionPos == string::npos) {
					line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
					output << line << KL::Lace_NL();
				}
			}
		}
		return output.str();
	}
	cout << endl << endl << "ERROR" << endl << file_path;
	throw runtime_error(to_string(errno));
}

string preprocessShader(const string& file_path) {
	ifstream in(file_path, ios::binary);
	if (in) {
		KL::Lace output;
		string line;

		while (getline(in, line)) {
			if (!line.empty()) {
				size_t includePos = line.find("#include");
				if (includePos != string::npos) {
					size_t quotePos = line.find("\"", includePos);
					if (quotePos != string::npos) {
						size_t endQuotePos = line.find("\"", quotePos + 1);
						if (endQuotePos != string::npos) {
							string includeFilename = line.substr(quotePos + 1, endQuotePos - quotePos - 1);
							output << processSubShader("./Resources/Shaders/Compute/" + includeFilename);
							continue;
						}
					}
				}
				line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
				output << line << KL::Lace_NL();
			}
		}
		return output.str().substr(0, output.str().size() - 1);
	}
	cout << endl << endl << "ERROR" << endl << file_path;
	throw runtime_error(to_string(errno));
}

KL::Transform::Transform(const dvec3& position, const dvec3& rotation, const dvec3& scale, const Rotation_Type& type) :
	rotation_type(type),
	position(position),
	euler_rotation(rotation),
	scale(scale)
{
	quat_rotation = dquat(1.0, 0.0, 0.0, 0.0);
	axis_rotation = dvec3(0.0);
	x_vec = dvec3(1.0, 0.0, 0.0);
	y_vec = dvec3(0.0, 1.0, 0.0);
	z_vec = dvec3(0.0, 0.0, 1.0);
}

KL::Transform::Transform(const dvec3& position, const dvec3& axis, const dvec3& rotation, const dvec3& scale, const Rotation_Type& type) :
	rotation_type(type),
	position(position),
	euler_rotation(rotation),
	scale(scale),
	axis_rotation(axis)
{
	quat_rotation = dquat(1.0, 0.0, 0.0, 0.0);
	x_vec = dvec3(1.0, 0.0, 0.0);
	y_vec = dvec3(0.0, 1.0, 0.0);
	z_vec = dvec3(0.0, 0.0, 1.0);
}

KL::Transform::Transform(const dvec3& position, const dquat& rotation, const dvec3& scale, const Rotation_Type& type) :
	rotation_type(type),
	position(position),
	quat_rotation(rotation),
	scale(scale)
{
	euler_rotation = dvec3(0.0);
	axis_rotation = dvec3(0.0);
	x_vec = dvec3(1.0, 0.0, 0.0);
	y_vec = dvec3(0.0, 1.0, 0.0);
	z_vec = dvec3(0.0, 0.0, 1.0);
}
// TODO account for different rotation modes
KL::Transform KL::Transform::operator+(const Transform& other) const {
	Transform result;
	result.position       = position       + other.position;
	result.euler_rotation = euler_rotation + other.euler_rotation;
	result.axis_rotation  = axis_rotation  + other.axis_rotation;
	//result.quat_rotation  = quat_rotation  + other.quat_rotation;
	result.scale          = scale          + other.scale;
	return result;
}
KL::Transform KL::Transform::operator-(const Transform& other) const {
	Transform result;
	result.position       = position       - other.position;
	result.euler_rotation = euler_rotation - other.euler_rotation;
	result.axis_rotation  = axis_rotation  - other.axis_rotation;
	//result.quat_rotation  = quat_rotation  - other.quat_rotation;
	result.scale          = scale          - other.scale;
	return result;
}
KL::Transform KL::Transform::operator*(const Transform& other) const {
	Transform result;
	result.position       = position       * other.position;
	result.euler_rotation = euler_rotation * other.euler_rotation;
	result.axis_rotation  = axis_rotation  * other.axis_rotation;
	//result.quat_rotation  = quat_rotation  * other.quat_rotation;
	result.scale          = scale          * other.scale;
	return result;
}
KL::Transform KL::Transform::operator/(const Transform& other) const {
	Transform result;
	result.position       = position       / other.position;
	result.euler_rotation = euler_rotation / other.euler_rotation;
	result.axis_rotation  = axis_rotation  / other.axis_rotation;
	//result.quat_rotation  = quat_rotation  / other.quat_rotation;
	result.scale          = scale          / other.scale;
	return result;
}

KL::Transform KL::Transform::operator*(const dvec1& other) const {
	Transform result;
	result.position       = position       * other;
	result.euler_rotation = euler_rotation * other;
	result.axis_rotation  = axis_rotation  * other;
	//result.quat_rotation  = quat_rotation  * other;
	result.scale          = scale          * other;
	return result;
}

void KL::Transform::moveLocal(const dvec3& value) {
	const dmat4 matrix = glm::yawPitchRoll(glm::radians(euler_rotation.y), glm::radians(euler_rotation.x), glm::radians(euler_rotation.z));
	const dvec3 x_vector = matrix[0];
	const dvec3 y_vector = matrix[1];
	const dvec3 z_vector = matrix[2];
	position += value.x * x_vector;
	position += value.y * y_vector;
	position += value.z * z_vector;
}

void KL::Transform::rotate(const dvec3& value) {
	switch (rotation_type) {
		case KL::Rotation_Type::QUATERNION: {
			const dquat pitch = glm::angleAxis(glm::radians(value.x), dvec3(1, 0, 0));
			const dquat yaw   = glm::angleAxis(glm::radians(value.y), dvec3(0, 1, 0));
			const dquat roll  = glm::angleAxis(glm::radians(value.z), dvec3(0, 0, 1));

			quat_rotation = yaw * pitch * roll * quat_rotation;
			quat_rotation = glm::normalize(quat_rotation);
			break;
		}
		case KL::Rotation_Type::XYZ: {
			euler_rotation += value;

			if (euler_rotation.x > 89.0)  euler_rotation.x = 89.0;
			if (euler_rotation.x < -89.0) euler_rotation.x = -89.0;
		}
	}
}

void KL::Transform::orbit(const dvec3& pivot, const dvec2& py_rotation) {
	switch (rotation_type) {
		case KL::Rotation_Type::QUATERNION: {
			rotate(glm::vec3(py_rotation.x, py_rotation.y, 0.0));

			const dvec3 forward   = glm::normalize(glm::inverse(quat_rotation) * dvec3(0, 0, -1));
			const dvec3 direction = glm::normalize(position - pivot);
			const dvec1 z_distance  = glm::length(position - pivot);

			position = pivot - forward * z_distance;
			break;
		}
		case KL::Rotation_Type::XYZ: {
			rotate(dvec3(py_rotation.x, py_rotation.y, 0.0));

			const dmat4 matrix = glm::yawPitchRoll(glm::radians(euler_rotation.y), glm::radians(euler_rotation.x), glm::radians(euler_rotation.z));
			const dvec3 z_vector = -matrix[2];

			const dvec1 z_distance = glm::length(pivot - position);
			const dvec3 camera_position = position - z_vector * z_distance;

			position = pivot - z_vector * z_distance;
			break;
		}
	}
}

void KL::Transform::f_computeVectors() {
	dmat4 rotation_matrix;
	switch (rotation_type) {
		case KL::Rotation_Type::QUATERNION: {
			rotation_matrix = glm::mat4_cast(quat_rotation);
			break;
		}
		case KL::Rotation_Type::XYZ: {
			rotation_matrix = glm::yawPitchRoll(glm::radians(euler_rotation.y), glm::radians(euler_rotation.x), glm::radians(euler_rotation.z));
			break;
		}
	}
	x_vec = rotation_matrix[0];
	y_vec = rotation_matrix[1];
	z_vec = rotation_matrix[2];
}

dmat4 KL::Transform::getMatrix() const {
	const dmat4 translation_matrix = glm::translate(dmat4(1.0), position);
	const dmat4 scale_matrix = glm::scale(dmat4(1.0), scale);
	dmat4 rotation_matrix = dmat4(1.0);

	switch (rotation_type) {
		case KL::Rotation_Type::QUATERNION: {
			rotation_matrix = glm::toMat4(quat_rotation);
			break;
		}
		case KL::Rotation_Type::XYZ: {
			const dmat4 rotationX = glm::rotate(dmat4(1.0), glm::radians(euler_rotation.x), dvec3(1.0, 0.0, 0.0));
			const dmat4 rotationY = glm::rotate(dmat4(1.0), glm::radians(euler_rotation.y), dvec3(0.0, 1.0, 0.0));
			const dmat4 rotationZ = glm::rotate(dmat4(1.0), glm::radians(euler_rotation.z), dvec3(0.0, 0.0, 1.0));
			rotation_matrix =  rotationZ * rotationY * rotationX;
			break;
		}
	}
	return translation_matrix * rotation_matrix * scale_matrix;
}

string KL::Transform::serialize() const {
	Lace value;
	value << position << " | " << euler_rotation << " | " << scale;
	return value.str();
}

KL::Observable::Observable() {}

void KL::Observable::call() {
	for (const auto& [key, func] : callbacks) {
		func();
	}
}

void KL::Observable::addCallback(void* key, function<void()> func) {
	callbacks[key] = std::move(func);
}

void KL::Observable::removeCallback(void* key) {
	callbacks.erase(key);
}

void KL::Observable::clearCallbacks() {
	callbacks.clear();
}