#pragma once
#include "Serializer.h"

namespace suv
{
class INetPacket
{
    template <typename Ctx, typename TypeEnum, typename Packet>
    friend struct PacketFunctor;
public:
    INetPacket(uint32 type = 0)
        : m_type_u32(type) {}
    virtual ~INetPacket() = default;

    virtual void SerializeHeader(serialize::Serializer *serializer) = 0;
    virtual void SerializeBody(serialize::Serializer *serialize) = 0;

    uint32 GetType() const { return m_type_u32; }

protected:
    union // Needed for proper serialization with Value()
    {
        uint32 m_type_u32;
        uint16 m_type_u16;
    };
};
} // namespace suv