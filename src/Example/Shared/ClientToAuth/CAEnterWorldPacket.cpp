#include "CAEnterWorldPacket.h"

 CAEnterWorldPacket::CAEnterWorldPacket()
        : ClientToAuthPacket(ClientToAuthPacketType::CAEnterWorld)
        , wid(0) {}

void CAEnterWorldPacket::SerializeBody(suv::serialize::Serializer *serializer)
{
    serializer->Value("wid", wid);
}