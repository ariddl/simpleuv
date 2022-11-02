#pragma once
#include "AuthToClientPacket.h"

struct ACWorldCookiePacket : public AuthToClientPacket
{
    enum { ENDPOINTS = 4 };

    ACWorldCookiePacket();

    void SerializeBody(suv::serialize::Serializer *serializer) override;

    int32_t cookie;
    uint32_t ips[ENDPOINTS];
    uint16_t ports[ENDPOINTS];
};