#pragma once
#ifdef VMATH
#include "vmath.h"

typedef vmath::vec1 vec1;
typedef vmath::ivec1 ivec1;
typedef vmath::uvec1 uvec1;
typedef vmath::dvec1 dvec1;

typedef vmath::vec2 vec2;
typedef vmath::ivec2 ivec2;
typedef vmath::uvec2 uvec2;
typedef vmath::dvec2 dvec2;

typedef vmath::vec3 vec3;
typedef vmath::ivec3 ivec3;
typedef vmath::uvec3 uvec3;
typedef vmath::dvec3 dvec3;

typedef vmath::vec4 vec4;
typedef vmath::ivec4 ivec4;
typedef vmath::uvec4 uvec4;
typedef vmath::dvec4 dvec4;

typedef vmath::quaternion quaternion;
typedef vmath::iquaternion iquaternion;
typedef vmath::uquaternion uquaternion;
typedef vmath::dquaternion dquaternion;

typedef vmath::mat4 mat4;
typedef vmath::imat4 imat4;
typedef vmath::umat4 umat4;
typedef vmath::dmat4 dmat4;

#else

#endif