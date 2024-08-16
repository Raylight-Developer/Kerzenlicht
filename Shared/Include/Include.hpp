#pragma once

#include <unordered_map>
#include <unordered_set>
#include <type_traits>
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
#include <cstddef>
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

#if defined(COMPILE_RENDERER)
	#include <glad/glad.h>
#endif

#if defined(COMPILE_EDITOR) || defined(COMPILE_GUI_SCRIPTING)
	#include "QtCore"
#endif

#include <glm/glm.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#undef min
#undef max

using namespace std;

typedef std::byte  Byte; // 8-bits
typedef vector<string> Tokens;
typedef vector<Tokens> Token_Array;

// FLOATING

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

// INT

typedef int8_t              int8; // min: -128 | max: 127
typedef int16_t            int16; // min: -32'768 | max: 32'767
typedef int32_t            int32; // min: -2'147'483'648 | max: 2'147'483'647
typedef int64_t            int64; // min: -9'223'372'036'854'775'808 | max: 9'223'372'036'854'775'807

typedef int8              isvec1; // 8-bits
typedef glm::tvec2<int8>  isvec2; // 16-bits
typedef glm::tvec3<int8>  isvec3; // 24-bits
typedef glm::tvec4<int8>  isvec4; // 32-bits

typedef int16             ihvec1; // 16-bits
typedef glm::tvec2<int16> ihvec2; // 32-bits
typedef glm::tvec3<int16> ihvec3; // 48-bits
typedef glm::tvec4<int16> ihvec4; // 64-bits

typedef int32              ivec1; // 32-bits
typedef glm::ivec2         ivec2; // 64-bits
typedef glm::ivec3         ivec3; // 96-bits
typedef glm::ivec4         ivec4; // 128-bits

typedef int64             ilvec1; // 64-bits
typedef glm::tvec2<int64> ilvec2; // 128-bits
typedef glm::tvec3<int64> ilvec3; // 192-bits
typedef glm::tvec4<int64> ilvec4; // 256-bits

// UINT

typedef uint32_t             uint; //max: 4'294'967'295
typedef uint8_t             uint8; //max: 255
typedef uint16_t           uint16; //max: 65'535
typedef uint32_t           uint32; //max: 4'294'967'295
typedef uint64_t           uint64; //max: 18'446'744'073'709'551'615

typedef uint8              usvec1; // 8-bits
typedef glm::tvec2<uint8>  usvec2; // 16-bits
typedef glm::tvec3<uint8>  usvec3; // 24-bits
typedef glm::tvec4<uint8>  usvec4; // 32-bits

typedef uint16             uhvec1; // 16-bits
typedef glm::tvec2<uint16> uhvec2; // 32-bits
typedef glm::tvec3<uint16> uhvec3; // 48-bits
typedef glm::tvec4<uint16> uhvec4; // 64-bits

typedef uint32              uvec1; // 32-bits
typedef glm::uvec2          uvec2; // 64-bits
typedef glm::uvec3          uvec3; // 96-bits
typedef glm::uvec4          uvec4; // 128-bits

typedef uint64             ulvec1; // 64-bits
typedef glm::tvec2<uint64> ulvec2; // 128-bits
typedef glm::tvec3<uint64> ulvec3; // 192-bits
typedef glm::tvec4<uint64> ulvec4; // 256-bits

// FLOATING

inline dvec1  f_to_d (const vec1& val) { return static_cast<dvec1>  (val); }
inline dvec2  f_to_d (const vec2& val) { return static_cast<dvec2>  (val); }
inline dvec3  f_to_d (const vec3& val) { return static_cast<dvec3>  (val); }
inline dvec4  f_to_d (const vec4& val) { return static_cast<dvec4>  (val); }
inline dquat  f_to_d (const quat& val) { return static_cast<dquat>  (val); }
inline dmat2  f_to_d (const mat2& val) { return static_cast<dmat2>  (val); }
inline dmat3  f_to_d (const mat3& val) { return static_cast<dmat3>  (val); }
inline dmat4  f_to_d (const mat4& val) { return static_cast<dmat4>  (val); }

inline isvec1 f_to_is(const vec1& val) { return static_cast<isvec1> (val); }
inline isvec2 f_to_is(const vec2& val) { return static_cast<isvec2> (val); }
inline isvec3 f_to_is(const vec3& val) { return static_cast<isvec3> (val); }
inline isvec4 f_to_is(const vec4& val) { return static_cast<isvec4> (val); }
inline ihvec1 f_to_ih(const vec1& val) { return static_cast<ihvec1> (val); }
inline ihvec2 f_to_ih(const vec2& val) { return static_cast<ihvec2> (val); }
inline ihvec3 f_to_ih(const vec3& val) { return static_cast<ihvec3> (val); }
inline ihvec4 f_to_ih(const vec4& val) { return static_cast<ihvec4> (val); }
inline ivec1  f_to_i (const vec1& val) { return static_cast<ivec1>  (val); }
inline ivec2  f_to_i (const vec2& val) { return static_cast<ivec2>  (val); }
inline ivec3  f_to_i (const vec3& val) { return static_cast<ivec3>  (val); }
inline ivec4  f_to_i (const vec4& val) { return static_cast<ivec4>  (val); }
inline ilvec1 f_to_il(const vec1& val) { return static_cast<ilvec1> (val); }
inline ilvec2 f_to_il(const vec2& val) { return static_cast<ilvec2> (val); }
inline ilvec3 f_to_il(const vec3& val) { return static_cast<ilvec3> (val); }
inline ilvec4 f_to_il(const vec4& val) { return static_cast<ilvec4> (val); }

