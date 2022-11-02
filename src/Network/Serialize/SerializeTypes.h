#pragma once
#include <cstdint>
#include <string>

namespace suv
{
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using String = std::string;

struct Vec2
{
	float x;
	float y;
};
struct Vec2i
{
	int32 x;
	int32 y;
};
struct Vec3
{
	float x;
	float y;
	float z;
};
struct Quat
{
	float x;
	float y;
	float z;
	float w;
};
struct Ang3
{
	float x;
	float y;
	float z;
};

#define SERIALIZE_TYPES \
	SERIALIZE_TYPE(uint64) \
	SERIALIZE_TYPE(uint32) \
	SERIALIZE_TYPE(uint16) \
	SERIALIZE_TYPE(uint8) \
	SERIALIZE_TYPE(int64) \
	SERIALIZE_TYPE(int32) \
	SERIALIZE_TYPE(int16) \
	SERIALIZE_TYPE(int8) \
	SERIALIZE_TYPE(Ang3) \
	SERIALIZE_TYPE(Quat) \
	SERIALIZE_TYPE(Vec3) \
	SERIALIZE_TYPE(Vec2i) \
	SERIALIZE_TYPE(Vec2) \
	SERIALIZE_TYPE(double) \
	SERIALIZE_TYPE(float) \
	SERIALIZE_TYPE(bool)
} // namespace suv