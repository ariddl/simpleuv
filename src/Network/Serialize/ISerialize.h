#pragma once
#include "SerializeTypes.h"

namespace suv::serialize
{
class ISerialize
{
public:
	virtual ~ISerialize() = default;

	virtual bool IsReading() const = 0;
	virtual bool Ok() const = 0;

#define SERIALIZE_TYPE(T) \
	virtual bool Value(const char *name, T &x) = 0;
	SERIALIZE_TYPES
#undef SERIALIZE_TYPE
};
} // namespace suv::serialize