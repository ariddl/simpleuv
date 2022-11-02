#pragma once
#include "AuthToClientPacket.h"

struct ACAuthResponsePacket : public AuthToClientPacket
{
    ACAuthResponsePacket();

    void SerializeBody(suv::serialize::Serializer *serializer) override;

    uint32_t accountId;
};