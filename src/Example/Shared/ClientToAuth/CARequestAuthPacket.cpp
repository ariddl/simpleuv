#include "CARequestAuthPacket.h"

CARequestAuthPacket::CARequestAuthPacket()
        : ClientToAuthPacket(ClientToAuthPacketType::CARequestAuth) {}

void CARequestAuthPacket::SerializeBody(suv::serialize::Serializer *serializer)
{
    serializer->Value("ticket", ticket);
}