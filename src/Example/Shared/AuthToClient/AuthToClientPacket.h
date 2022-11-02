#pragma once
#include "INetPacket.h"

enum class AuthToClientPacketType
{
    ACJoinResponse = 1,
    ACAuthResponse,
    ACWorldCookie
};

struct AuthToClientPacket : public suv::INetPacket
{
    AuthToClientPacket(AuthToClientPacketType type)
        : INetPacket(static_cast<uint32_t>(type)) {}

    virtual void SerializeHeader(suv::serialize::Serializer *serializer) override { serializer->Value("type", m_type_u16); }
    virtual void SerializeBody(suv::serialize::Serializer *serialize) override {}
};