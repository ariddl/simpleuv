#pragma once
#include "INetHandler.h"
#include "INetPacket.h"
#include "NetConnection.h"
#include "AuthServer.h"

#include "CARequestAuthPacket.h"
#include "CAEnterWorldPacket.h"

class AuthConnection : public suv::INetHandlerT<AuthConnection>
{
public:
    AuthConnection(suv::NetConnection::Ptr connection, AuthServer::Ptr server);

    bool Send(suv::INetPacket *packet);

    void HandleData(suv::NetConnection::Ptr, const uint8_t *data, size_t size) override;
    void HandleDisconnect(suv::NetConnection::Ptr, suv::NetConnection::DisconnectReason reason) override;

    void OnRequestAuth(const CARequestAuthPacket *packet);
    void OnEnterWorld(const CAEnterWorldPacket *packet);

    AuthServer::Ptr m_server;
    suv::NetConnection::Ptr m_netConnection;
};