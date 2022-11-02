#pragma once
#include "Serializer.h"

namespace suv::serialize
{
class InputSerializer : public Serializer
{
public:
	using Serializer::Serializer;
	InputSerializer(const uint8 *data, uint32 size)
		: Serializer(const_cast<uint8 *>(data), size) {}

	bool IsReading() const override { return true; }

#define SERIALIZE_TYPE(T) \
bool Value(const char *name, T &x) override { return ReadValue(name, x); }
	SERIALIZE_TYPES
#undef SERIALIZE_TYPE

	bool ReadBytes(const char *name, uint8 *x, size_t size) override
	{
		if (!CheckEnoughBuffer(name, size))
			return false;
		std::memcpy(x, m_dataCurrent, size);
		m_dataCurrent += size;
		return true;
	}

	bool WriteBytes(const char *name, const uint8 *x, size_t size) override { return false; }

	size_t ReadString(const char *name, char *x, uint32 maxLength) override
	{
		uint16 len;
		if (!ReadValue(name, len) || !len)
			return 0;
		if (len > maxLength || !CheckEnoughBuffer(name, len))
			return 0;
		std::memcpy(x, m_dataCurrent, len);
		m_dataCurrent += len;
		return len;
	}

	size_t WriteString(const char *name, char *x, uint32 length) override { return 0; }

private:
	template <class B>
	bool ReadValue(const char *name, B &x)
	{
		if (!CheckEnoughBuffer(name, sizeof(B)))
			return false;
		x = *reinterpret_cast<const B *>(m_dataCurrent);
		m_dataCurrent += sizeof(B);
		return true;
	}

	template <>
	bool ReadValue(const char *name, Ang3 &x)
	{
		return Value(name, x.x), Value(name, x.y), Value(name, x.z);
	}

	template <>
	bool ReadValue(const char *name, Quat &x)
	{
		return Value(name, x.x), Value(name, x.y), Value(name, x.z), Value(name, x.w);
	}

	template <>
	bool ReadValue(const char *name, Vec3 &x)
	{
		return Value(name, x.x), Value(name, x.y), Value(name, x.z);
	}

	template <>
	bool ReadValue(const char *name, Vec2i &x)
	{
		return Value(name, x.x), Value(name, x.y);
	}

	template <>
	bool ReadValue(const char *name, Vec2 &x)
	{
		return Value(name, x.x), Value(name, x.y);
	}
};
} // namespace suv::serialize