inline usvec1 f_to_us(const vec1& val) { return static_cast<usvec1> (val); }
inline usvec2 f_to_us(const vec2& val) { return static_cast<usvec2> (val); }
inline usvec3 f_to_us(const vec3& val) { return static_cast<usvec3> (val); }
inline usvec4 f_to_us(const vec4& val) { return static_cast<usvec4> (val); }
inline uhvec1 f_to_uh(const vec1& val) { return static_cast<uhvec1> (val); }
inline uhvec2 f_to_uh(const vec2& val) { return static_cast<uhvec2> (val); }
inline uhvec3 f_to_uh(const vec3& val) { return static_cast<uhvec3> (val); }
inline uhvec4 f_to_uh(const vec4& val) { return static_cast<uhvec4> (val); }
inline uvec1  f_to_u (const vec1& val) { return static_cast<uvec1>  (val); }
inline uvec2  f_to_u (const vec2& val) { return static_cast<uvec2>  (val); }
inline uvec3  f_to_u (const vec3& val) { return static_cast<uvec3>  (val); }
inline uvec4  f_to_u (const vec4& val) { return static_cast<uvec4>  (val); }
inline ulvec1 f_to_ul(const vec1& val) { return static_cast<ulvec1> (val); }
inline ulvec2 f_to_ul(const vec2& val) { return static_cast<ulvec2> (val); }
inline ulvec3 f_to_ul(const vec3& val) { return static_cast<ulvec3> (val); }
inline ulvec4 f_to_ul(const vec4& val) { return static_cast<ulvec4> (val); }

inline vec1   d_to_f (const dvec1& val) { return static_cast<vec1>  (val); }
inline vec2   d_to_f (const dvec2& val) { return static_cast<vec2>  (val); }
inline vec3   d_to_f (const dvec3& val) { return static_cast<vec3>  (val); }
inline vec4   d_to_f (const dvec4& val) { return static_cast<vec4>  (val); }
inline quat   d_to_f (const dquat& val) { return static_cast<quat>  (val); }
inline mat2   d_to_f (const dmat2& val) { return static_cast<mat2>  (val); }
inline mat3   d_to_f (const dmat3& val) { return static_cast<mat3>  (val); }
inline mat4   d_to_f (const dmat4& val) { return static_cast<mat4>  (val); }

inline isvec1 d_to_is(const dvec1& val) { return static_cast<isvec1>(val); }
inline isvec2 d_to_is(const dvec2& val) { return static_cast<isvec2>(val); }
inline isvec3 d_to_is(const dvec3& val) { return static_cast<isvec3>(val); }
inline isvec4 d_to_is(const dvec4& val) { return static_cast<isvec4>(val); }
inline ihvec1 d_to_ih(const dvec1& val) { return static_cast<ihvec1>(val); }
inline ihvec2 d_to_ih(const dvec2& val) { return static_cast<ihvec2>(val); }
inline ihvec3 d_to_ih(const dvec3& val) { return static_cast<ihvec3>(val); }
inline ihvec4 d_to_ih(const dvec4& val) { return static_cast<ihvec4>(val); }
inline ivec1  d_to_i (const dvec1& val) { return static_cast<ivec1> (val); }
inline ivec2  d_to_i (const dvec2& val) { return static_cast<ivec2> (val); }
inline ivec3  d_to_i (const dvec3& val) { return static_cast<ivec3> (val); }
inline ivec4  d_to_i (const dvec4& val) { return static_cast<ivec4> (val); }
inline ilvec1 d_to_il(const dvec1& val) { return static_cast<ilvec1>(val); }
inline ilvec2 d_to_il(const dvec2& val) { return static_cast<ilvec2>(val); }
inline ilvec3 d_to_il(const dvec3& val) { return static_cast<ilvec3>(val); }
inline ilvec4 d_to_il(const dvec4& val) { return static_cast<ilvec4>(val); }

inline usvec1 d_to_us(const dvec1& val) { return static_cast<usvec1>(val); }
inline usvec2 d_to_us(const dvec2& val) { return static_cast<usvec2>(val); }
inline usvec3 d_to_us(const dvec3& val) { return static_cast<usvec3>(val); }
inline usvec4 d_to_us(const dvec4& val) { return static_cast<usvec4>(val); }
inline uhvec1 d_to_uh(const dvec1& val) { return static_cast<uhvec1>(val); }
inline uhvec2 d_to_uh(const dvec2& val) { return static_cast<uhvec2>(val); }
inline uhvec3 d_to_uh(const dvec3& val) { return static_cast<uhvec3>(val); }
inline uhvec4 d_to_uh(const dvec4& val) { return static_cast<uhvec4>(val); }
inline uvec1  d_to_u (const dvec1& val) { return static_cast<uvec1> (val); }
inline uvec2  d_to_u (const dvec2& val) { return static_cast<uvec2> (val); }
inline uvec3  d_to_u (const dvec3& val) { return static_cast<uvec3> (val); }
inline uvec4  d_to_u (const dvec4& val) { return static_cast<uvec4> (val); }
inline ulvec1 d_to_ul(const dvec1& val) { return static_cast<ulvec1>(val); }
inline ulvec2 d_to_ul(const dvec2& val) { return static_cast<ulvec2>(val); }
inline ulvec3 d_to_ul(const dvec3& val) { return static_cast<ulvec3>(val); }
inline ulvec4 d_to_ul(const dvec4& val) { return static_cast<ulvec4>(val); }

// INT

