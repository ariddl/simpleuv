#pragma once
#include "ISerialize.h"
#include <cstdio>

namespace suv::serialize
{
class Serializer : public ISerialize
{
public:
	Serializer(uint8 *data, uint32 size)
		: m_data(data)
		, m_dataCurrent(data)
		, m_dataEnd(data + size) {}

	virtual ~Serializer() = default;

	virtual bool Ok() const override { return m_dataEnd >= m_dataCurrent; }

	virtual bool ReadBytes(const char *name, uint8 *x, size_t size) = 0;
	virtual bool WriteBytes(const char *name, const uint8 *x, size_t size) = 0;

	virtual uint32 Offset() const { return Ok() ? (m_dataCurrent - m_data) : 0; }

	virtual size_t ReadString(const char *name, char *x, uint32 maxLength) = 0;
	virtual size_t WriteString(const char *name, char *x, uint32 length) = 0;

	template <uint32 StrSize>
	void Value(char *name, char(&x)[StrSize])
	{
		IsReading() ? (x[ReadString(name, x, StrSize - 1)] = 0) : WriteString(name, x, std::strlen(x));
	}

	template <uint32 Size>
	void Value(char *name, uint8(&x)[Size])
	{
		IsReading() ? (x[ReadBytes(name, x, Size)]) : WriteBytes(name, x, Size);
	}

	void Value(const char *name, char *x, size_t size)
	{
		IsReading() ? (x[ReadString(name, x, size - 1)] = 0) : WriteString(name, x, size);
	}

	void Value(const char *name, uint8 *x, size_t size)
	{
		IsReading() ? ReadBytes(name, x, size) : WriteBytes(name, x, size);
	}

	// Must forward ISerialize's templated Value function now that we have our own Value functions as well
	using ISerialize::Value;

protected:
	bool CheckEnoughBuffer(const char *valueName, uint32_t size)
	{
		if (m_dataCurrent + size <= m_dataEnd)
			return true;
		printf("not enough buffer for %s", valueName);
		m_dataCurrent = m_dataEnd + 1;
		return false;
	}

	uint8 *m_data;
	uint8 *m_dataCurrent;
	uint8 *m_dataEnd;
};
} // namespace suv::serialize