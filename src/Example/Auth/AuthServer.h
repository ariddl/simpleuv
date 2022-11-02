#pragma once
#include "INetHandler.h"
#include "NetManager.h"
#include "PacketHandler.h"
#include "ClientToAuthPacket.h"

class AuthConnection;
class AuthServer : public suv::INetHandlerT<AuthServer>
{
public:
    void Init(suv::NetManager::Ptr manager);

    uint32_t HandlePacket(AuthConnection *connection, const uint8_t *data, size_t size);

private:
    void HandleBindFail() override;
    void HandleConnectionAccept(suv::NetConnection::Ptr connection) override;

    suv::PacketHandler<AuthConnection, ClientToAuthPacketType,
                  static_cast<uint32_t>(ClientToAuthPacketType::_Count)> m_handler;
};