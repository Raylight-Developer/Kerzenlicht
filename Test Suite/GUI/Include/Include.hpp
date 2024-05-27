#pragma once

#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <functional>
#include <windows.h>
#include <stdexcept>
#include <iostream>
#include <optional>
#include <typeinfo>
#include <direct.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <variant>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <cerrno>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <future>
#include <string>
#include <memory>
#include <math.h>
#include <regex>
#include <array>
#include <tuple>
#include <any>
#include <map>
#include <set>

#include <qopenglfunctions_4_5_core.h>
#include <QtOpenGLWidgets>
#include <QtWidgets>
#include <QtCore>
#include <QtGui>

#include <glm/glm.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

#define SCRIPT __declspec(dllexport)
//TODO change to typedef

using dvec1 = double;     // 64-bits | min: 2.2250738585072014e-308 | max: 1.7976931348623158e+308
using dvec2 = glm::dvec2; // 128-bits
using dvec3 = glm::dvec3; // 196-bits
using dvec4 = glm::dvec4; // 256-bits
using dquat = glm::dquat; // 256-bits
using dmat2 = glm::dmat2; // 256-bits
using dmat3 = glm::dmat3; // 576-bits
using dmat4 = glm::dmat4; // 1024-bits

using vec1 = float;     // 32-bits | min: 1.175494351e-38f | max: 3.402823466e+38f
using vec2 = glm::vec2; // 64-bits
using vec3 = glm::vec3; // 96-bits
using vec4 = glm::vec4; // 128-bits
using quat = glm::quat; // 128-bits
using mat2 = glm::mat2; // 128-bits
using mat3 = glm::mat3; // 288-bits
using mat4 = glm::mat4; // 512-bits

using uint   = uint32_t;
using uint8  = uint8_t;  //max: 255
using uint16 = uint16_t; //max: 65'535
using uint32 = uint32_t; //max: 4'294'967'295
using uint64 = uint64_t; //max: 18'446'744'073'709'551'615

using int8  = int8_t;  // min: -128 | max: 127
using int16 = int16_t; // min: -32'768 | max: 32'767
using int32 = int32_t; // min: -2'147'483'648 | max: 2'147'483'647
using int64 = int64_t; // min: -9'223'372'036'854'775'808 | max: 9'223'372'036'854'775'807

using uvec1 = uint32;     // 32-bits
using uvec2 = glm::uvec2; // 64-bits
using uvec3 = glm::uvec3; // 96-bits
using uvec4 = glm::uvec4; // 128-bits

using ivec1 = int32;      // 32-bits
using ivec2 = glm::ivec2; // 64-bits
using ivec3 = glm::ivec3; // 96-bits
using ivec4 = glm::ivec4; // 128-bits

using lvec1 = int64;             // 64-bits
using lvec2 = glm::tvec2<int64>; // 128-bits
using lvec3 = glm::tvec3<int64>; // 192-bits
using lvec4 = glm::tvec4<int64>; // 256-bits

using ulvec1 = uint64;             // 64-bits
using ulvec2 = glm::tvec2<uint64>; // 128-bits
using ulvec3 = glm::tvec3<uint64>; // 192-bits
using ulvec4 = glm::tvec4<uint64>; // 256-bits

inline uvec1 i_to_u(const ivec1& val) { return static_cast<uvec1>(val); }
inline uvec2 i_to_u(const ivec2& val) { return static_cast<uvec2>(val); }
inline uvec3 i_to_u(const ivec3& val) { return static_cast<uvec3>(val); }
inline uvec4 i_to_u(const ivec4& val) { return static_cast<uvec4>(val); }
inline dvec1 i_to_d(const ivec1& val) { return static_cast<dvec1 >(val); }
inline dvec2 i_to_d(const ivec2& val) { return static_cast<dvec2 >(val); }
inline dvec3 i_to_d(const ivec3& val) { return static_cast<dvec3 >(val); }
inline dvec4 i_to_d(const ivec4& val) { return static_cast<dvec4 >(val); }

inline ivec1 u_to_i(const uvec1& val) { return static_cast<ivec1>(val); }
inline ivec2 u_to_i(const uvec2& val) { return static_cast<ivec2>(val); }
inline ivec3 u_to_i(const uvec3& val) { return static_cast<ivec3>(val); }
inline ivec4 u_to_i(const uvec4& val) { return static_cast<ivec4>(val); }
inline dvec1 u_to_d(const uvec1& val) { return static_cast<dvec1 >(val); }
inline dvec2 u_to_d(const uvec2& val) { return static_cast<dvec2 >(val); }
inline dvec3 u_to_d(const uvec3& val) { return static_cast<dvec3 >(val); }
inline dvec4 u_to_d(const uvec4& val) { return static_cast<dvec4 >(val); }

inline uvec1 d_to_u(const dvec1& val) { return static_cast<uvec1>(val); }
inline uvec2 d_to_u(const dvec2& val) { return static_cast<uvec2>(val); }
inline uvec3 d_to_u(const dvec3& val) { return static_cast<uvec3>(val); }
inline uvec4 d_to_u(const dvec4& val) { return static_cast<uvec4>(val); }
inline ivec1 d_to_i(const dvec1& val) { return static_cast<ivec1>(val); }
inline ivec2 d_to_i(const dvec2& val) { return static_cast<ivec2>(val); }
inline ivec3 d_to_i(const dvec3& val) { return static_cast<ivec3>(val); }
inline ivec4 d_to_i(const dvec4& val) { return static_cast<ivec4>(val); }

