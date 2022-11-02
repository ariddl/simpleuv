#include "ACAuthResponsePacket.h"

ACAuthResponsePacket::ACAuthResponsePacket()
        : AuthToClientPacket(AuthToClientPacketType::ACAuthResponse)
        , accountId(0) {}

void ACAuthResponsePacket::SerializeBody(suv::serialize::Serializer *serializer)
{
    serializer->Value("accountId", accountId);
}