inline ivec1  il_to_i (const ilvec1& val) { return static_cast<ivec1> (val); }
inline ihvec1 il_to_ih(const ilvec1& val) { return static_cast<ihvec1>(val); }
inline isvec1 il_to_is(const ilvec1& val) { return static_cast<isvec1>(val); }
inline ivec2  il_to_i (const ilvec2& val) { return static_cast<ivec2> (val); }
inline ihvec2 il_to_ih(const ilvec2& val) { return static_cast<ihvec2>(val); }
inline isvec2 il_to_is(const ilvec2& val) { return static_cast<isvec2>(val); }
inline ivec3  il_to_i (const ilvec3& val) { return static_cast<ivec3> (val); }
inline ihvec3 il_to_ih(const ilvec3& val) { return static_cast<ihvec3>(val); }
inline isvec3 il_to_is(const ilvec3& val) { return static_cast<isvec3>(val); }
inline ivec4  il_to_i (const ilvec4& val) { return static_cast<ivec4> (val); }
inline ihvec4 il_to_ih(const ilvec4& val) { return static_cast<ihvec4>(val); }
inline isvec4 il_to_is(const ilvec4& val) { return static_cast<isvec4>(val); }

inline ilvec1 i_to_il (const ivec1& val)  { return static_cast<ilvec1>(val); }
inline ihvec1 i_to_ih (const ivec1& val)  { return static_cast<ihvec1>(val); }
inline isvec1 i_to_is (const ivec1& val)  { return static_cast<isvec1>(val); }
inline ilvec2 i_to_il (const ivec2& val)  { return static_cast<ilvec2>(val); }
inline ihvec2 i_to_ih (const ivec2& val)  { return static_cast<ihvec2>(val); }
inline isvec2 i_to_is (const ivec2& val)  { return static_cast<isvec2>(val); }
inline ilvec3 i_to_il (const ivec3& val)  { return static_cast<ilvec3>(val); }
inline ihvec3 i_to_ih (const ivec3& val)  { return static_cast<ihvec3>(val); }
inline isvec3 i_to_is (const ivec3& val)  { return static_cast<isvec3>(val); }
inline ilvec4 i_to_il (const ivec4& val)  { return static_cast<ilvec4>(val); }
inline ihvec4 i_to_ih (const ivec4& val)  { return static_cast<ihvec4>(val); }
inline isvec4 i_to_is (const ivec4& val)  { return static_cast<isvec4>(val); }

inline ilvec1 ih_to_il(const ihvec1& val) { return static_cast<ilvec1>(val); }
inline ivec1  ih_to_i (const ihvec1& val) { return static_cast<ivec1> (val); }
inline isvec1 ih_to_is(const ihvec1& val) { return static_cast<isvec1>(val); }
inline ilvec2 ih_to_il(const ihvec2& val) { return static_cast<ilvec2>(val); }
inline ivec2  ih_to_i (const ihvec2& val) { return static_cast<ivec2> (val); }
inline isvec2 ih_to_is(const ihvec2& val) { return static_cast<isvec2>(val); }
inline ilvec3 ih_to_il(const ihvec3& val) { return static_cast<ilvec3>(val); }
inline ivec3  ih_to_i (const ihvec3& val) { return static_cast<ivec3> (val); }
inline isvec3 ih_to_is(const ihvec3& val) { return static_cast<isvec3>(val); }
inline ilvec4 ih_to_il(const ihvec4& val) { return static_cast<ilvec4>(val); }
inline ivec4  ih_to_i (const ihvec4& val) { return static_cast<ivec4> (val); }
inline isvec4 ih_to_is(const ihvec4& val) { return static_cast<isvec4>(val); }

inline ilvec1 is_to_il(const isvec1& val) { return static_cast<ilvec1>(val); }
inline ivec1  is_to_i (const isvec1& val) { return static_cast<ivec1> (val); }
inline ihvec1 is_to_ih(const isvec1& val) { return static_cast<ihvec1>(val); }
inline ilvec2 is_to_il(const isvec2& val) { return static_cast<ilvec2>(val); }
inline ivec2  is_to_i (const isvec2& val) { return static_cast<ivec2> (val); }
inline ihvec2 is_to_ih(const isvec2& val) { return static_cast<ihvec2>(val); }
inline ilvec3 is_to_il(const isvec3& val) { return static_cast<ilvec3>(val); }
inline ivec3  is_to_i (const isvec3& val) { return static_cast<ivec3> (val); }
inline ihvec3 is_to_ih(const isvec3& val) { return static_cast<ihvec3>(val); }
inline ilvec4 is_to_il(const isvec4& val) { return static_cast<ilvec4>(val); }
inline ivec4  is_to_i (const isvec4& val) { return static_cast<ivec4> (val); }
inline ihvec4 is_to_ih(const isvec4& val) { return static_cast<ihvec4>(val); }

inline ulvec1 il_to_ul(const ilvec1& val) { return static_cast<ulvec1>(val); }
inline uvec1  il_to_u (const ilvec1& val) { return static_cast<uvec1> (val); }
inline uhvec1 il_to_uh(const ilvec1& val) { return static_cast<uhvec1>(val); }
inline usvec1 il_to_us(const ilvec1& val) { return static_cast<usvec1>(val); }
inline uvec2  il_to_u (const ilvec2& val) { return static_cast<uvec2> (val); }
inline uhvec2 il_to_uh(const ilvec2& val) { return static_cast<uhvec2>(val); }
inline usvec2 il_to_us(const ilvec2& val) { return static_cast<usvec2>(val); }
inline ulvec2 il_to_ul(const ilvec2& val) { return static_cast<ulvec2>(val); }
inline uvec3  il_to_u (const ilvec3& val) { return static_cast<uvec3> (val); }
inline uhvec3 il_to_uh(const ilvec3& val) { return static_cast<uhvec3>(val); }
inline usvec3 il_to_us(const ilvec3& val) { return static_cast<usvec3>(val); }
inline ulvec3 il_to_ul(const ilvec3& val) { return static_cast<ulvec3>(val); }
inline uvec4  il_to_u (const ilvec4& val) { return static_cast<uvec4> (val); }
inline uhvec4 il_to_uh(const ilvec4& val) { return static_cast<uhvec4>(val); }
inline usvec4 il_to_us(const ilvec4& val) { return static_cast<usvec4>(val); }
inline ulvec4 il_to_ul(const ilvec4& val) { return static_cast<ulvec4>(val); }

