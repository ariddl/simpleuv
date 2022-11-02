#pragma once
#include "INetPacket.h"

enum class ClientToAuthPacketType
{
    CARequestAuth,
    CAListWorld,
    CAEnterWorld,
    _Count
};

struct ClientToAuthPacket : public suv::INetPacket
{
    ClientToAuthPacket(ClientToAuthPacketType type)
        : INetPacket(static_cast<uint32_t>(type)) {}

    virtual void SerializeHeader(suv::serialize::Serializer *serializer) override { serializer->Value("type", m_type_u16); }
    virtual void SerializeBody(suv::serialize::Serializer *serialize) override {}
};