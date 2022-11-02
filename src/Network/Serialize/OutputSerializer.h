#pragma once
#include "Serializer.h"

namespace suv::serialize
{
class OutputSerializer : public Serializer
{
public:
	using Serializer::Serializer;

	bool IsReading() const override { return false; }

#define SERIALIZE_TYPE(T) \
bool Value(const char *name, T &x) override { return WriteValue(name, x, policy); }
	SERIALIZE_TYPES
#undef SERIALIZE_TYPE

	bool ReadBytes(const char *name, uint8 *x, size_t size) override { return false; }

	bool WriteBytes(const char *name, const uint8 *x, size_t size) override
	{
		if (!CheckEnoughBuffer(name, size))
			return false;
		std::memcpy(m_dataCurrent, x, size);
		m_dataCurrent += size;
		return true;
	}

	size_t ReadString(const char *name, char *x, uint32 maxLength) override { return 0; }

	size_t WriteString(const char *name, char *x, uint32 length) override
	{
		if (!CheckEnoughBuffer(name, length + sizeof(uint16)))
			return 0;
		*reinterpret_cast<uint16 *>(m_dataCurrent) = length;
		m_dataCurrent += sizeof(uint16);
		std::memcpy(m_dataCurrent, x, length);
		m_dataCurrent += length;
		return length;
	}

private:
	template <class B>
	bool WriteValue(const char *name, B &x)
	{
		if (!CheckEnoughBuffer(name, sizeof(B)))
			return false;
		*reinterpret_cast<B *>(m_dataCurrent) = x;
		m_dataCurrent += sizeof(B);
		return true;
	}

	template <>
	bool WriteValue(const char *name, Ang3 &x)
	{
		return Value(name, x.x), Value(name, x.y), Value(name, x.z);
	}
	
	template <>
	bool WriteValue(const char *name, Quat &x)
	{
		return Value(name, x.x), Value(name, x.y), Value(name, x.z), Value(name, x.w);
	}

	template <>
	bool WriteValue(const char *name, Vec3 &x)
	{
		return Value(name, x.x), Value(name, x.y), Value(name, x.z);
	}

	template <>
	bool WriteValue(const char *name, Vec2i &x)
	{
		return Value(name, x.x), Value(name, x.y);
	}

	template <>
	bool WriteValue(const char *name, Vec2 &x)
	{
		return Value(name, x.x), Value(name, x.y);
	}
};
} // namespace suv::serialize