inline ulvec1 i_to_ul (const ivec1& val)  { return static_cast<ulvec1>(val); }
inline uvec1  i_to_u  (const ivec1& val)  { return static_cast<uvec1> (val); }
inline uhvec1 i_to_uh (const ivec1& val)  { return static_cast<uhvec1>(val); }
inline usvec1 i_to_us (const ivec1& val)  { return static_cast<usvec1>(val); }
inline ulvec2 i_to_ul (const ivec2& val)  { return static_cast<ulvec2>(val); }
inline uvec2  i_to_u  (const ivec2& val)  { return static_cast<uvec2> (val); }
inline uhvec2 i_to_uh (const ivec2& val)  { return static_cast<uhvec2>(val); }
inline usvec2 i_to_us (const ivec2& val)  { return static_cast<usvec2>(val); }
inline ulvec3 i_to_ul (const ivec3& val)  { return static_cast<ulvec3>(val); }
inline uvec3  i_to_u  (const ivec3& val)  { return static_cast<uvec3> (val); }
inline uhvec3 i_to_uh (const ivec3& val)  { return static_cast<uhvec3>(val); }
inline usvec3 i_to_us (const ivec3& val)  { return static_cast<usvec3>(val); }
inline ulvec4 i_to_ul (const ivec4& val)  { return static_cast<ulvec4>(val); }
inline uvec4  i_to_u  (const ivec4& val)  { return static_cast<uvec4> (val); }
inline uhvec4 i_to_uh (const ivec4& val)  { return static_cast<uhvec4>(val); }
inline usvec4 i_to_us (const ivec4& val)  { return static_cast<usvec4>(val); }

inline ulvec1 ih_to_ul(const ihvec1& val) { return static_cast<ulvec1>(val); }
inline uvec1  ih_to_u (const ihvec1& val) { return static_cast<uvec1> (val); }
inline uhvec1 ih_to_uh(const ihvec1& val) { return static_cast<uhvec1>(val); }
inline usvec1 ih_to_us(const ihvec1& val) { return static_cast<usvec1>(val); }
inline ulvec2 ih_to_ul(const ihvec2& val) { return static_cast<ulvec2>(val); }
inline uvec2  ih_to_u (const ihvec2& val) { return static_cast<uvec2> (val); }
inline uhvec2 ih_to_uh(const ihvec2& val) { return static_cast<uhvec2>(val); }
inline usvec2 ih_to_us(const ihvec2& val) { return static_cast<usvec2>(val); }
inline ulvec3 ih_to_ul(const ihvec3& val) { return static_cast<ulvec3>(val); }
inline uvec3  ih_to_u (const ihvec3& val) { return static_cast<uvec3> (val); }
inline uhvec3 ih_to_uh(const ihvec3& val) { return static_cast<uhvec3>(val); }
inline usvec3 ih_to_us(const ihvec3& val) { return static_cast<usvec3>(val); }
inline ulvec4 ih_to_ul(const ihvec4& val) { return static_cast<ulvec4>(val); }
inline uvec4  ih_to_u (const ihvec4& val) { return static_cast<uvec4> (val); }
inline uhvec4 ih_to_uh(const ihvec4& val) { return static_cast<uhvec4>(val); }
inline usvec4 ih_to_us(const ihvec4& val) { return static_cast<usvec4>(val); }

inline ulvec1 is_to_ul(const isvec1& val) { return static_cast<ulvec1>(val); }
inline uvec1  is_to_u (const isvec1& val) { return static_cast<uvec1> (val); }
inline uhvec1 is_to_uh(const isvec1& val) { return static_cast<uhvec1>(val); }
inline usvec1 is_to_us(const isvec1& val) { return static_cast<usvec1>(val); }
inline ulvec2 is_to_ul(const isvec2& val) { return static_cast<ulvec2>(val); }
inline uvec2  is_to_u (const isvec2& val) { return static_cast<uvec2> (val); }
inline uhvec2 is_to_uh(const isvec2& val) { return static_cast<uhvec2>(val); }
inline usvec2 is_to_us(const isvec2& val) { return static_cast<usvec2>(val); }
inline ulvec3 is_to_ul(const isvec3& val) { return static_cast<ulvec3>(val); }
inline uvec3  is_to_u (const isvec3& val) { return static_cast<uvec3> (val); }
inline uhvec3 is_to_uh(const isvec3& val) { return static_cast<uhvec3>(val); }
inline usvec3 is_to_us(const isvec3& val) { return static_cast<usvec3>(val); }
inline ulvec4 is_to_ul(const isvec4& val) { return static_cast<ulvec4>(val); }
inline uvec4  is_to_u (const isvec4& val) { return static_cast<uvec4> (val); }
inline uhvec4 is_to_uh(const isvec4& val) { return static_cast<uhvec4>(val); }
inline usvec4 is_to_us(const isvec4& val) { return static_cast<usvec4>(val); }

inline vec1  il_to_f(const ilvec1& val) { return static_cast<vec1> (val); }
inline vec2  il_to_f(const ilvec2& val) { return static_cast<vec2> (val); }
inline vec3  il_to_f(const ilvec3& val) { return static_cast<vec3> (val); }
inline vec4  il_to_f(const ilvec4& val) { return static_cast<vec4> (val); }

