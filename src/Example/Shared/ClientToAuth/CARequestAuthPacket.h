#pragma once
#include "ClientToAuthPacket.h"

struct CARequestAuthPacket : public ClientToAuthPacket
{
    CARequestAuthPacket();

    void SerializeBody(suv::serialize::Serializer *serializer) override;

    uint64_t ticket;
};