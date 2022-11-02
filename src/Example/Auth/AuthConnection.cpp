#include "AuthConnection.h"
#include "OutputSerializer.h"

#include "ACAuthResponsePacket.h"
#include "ACWorldCookiePacket.h"

AuthConnection::AuthConnection(suv::NetConnection::Ptr connection, AuthServer::Ptr server)
    : m_server(server)
    , m_netConnection(connection) {}

bool AuthConnection::Send(suv::INetPacket *packet)
{
    // We limit ourselves to 65k for simplicity
    uint8_t buf[suv::NetConnection::MAX_DATA_SZ];
    suv::serialize::OutputSerializer serializer(buf, sizeof(buf));
    packet->SerializeHeader(&serializer);
    packet->SerializeBody(&serializer);
    if (!serializer.Ok())
        return false;
    m_netConnection->Send(buf, serializer.Offset());
    return true;
}

void AuthConnection::HandleData(suv::NetConnection::Ptr, const uint8_t *data, size_t size)
{
    m_server->HandlePacket(this, data, size);
}

void AuthConnection::HandleDisconnect(suv::NetConnection::Ptr, suv::NetConnection::DisconnectReason reason)
{
}

void AuthConnection::OnRequestAuth(const CARequestAuthPacket *packet)
{
    ACAuthResponsePacket auth;
    auth.accountId = 123456;
    Send(&auth);
}

void AuthConnection::OnEnterWorld(const CAEnterWorldPacket *packet)
{
    ACWorldCookiePacket cookie;
    cookie.cookie = 0xDEADBEEF;
    cookie.ips[0] = (192 << 24) | (168 << 16) | (1 << 8) | 5;
    cookie.ports[0] = 1239;
    Send(&cookie);
}