inline dvec1 il_to_d(const ilvec1& val) { return static_cast<dvec1>(val); }
inline dvec2 il_to_d(const ilvec2& val) { return static_cast<dvec2>(val); }
inline dvec3 il_to_d(const ilvec3& val) { return static_cast<dvec3>(val); }
inline dvec4 il_to_d(const ilvec4& val) { return static_cast<dvec4>(val); }

inline vec1  i_to_f (const ivec1& val)  { return static_cast<vec1> (val); }
inline vec2  i_to_f (const ivec2& val)  { return static_cast<vec2> (val); }
inline vec3  i_to_f (const ivec3& val)  { return static_cast<vec3> (val); }
inline vec4  i_to_f (const ivec4& val)  { return static_cast<vec4> (val); }

inline dvec1 i_to_d (const ivec1& val)  { return static_cast<dvec1>(val); }
inline dvec2 i_to_d (const ivec2& val)  { return static_cast<dvec2>(val); }
inline dvec3 i_to_d (const ivec3& val)  { return static_cast<dvec3>(val); }
inline dvec4 i_to_d (const ivec4& val)  { return static_cast<dvec4>(val); }

inline vec1  ih_to_f(const ihvec1& val) { return static_cast<vec1> (val); }
inline vec2  ih_to_f(const ihvec2& val) { return static_cast<vec2> (val); }
inline vec3  ih_to_f(const ihvec3& val) { return static_cast<vec3> (val); }
inline vec4  ih_to_f(const ihvec4& val) { return static_cast<vec4> (val); }

inline dvec1 ih_to_d(const ihvec1& val) { return static_cast<dvec1>(val); }
inline dvec2 ih_to_d(const ihvec2& val) { return static_cast<dvec2>(val); }
inline dvec3 ih_to_d(const ihvec3& val) { return static_cast<dvec3>(val); }
inline dvec4 ih_to_d(const ihvec4& val) { return static_cast<dvec4>(val); }

inline vec1  is_to_f(const isvec1& val) { return static_cast<vec1> (val); }
inline vec2  is_to_f(const isvec2& val) { return static_cast<vec2> (val); }
inline vec3  is_to_f(const isvec3& val) { return static_cast<vec3> (val); }
inline vec4  is_to_f(const isvec4& val) { return static_cast<vec4> (val); }

inline dvec1 is_to_d(const isvec1& val) { return static_cast<dvec1>(val); }
inline dvec2 is_to_d(const isvec2& val) { return static_cast<dvec2>(val); }
inline dvec3 is_to_d(const isvec3& val) { return static_cast<dvec3>(val); }
inline dvec4 is_to_d(const isvec4& val) { return static_cast<dvec4>(val); }


// UINT

inline uvec1  ul_to_u (const ulvec1& val) { return static_cast<uvec1> (val); }
inline uhvec1 ul_to_uh(const ulvec1& val) { return static_cast<uhvec1>(val); }
inline usvec1 ul_to_us(const ulvec1& val) { return static_cast<usvec1>(val); }
inline uvec2  ul_to_u (const ulvec2& val) { return static_cast<uvec2> (val); }
inline uhvec2 ul_to_uh(const ulvec2& val) { return static_cast<uhvec2>(val); }
inline usvec2 ul_to_us(const ulvec2& val) { return static_cast<usvec2>(val); }
inline uvec3  ul_to_u (const ulvec3& val) { return static_cast<uvec3> (val); }
inline uhvec3 ul_to_uh(const ulvec3& val) { return static_cast<uhvec3>(val); }
inline usvec3 ul_to_us(const ulvec3& val) { return static_cast<usvec3>(val); }
inline uvec4  ul_to_u (const ulvec4& val) { return static_cast<uvec4> (val); }
inline uhvec4 ul_to_uh(const ulvec4& val) { return static_cast<uhvec4>(val); }
inline usvec4 ul_to_us(const ulvec4& val) { return static_cast<usvec4>(val); }

inline ulvec1 u_to_ul (const uvec1& val)  { return static_cast<ulvec1>(val); }
inline uhvec1 u_to_uh (const uvec1& val)  { return static_cast<uhvec1>(val); }
inline usvec1 u_to_us (const uvec1& val)  { return static_cast<usvec1>(val); }
inline ulvec2 u_to_ul (const uvec2& val)  { return static_cast<ulvec2>(val); }
inline uhvec2 u_to_uh (const uvec2& val)  { return static_cast<uhvec2>(val); }
inline usvec2 u_to_us (const uvec2& val)  { return static_cast<usvec2>(val); }
inline ulvec3 u_to_ul (const uvec3& val)  { return static_cast<ulvec3>(val); }
inline uhvec3 u_to_uh (const uvec3& val)  { return static_cast<uhvec3>(val); }
inline usvec3 u_to_us (const uvec3& val)  { return static_cast<usvec3>(val); }
inline ulvec4 u_to_ul (const uvec4& val)  { return static_cast<ulvec4>(val); }
inline uhvec4 u_to_uh (const uvec4& val)  { return static_cast<uhvec4>(val); }
inline usvec4 u_to_us (const uvec4& val)  { return static_cast<usvec4>(val); }

