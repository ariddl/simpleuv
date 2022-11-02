#include "ACWorldCookiePacket.h"

ACWorldCookiePacket::ACWorldCookiePacket()
    : AuthToClientPacket(AuthToClientPacketType::ACWorldCookie)
    , cookie(0)
    , ips{0}
    , ports{0} {}

void ACWorldCookiePacket::SerializeBody(suv::serialize::Serializer *serializer)
{
    serializer->Value("cookie", cookie);
    for (uint32_t i = 0; i < ENDPOINTS; ++i)
    {
        serializer->Value("ip", ips[i]);
        serializer->Value("port", ports[i]);
    }
}