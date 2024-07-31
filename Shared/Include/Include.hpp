#pragma once

#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <functional>
#include <windows.h>
#include <stdexcept>
#include <iostream>
#include <optional>
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
#include <math.h>
#include <regex>
#include <array>
#include <tuple>
#include <any>
#include <map>
#include <set>

#ifdef COMPILE_RENDERER
	#include <glad/glad.h>
#endif

#ifdef COMPILE_EDITOR
	#include "QtCore"
#endif

#include <glm/glm.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

typedef double     dvec1; // 64-bits | min: 2.2250738585072014e-308 | max: 1.7976931348623158e+308
typedef glm::dvec2 dvec2; // 128-bits
typedef glm::dvec3 dvec3; // 196-bits
typedef glm::dvec4 dvec4; // 256-bits
typedef glm::dquat dquat; // 256-bits
typedef glm::dmat2 dmat2; // 256-bits
typedef glm::dmat3 dmat3; // 576-bits
typedef glm::dmat4 dmat4; // 1024-bits

typedef float     vec1; // 32-bits | min: 1.175494351e-38f | max: 3.402823466e+38f
typedef glm::vec2 vec2; // 64-bits
typedef glm::vec3 vec3; // 96-bits
typedef glm::vec4 vec4; // 128-bits
typedef glm::quat quat; // 128-bits
typedef glm::mat2 mat2; // 128-bits
typedef glm::mat3 mat3; // 288-bits
typedef glm::mat4 mat4; // 512-bits

typedef uint32_t uint;   //max: 4'294'967'295
typedef uint8_t  uint8;  //max: 255
typedef uint16_t uint16; //max: 65'535
typedef uint32_t uint32; //max: 4'294'967'295
typedef uint64_t uint64; //max: 18'446'744'073'709'551'615

typedef int8_t  int8;  // min: -128 | max: 127
typedef int16_t int16; // min: -32'768 | max: 32'767
typedef int32_t int32; // min: -2'147'483'648 | max: 2'147'483'647
typedef int64_t int64; // min: -9'223'372'036'854'775'808 | max: 9'223'372'036'854'775'807

typedef uint32     uvec1; // 32-bits
typedef glm::uvec2 uvec2; // 64-bits
typedef glm::uvec3 uvec3; // 96-bits
typedef glm::uvec4 uvec4; // 128-bits

typedef int32      ivec1; // 32-bits
typedef glm::ivec2 ivec2; // 64-bits
typedef glm::ivec3 ivec3; // 96-bits
typedef glm::ivec4 ivec4; // 128-bits

typedef int64             lvec1; // 64-bits
typedef glm::tvec2<int64> lvec2; // 128-bits
typedef glm::tvec3<int64> lvec3; // 192-bits
typedef glm::tvec4<int64> lvec4; // 256-bits

typedef uint64             ulvec1; // 64-bits
typedef glm::tvec2<uint64> ulvec2; // 128-bits
typedef glm::tvec3<uint64> ulvec3; // 192-bits
typedef glm::tvec4<uint64> ulvec4; // 256-bits

inline uvec1 i_to_u(const ivec1& val) { return static_cast<uvec1>(val); }
inline uvec2 i_to_u(const ivec2& val) { return static_cast<uvec2>(val); }
inline uvec3 i_to_u(const ivec3& val) { return static_cast<uvec3>(val); }
inline uvec4 i_to_u(const ivec4& val) { return static_cast<uvec4>(val); }
inline dvec1 i_to_d(const ivec1& val) { return static_cast<dvec1>(val); }
inline dvec2 i_to_d(const ivec2& val) { return static_cast<dvec2>(val); }
inline dvec3 i_to_d(const ivec3& val) { return static_cast<dvec3>(val); }
inline dvec4 i_to_d(const ivec4& val) { return static_cast<dvec4>(val); }

inline ivec1 u_to_i(const uvec1& val) { return static_cast<ivec1>(val); }
inline ivec2 u_to_i(const uvec2& val) { return static_cast<ivec2>(val); }
inline ivec3 u_to_i(const uvec3& val) { return static_cast<ivec3>(val); }
inline ivec4 u_to_i(const uvec4& val) { return static_cast<ivec4>(val); }
inline dvec1 u_to_d(const uvec1& val) { return static_cast<dvec1>(val); }
inline dvec2 u_to_d(const uvec2& val) { return static_cast<dvec2>(val); }
inline dvec3 u_to_d(const uvec3& val) { return static_cast<dvec3>(val); }
inline dvec4 u_to_d(const uvec4& val) { return static_cast<dvec4>(val); }
inline vec1  u_to_f(const uvec1& val) { return static_cast<vec1> (val); }
inline vec2  u_to_f(const uvec2& val) { return static_cast<vec2> (val); }
inline vec3  u_to_f(const uvec3& val) { return static_cast<vec3> (val); }
inline vec4  u_to_f(const uvec4& val) { return static_cast<vec4> (val); }