inline ulvec1 uh_to_ul(const uhvec1& val) { return static_cast<ulvec1>(val); }
inline uvec1  uh_to_u (const uhvec1& val) { return static_cast<uvec1> (val); }
inline usvec1 uh_to_us(const uhvec1& val) { return static_cast<usvec1>(val); }
inline ulvec2 uh_to_ul(const uhvec2& val) { return static_cast<ulvec2>(val); }
inline uvec2  uh_to_u (const uhvec2& val) { return static_cast<uvec2> (val); }
inline usvec2 uh_to_us(const uhvec2& val) { return static_cast<usvec2>(val); }
inline ulvec3 uh_to_ul(const uhvec3& val) { return static_cast<ulvec3>(val); }
inline uvec3  uh_to_u (const uhvec3& val) { return static_cast<uvec3> (val); }
inline usvec3 uh_to_us(const uhvec3& val) { return static_cast<usvec3>(val); }
inline ulvec4 uh_to_ul(const uhvec4& val) { return static_cast<ulvec4>(val); }
inline uvec4  uh_to_u (const uhvec4& val) { return static_cast<uvec4> (val); }
inline usvec4 uh_to_us(const uhvec4& val) { return static_cast<usvec4>(val); }

inline ulvec1 us_to_ul(const usvec1& val) { return static_cast<ulvec1>(val); }
inline uvec1  us_to_u (const usvec1& val) { return static_cast<uvec1> (val); }
inline uhvec1 us_to_uh(const usvec1& val) { return static_cast<uhvec1>(val); }
inline ulvec2 us_to_ul(const usvec2& val) { return static_cast<ulvec2>(val); }
inline uvec2  us_to_u (const usvec2& val) { return static_cast<uvec2> (val); }
inline uhvec2 us_to_uh(const usvec2& val) { return static_cast<uhvec2>(val); }
inline ulvec3 us_to_ul(const usvec3& val) { return static_cast<ulvec3>(val); }
inline uvec3  us_to_u (const usvec3& val) { return static_cast<uvec3> (val); }
inline uhvec3 us_to_uh(const usvec3& val) { return static_cast<uhvec3>(val); }
inline ulvec4 us_to_ul(const usvec4& val) { return static_cast<ulvec4>(val); }
inline uvec4  us_to_u (const usvec4& val) { return static_cast<uvec4> (val); }
inline uhvec4 us_to_uh(const usvec4& val) { return static_cast<uhvec4>(val); }

inline ilvec1 ul_to_il(const ulvec1& val) { return static_cast<ilvec1>(val); }
inline ivec1  ul_to_i (const ulvec1& val) { return static_cast<ivec1> (val); }
inline ihvec1 ul_to_ih(const ulvec1& val) { return static_cast<ihvec1>(val); }
inline isvec1 ul_to_is(const ulvec1& val) { return static_cast<isvec1>(val); }
inline ivec2  ul_to_i (const ulvec2& val) { return static_cast<ivec2> (val); }
inline ihvec2 ul_to_ih(const ulvec2& val) { return static_cast<ihvec2>(val); }
inline isvec2 ul_to_is(const ulvec2& val) { return static_cast<isvec2>(val); }
inline ilvec2 ul_to_il(const ulvec2& val) { return static_cast<ilvec2>(val); }
inline ivec3  ul_to_i (const ulvec3& val) { return static_cast<ivec3> (val); }
inline ihvec3 ul_to_ih(const ulvec3& val) { return static_cast<ihvec3>(val); }
inline isvec3 ul_to_is(const ulvec3& val) { return static_cast<isvec3>(val); }
inline ilvec3 ul_to_il(const ulvec3& val) { return static_cast<ilvec3>(val); }
inline ivec4  ul_to_i (const ulvec4& val) { return static_cast<ivec4> (val); }
inline ihvec4 ul_to_ih(const ulvec4& val) { return static_cast<ihvec4>(val); }
inline isvec4 ul_to_is(const ulvec4& val) { return static_cast<isvec4>(val); }
inline ilvec4 ul_to_il(const ulvec4& val) { return static_cast<ilvec4>(val); }

inline ilvec1 u_to_il (const uvec1& val)  { return static_cast<ilvec1>(val); }
inline ivec1  u_to_i  (const uvec1& val)  { return static_cast<ivec1> (val); }
inline ihvec1 u_to_ih (const uvec1& val)  { return static_cast<ihvec1>(val); }
inline isvec1 u_to_is (const uvec1& val)  { return static_cast<isvec1>(val); }
inline ilvec2 u_to_il (const uvec2& val)  { return static_cast<ilvec2>(val); }
inline ivec2  u_to_i  (const uvec2& val)  { return static_cast<ivec2> (val); }
inline ihvec2 u_to_ih (const uvec2& val)  { return static_cast<ihvec2>(val); }
inline isvec2 u_to_is (const uvec2& val)  { return static_cast<isvec2>(val); }
inline ilvec3 u_to_il (const uvec3& val)  { return static_cast<ilvec3>(val); }
inline ivec3  u_to_i  (const uvec3& val)  { return static_cast<ivec3> (val); }
inline ihvec3 u_to_ih (const uvec3& val)  { return static_cast<ihvec3>(val); }
inline isvec3 u_to_is (const uvec3& val)  { return static_cast<isvec3>(val); }
inline ilvec4 u_to_il (const uvec4& val)  { return static_cast<ilvec4>(val); }
inline ivec4  u_to_i  (const uvec4& val)  { return static_cast<ivec4> (val); }
inline ihvec4 u_to_ih (const uvec4& val)  { return static_cast<ihvec4>(val); }
inline isvec4 u_to_is (const uvec4& val)  { return static_cast<isvec4>(val); }