inline dvec1 f_to_d(const vec1& val) { return static_cast<dvec1>(val); } // vec1 to dvec1
inline dvec2 f_to_d(const vec2& val) { return static_cast<dvec2>(val); } // vec2 to dvec2
inline dvec3 f_to_d(const vec3& val) { return static_cast<dvec3>(val); } // vec3 to dvec3
inline dvec4 f_to_d(const vec4& val) { return static_cast<dvec4>(val); } // vec4 to dvec4

inline vec1 d_to_f(const dvec1& val) { return static_cast<vec1>(val); } // dvec1 to vec1
inline vec2 d_to_f(const dvec2& val) { return static_cast<vec2>(val); } // dvec2 to vec2
inline vec3 d_to_f(const dvec3& val) { return static_cast<vec3>(val); } // dvec3 to vec3
inline vec4 d_to_f(const dvec4& val) { return static_cast<vec4>(val); } // dvec4 to vec4
inline mat4 d_to_f(const dmat4& val) { return static_cast<mat4>(val); } // dmat4 to mat4

inline dvec1 str_to_d(const string& val_1) { return stod(val_1); }
inline dvec2 str_to_d(const string& val_1, const string& val_2) { return dvec2(stod(val_1), stod(val_2)); }
inline dvec3 str_to_d(const string& val_1, const string& val_2, const string& val_3) { return dvec3(stod(val_1), stod(val_2), stod(val_3)); }
inline dvec4 str_to_d(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return dvec4(stod(val_1), stod(val_2), stod(val_3), stod(val_4)); }

inline uvec1 str_to_u(const string& val_1) { return stoul(val_1); }
inline uvec2 str_to_u(const string& val_1, const string& val_2) { return uvec2(stoul(val_1), stoul(val_2)); }
inline uvec3 str_to_u(const string& val_1, const string& val_2, const string& val_3) { return uvec3(stoul(val_1), stoul(val_2), stoul(val_3)); }
inline uvec4 str_to_u(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return dvec4(stoul(val_1), stoul(val_2), stoul(val_3), stoul(val_4)); }

inline ivec1 str_to_i(const string& val_1) { return stoi(val_1); }
inline ivec2 str_to_i(const string& val_1, const string& val_2) { return ivec2(stoi(val_1), stoi(val_2)); }
inline ivec3 str_to_i(const string& val_1, const string& val_2, const string& val_3) { return ivec3(stoi(val_1), stoi(val_2), stoi(val_3)); }
inline ivec4 str_to_i(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return ivec4(stoi(val_1), stoi(val_2), stoi(val_3), stoi(val_4)); }

inline lvec1 str_to_l(const string& val_1) { return stoll(val_1); }
inline lvec2 str_to_l(const string& val_1, const string& val_2) { return lvec2(stoll(val_1), stoll(val_2)); }
inline lvec3 str_to_l(const string& val_1, const string& val_2, const string& val_3) { return lvec3(stoll(val_1), stoll(val_2), stoll(val_3)); }
inline lvec4 str_to_l(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return lvec4(stoll(val_1), stoll(val_2), stoll(val_3), stoll(val_4)); }

inline ulvec1 str_to_ul(const string& val_1) { return stoull(val_1); }
inline ulvec2 str_to_ul(const string& val_1, const string& val_2) { return ulvec2(stoull(val_1), stoull(val_2)); }
inline ulvec3 str_to_ul(const string& val_1, const string& val_2, const string& val_3) { return ulvec3(stoull(val_1), stoull(val_2), stoull(val_3)); }
inline ulvec4 str_to_ul(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return ulvec4(stoull(val_1), stoull(val_2), stoull(val_3), stoull(val_4)); }

#define PI          dvec1(3.141592653589793)
#define TWO_PI      dvec1(6.283185307179586)
#define INVERTED_PI dvec1(0.318309886183791)
#define DEG_RAD     dvec1(0.017453292519943)
#define EULER       dvec1(2.718281828459045)
#define PHI         dvec1(1.618033988749895)
#define MAX_DIST    dvec1(10000.0)
#define EPSILON     dvec1(0.00001)

#define MIN_VEC1   std::numeric_limits<vec1 >::min()
#define MIN_DVEC1  std::numeric_limits<dvec1>::min()
#define MAX_VEC1   std::numeric_limits<vec1 >::max()
#define MAX_DVEC1  std::numeric_limits<dvec1>::max()

#define MIN_INT8   std::numeric_limits<int8 >::min()
#define MIN_INT16  std::numeric_limits<int16>::min()
#define MIN_INT32  std::numeric_limits<int32>::min()
#define MIN_INT64  std::numeric_limits<int64>::min()
#define MAX_INT8   std::numeric_limits<int8 >::max()
#define MAX_INT16  std::numeric_limits<int16>::max()
#define MAX_INT32  std::numeric_limits<int32>::max()
#define MAX_INT64  std::numeric_limits<int64>::max()

#define MAX_UINT8  std::numeric_limits<uint8 >::max()
#define MAX_UINT16 std::numeric_limits<uint16>::max()
#define MAX_UINT32 std::numeric_limits<uint32>::max()
#define MAX_UINT64 std::numeric_limits<uint64>::max()