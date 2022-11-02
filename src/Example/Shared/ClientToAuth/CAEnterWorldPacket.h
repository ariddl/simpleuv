#pragma once
#include "ClientToAuthPacket.h"

struct CAEnterWorldPacket : public ClientToAuthPacket
{
    CAEnterWorldPacket();

    void SerializeBody(suv::serialize::Serializer *serializer) override;

    uint8_t wid;
};