inline ilvec1 uh_to_il(const uhvec1& val) { return static_cast<ilvec1>(val); }
inline ivec1  uh_to_i (const uhvec1& val) { return static_cast<ivec1> (val); }
inline ihvec1 uh_to_ih(const uhvec1& val) { return static_cast<ihvec1>(val); }
inline isvec1 uh_to_is(const uhvec1& val) { return static_cast<isvec1>(val); }
inline ilvec2 uh_to_il(const uhvec2& val) { return static_cast<ilvec2>(val); }
inline ivec2  uh_to_i (const uhvec2& val) { return static_cast<ivec2> (val); }
inline ihvec2 uh_to_ih(const uhvec2& val) { return static_cast<ihvec2>(val); }
inline isvec2 uh_to_is(const uhvec2& val) { return static_cast<isvec2>(val); }
inline ilvec3 uh_to_il(const uhvec3& val) { return static_cast<ilvec3>(val); }
inline ivec3  uh_to_i (const uhvec3& val) { return static_cast<ivec3> (val); }
inline ihvec3 uh_to_ih(const uhvec3& val) { return static_cast<ihvec3>(val); }
inline isvec3 uh_to_is(const uhvec3& val) { return static_cast<isvec3>(val); }
inline ilvec4 uh_to_il(const uhvec4& val) { return static_cast<ilvec4>(val); }
inline ivec4  uh_to_i (const uhvec4& val) { return static_cast<ivec4> (val); }
inline ihvec4 uh_to_ih(const uhvec4& val) { return static_cast<ihvec4>(val); }
inline isvec4 uh_to_is(const uhvec4& val) { return static_cast<isvec4>(val); }

inline ilvec1 us_to_il(const usvec1& val) { return static_cast<ilvec1>(val); }
inline ivec1  us_to_i (const usvec1& val) { return static_cast<ivec1> (val); }
inline ihvec1 us_to_ih(const usvec1& val) { return static_cast<ihvec1>(val); }
inline isvec1 us_to_is(const usvec1& val) { return static_cast<isvec1>(val); }
inline ilvec2 us_to_il(const usvec2& val) { return static_cast<ilvec2>(val); }
inline ivec2  us_to_i (const usvec2& val) { return static_cast<ivec2> (val); }
inline ihvec2 us_to_ih(const usvec2& val) { return static_cast<ihvec2>(val); }
inline isvec2 us_to_is(const usvec2& val) { return static_cast<isvec2>(val); }
inline ilvec3 us_to_il(const usvec3& val) { return static_cast<ilvec3>(val); }
inline ivec3  us_to_i (const usvec3& val) { return static_cast<ivec3> (val); }
inline ihvec3 us_to_ih(const usvec3& val) { return static_cast<ihvec3>(val); }
inline isvec3 us_to_is(const usvec3& val) { return static_cast<isvec3>(val); }
inline ilvec4 us_to_il(const usvec4& val) { return static_cast<ilvec4>(val); }
inline ivec4  us_to_i (const usvec4& val) { return static_cast<ivec4> (val); }
inline ihvec4 us_to_ih(const usvec4& val) { return static_cast<ihvec4>(val); }
inline isvec4 us_to_is(const usvec4& val) { return static_cast<isvec4>(val); }

inline vec1  ul_to_f(const ulvec1& val) { return static_cast<vec1> (val); }
inline vec2  ul_to_f(const ulvec2& val) { return static_cast<vec2> (val); }
inline vec3  ul_to_f(const ulvec3& val) { return static_cast<vec3> (val); }
inline vec4  ul_to_f(const ulvec4& val) { return static_cast<vec4> (val); }

inline dvec1 ul_to_d(const ulvec1& val) { return static_cast<dvec1>(val); }
inline dvec2 ul_to_d(const ulvec2& val) { return static_cast<dvec2>(val); }
inline dvec3 ul_to_d(const ulvec3& val) { return static_cast<dvec3>(val); }
inline dvec4 ul_to_d(const ulvec4& val) { return static_cast<dvec4>(val); }

inline vec1  u_to_f (const uvec1& val)  { return static_cast<vec1> (val); }
inline vec2  u_to_f (const uvec2& val)  { return static_cast<vec2> (val); }
inline vec3  u_to_f (const uvec3& val)  { return static_cast<vec3> (val); }
inline vec4  u_to_f (const uvec4& val)  { return static_cast<vec4> (val); }

inline dvec1 u_to_d (const uvec1& val)  { return static_cast<dvec1>(val); }
inline dvec2 u_to_d (const uvec2& val)  { return static_cast<dvec2>(val); }
inline dvec3 u_to_d (const uvec3& val)  { return static_cast<dvec3>(val); }
inline dvec4 u_to_d (const uvec4& val)  { return static_cast<dvec4>(val); }

inline vec1  uh_to_f(const uhvec1& val) { return static_cast<vec1> (val); }
inline vec2  uh_to_f(const uhvec2& val) { return static_cast<vec2> (val); }
inline vec3  uh_to_f(const uhvec3& val) { return static_cast<vec3> (val); }
inline vec4  uh_to_f(const uhvec4& val) { return static_cast<vec4> (val); }

inline dvec1 uh_to_d(const uhvec1& val) { return static_cast<dvec1>(val); }
inline dvec2 uh_to_d(const uhvec2& val) { return static_cast<dvec2>(val); }
inline dvec3 uh_to_d(const uhvec3& val) { return static_cast<dvec3>(val); }
inline dvec4 uh_to_d(const uhvec4& val) { return static_cast<dvec4>(val); }

inline vec1  us_to_f(const usvec1& val) { return static_cast<vec1> (val); }
inline vec2  us_to_f(const usvec2& val) { return static_cast<vec2> (val); }
inline vec3  us_to_f(const usvec3& val) { return static_cast<vec3> (val); }
inline vec4  us_to_f(const usvec4& val) { return static_cast<vec4> (val); }

inline dvec1 us_to_d(const usvec1& val) { return static_cast<dvec1>(val); }
inline dvec2 us_to_d(const usvec2& val) { return static_cast<dvec2>(val); }
inline dvec3 us_to_d(const usvec3& val) { return static_cast<dvec3>(val); }
inline dvec4 us_to_d(const usvec4& val) { return static_cast<dvec4>(val); }