inline dvec1 ul_to_d(const ulvec1& val) { return static_cast<dvec1>(val); }
inline dvec2 ul_to_d(const ulvec2& val) { return static_cast<dvec2>(val); }
inline dvec3 ul_to_d(const ulvec3& val) { return static_cast<dvec3>(val); }
inline dvec4 ul_to_d(const ulvec4& val) { return static_cast<dvec4>(val); }
inline vec1  ul_to_f(const ulvec1& val) { return static_cast<vec1> (val); }
inline vec2  ul_to_f(const ulvec2& val) { return static_cast<vec2> (val); }
inline vec3  ul_to_f(const ulvec3& val) { return static_cast<vec3> (val); }
inline vec4  ul_to_f(const ulvec4& val) { return static_cast<vec4> (val); }
inline uvec1 ul_to_u(const ulvec1& val) { return static_cast<uvec1>(val); }
inline uvec2 ul_to_u(const ulvec2& val) { return static_cast<uvec2>(val); }
inline uvec3 ul_to_u(const ulvec3& val) { return static_cast<uvec3>(val); }
inline uvec4 ul_to_u(const ulvec4& val) { return static_cast<uvec4>(val); }
inline ivec1 ul_to_i(const ulvec1& val) { return static_cast<ivec1>(val); }
inline ivec2 ul_to_i(const ulvec2& val) { return static_cast<ivec2>(val); }
inline ivec3 ul_to_i(const ulvec3& val) { return static_cast<ivec3>(val); }
inline ivec4 ul_to_i(const ulvec4& val) { return static_cast<ivec4>(val); }

inline uvec1 d_to_u(const dvec1& val) { return static_cast<uvec1>(val); }
inline uvec2 d_to_u(const dvec2& val) { return static_cast<uvec2>(val); }
inline uvec3 d_to_u(const dvec3& val) { return static_cast<uvec3>(val); }
inline uvec4 d_to_u(const dvec4& val) { return static_cast<uvec4>(val); }
inline ivec1 d_to_i(const dvec1& val) { return static_cast<ivec1>(val); }
inline ivec2 d_to_i(const dvec2& val) { return static_cast<ivec2>(val); }
inline ivec3 d_to_i(const dvec3& val) { return static_cast<ivec3>(val); }
inline ivec4 d_to_i(const dvec4& val) { return static_cast<ivec4>(val); }

inline dvec1  f_to_d (const vec1& val) { return static_cast<dvec1> (val); }
inline dvec2  f_to_d (const vec2& val) { return static_cast<dvec2> (val); }
inline dvec3  f_to_d (const vec3& val) { return static_cast<dvec3> (val); }
inline dvec4  f_to_d (const vec4& val) { return static_cast<dvec4> (val); }
inline dmat2  f_to_d (const mat2& val) { return static_cast<dmat2> (val); }
inline dmat3  f_to_d (const mat3& val) { return static_cast<dmat3> (val); }
inline dmat4  f_to_d (const mat4& val) { return static_cast<dmat4> (val); }
inline uvec1  f_to_u (const vec1& val) { return static_cast<uvec1> (val); }
inline uvec2  f_to_u (const vec2& val) { return static_cast<uvec2> (val); }
inline uvec3  f_to_u (const vec3& val) { return static_cast<uvec3> (val); }
inline uvec4  f_to_u (const vec4& val) { return static_cast<uvec4> (val); }
inline ulvec1 f_to_ul(const vec1& val) { return static_cast<ulvec1>(val); }
inline ulvec2 f_to_ul(const vec2& val) { return static_cast<ulvec2>(val); }
inline ulvec3 f_to_ul(const vec3& val) { return static_cast<ulvec3>(val); }
inline ulvec4 f_to_ul(const vec4& val) { return static_cast<ulvec4>(val); }

inline vec1 d_to_f(const dvec1& val) { return static_cast<vec1>(val); }
inline vec2 d_to_f(const dvec2& val) { return static_cast<vec2>(val); }
inline vec3 d_to_f(const dvec3& val) { return static_cast<vec3>(val); }
inline vec4 d_to_f(const dvec4& val) { return static_cast<vec4>(val); }
inline mat2 d_to_f(const dmat2& val) { return static_cast<mat2>(val); }
inline mat3 d_to_f(const dmat3& val) { return static_cast<mat3>(val); }
inline mat4 d_to_f(const dmat4& val) { return static_cast<mat4>(val); }

inline vec1 str_to_f(const string& val_1) { return stof(val_1); }
inline vec2 str_to_f(const string& val_1, const string& val_2) { return vec2(stof(val_1), stof(val_2)); }
inline vec3 str_to_f(const string& val_1, const string& val_2, const string& val_3) { return vec3(stof(val_1), stof(val_2), stof(val_3)); }
inline vec4 str_to_f(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return vec4(stof(val_1), stof(val_2), stof(val_3), stof(val_4)); }

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

#define MIN_VEC1  numeric_limits<vec1 >::min()
#define MAX_VEC1  numeric_limits<vec1 >::max()
#define MIN_DVEC1 numeric_limits<dvec1>::min()
#define MAX_DVEC1 numeric_limits<dvec1>::max()

#define MIN_INT8  numeric_limits<int8 >::min()
#define MAX_INT8  numeric_limits<int8 >::max()
#define MIN_INT16 numeric_limits<int16>::min()
#define MAX_INT16 numeric_limits<int16>::max()
#define MIN_INT32 numeric_limits<int32>::min()
#define MAX_INT32 numeric_limits<int32>::max()
#define MIN_INT64 numeric_limits<int64>::min()
#define MAX_INT64 numeric_limits<int64>::max()

#define MAX_UINT8  numeric_limits<uint8 >::max()
#define MAX_UINT16 numeric_limits<uint16>::max()
#define MAX_UINT32 numeric_limits<uint32>::max()
#define MAX_UINT64 numeric_limits<uint64>::max()

#define PI          dvec1(3.141592653589793)
#define TWO_PI      dvec1(6.283185307179586)
#define INVERTED_PI dvec1(0.318309886183791)
#define DEG_RAD     dvec1(0.017453292519943)
#define EULER       dvec1(2.718281828459045)
#define PHI         dvec1(1.618033988749895)
#define FPS_60      dvec1(0.016666666666667)
#define MAX_DIST    dvec1(10000.0)
#define EPSILON     dvec1(0.00001)