// STRING

inline vec1  str_to_f(const string& val_1) { return stof(val_1); }
inline vec2  str_to_f(const string& val_1, const string& val_2) { return vec2(stof(val_1), stof(val_2)); }
inline vec3  str_to_f(const string& val_1, const string& val_2, const string& val_3) { return vec3(stof(val_1), stof(val_2), stof(val_3)); }
inline vec4  str_to_f(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return vec4(stof(val_1), stof(val_2), stof(val_3), stof(val_4)); }

inline dvec1 str_to_d(const string& val_1) { return stod(val_1); }
inline dvec2 str_to_d(const string& val_1, const string& val_2) { return dvec2(stod(val_1), stod(val_2)); }
inline dvec3 str_to_d(const string& val_1, const string& val_2, const string& val_3) { return dvec3(stod(val_1), stod(val_2), stod(val_3)); }
inline dvec4 str_to_d(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return dvec4(stod(val_1), stod(val_2), stod(val_3), stod(val_4)); }

inline ulvec1 str_to_ul(const string& val_1) { return stoull(val_1); }
inline ulvec2 str_to_ul(const string& val_1, const string& val_2) { return ulvec2(stoull(val_1), stoull(val_2)); }
inline ulvec3 str_to_ul(const string& val_1, const string& val_2, const string& val_3) { return ulvec3(stoull(val_1), stoull(val_2), stoull(val_3)); }
inline ulvec4 str_to_ul(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return ulvec4(stoull(val_1), stoull(val_2), stoull(val_3), stoull(val_4)); }

inline uvec1  str_to_u (const string& val_1) { return stoul(val_1); }
inline uvec2  str_to_u (const string& val_1, const string& val_2) { return uvec2(stoul(val_1), stoul(val_2)); }
inline uvec3  str_to_u (const string& val_1, const string& val_2, const string& val_3) { return uvec3(stoul(val_1), stoul(val_2), stoul(val_3)); }
inline uvec4  str_to_u (const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return dvec4(stoul(val_1), stoul(val_2), stoul(val_3), stoul(val_4)); }

inline uhvec1 str_to_uh(const string& val_1) { return u_to_uh(stoul(val_1)); }
inline uhvec2 str_to_uh(const string& val_1, const string& val_2) { return u_to_uh(uvec2(stoul(val_1), stoul(val_2))); }
inline uhvec3 str_to_uh(const string& val_1, const string& val_2, const string& val_3) { return u_to_uh(uvec3(stoul(val_1), stoul(val_2), stoul(val_3))); }
inline uhvec4 str_to_uh(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return u_to_uh(uvec4(stoul(val_1), stoul(val_2), stoul(val_3), stoul(val_4))); }

inline usvec1 str_to_us(const string& val_1) { return u_to_us(stoul(val_1)); }
inline usvec2 str_to_us(const string& val_1, const string& val_2) { return u_to_us(uvec2(stoul(val_1), stoul(val_2))); }
inline usvec3 str_to_us(const string& val_1, const string& val_2, const string& val_3) { return u_to_us(uvec3(stoul(val_1), stoul(val_2), stoul(val_3))); }
inline usvec4 str_to_us(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return u_to_us(uvec4(stoul(val_1), stoul(val_2), stoul(val_3), stoul(val_4))); }

inline ilvec1 str_to_il(const string& val_1) { return stoll(val_1); }
inline ilvec2 str_to_il(const string& val_1, const string& val_2) { return ilvec2(stoll(val_1), stoll(val_2)); }
inline ilvec3 str_to_il(const string& val_1, const string& val_2, const string& val_3) { return ilvec3(stoll(val_1), stoll(val_2), stoll(val_3)); }
inline ilvec4 str_to_il(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return ilvec4(stoll(val_1), stoll(val_2), stoll(val_3), stoll(val_4)); }

inline ivec1  str_to_i (const string& val_1) { return stoi(val_1); }
inline ivec2  str_to_i (const string& val_1, const string& val_2) { return ivec2(stoi(val_1), stoi(val_2)); }
inline ivec3  str_to_i (const string& val_1, const string& val_2, const string& val_3) { return ivec3(stoi(val_1), stoi(val_2), stoi(val_3)); }
inline ivec4  str_to_i (const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return ivec4(stoi(val_1), stoi(val_2), stoi(val_3), stoi(val_4)); }

inline ihvec1 str_to_ih(const string& val_1) { return i_to_ih(stoi(val_1)); }
inline ihvec2 str_to_ih(const string& val_1, const string& val_2) { return i_to_ih(ivec2(stoi(val_1), stoi(val_2))); }
inline ihvec3 str_to_ih(const string& val_1, const string& val_2, const string& val_3) { return i_to_ih(ivec3(stoi(val_1), stoi(val_2), stoi(val_3))); }
inline ihvec4 str_to_ih(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return i_to_ih(ivec4(stoi(val_1), stoi(val_2), stoi(val_3), stoi(val_4))); }

inline isvec1 str_to_is(const string& val_1) { return i_to_is(stoi(val_1)); }
inline isvec2 str_to_is(const string& val_1, const string& val_2) { return i_to_is(ivec2(stoi(val_1), stoi(val_2))); }
inline isvec3 str_to_is(const string& val_1, const string& val_2, const string& val_3) { return i_to_is(ivec3(stoi(val_1), stoi(val_2), stoi(val_3))); }
inline isvec4 str_to_is(const string& val_1, const string& val_2, const string& val_3, const string& val_4) { return i_to_is(ivec4(stoi(val_1), stoi(val_2), stoi(val_3), stoi(val_4))); }

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