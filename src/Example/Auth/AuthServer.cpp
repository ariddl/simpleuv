#include <iostream>
#include "AuthServer.h"
#include "AuthConnection.h"
#include "InputSerializer.h"

#include "CARequestAuthPacket.h"
#include "CAEnterWorldPacket.h"

#include <iostream> // for debug only

void AuthServer::Init(suv::NetManager::Ptr manager)
{
    m_handler.Register<CARequestAuthPacket>(&AuthConnection::OnRequestAuth);
    m_handler.Register<CAEnterWorldPacket>(&AuthConnection::OnEnterWorld);

    manager->Listen("0.0.0.0", 1237);
}

void AuthServer::HandleBindFail()
{
    std::cerr << "AuthServer failed to bind port." << std::endl;
}

void AuthServer::HandleConnectionAccept(suv::NetConnection::Ptr connection)
{
    AuthConnection::Ptr authConnection
        = std::make_shared<AuthConnection>(connection, shared_from_this());
    connection->SetHandler(authConnection);
}

uint32_t AuthServer::HandlePacket(AuthConnection *connection, const uint8_t *data, size_t size)
{
    suv::serialize::InputSerializer serializer(data, size);
    if (!m_handler.Handle(&serializer, nullptr, connection))
    {
        std::cerr << (serializer.Ok() ? "AuthServer: unknown client packet" :
            "AuthServer: serializer error") << std::endl;
        return 0;
    }

    uint32_t offset = serializer.Offset();
    if (offset != size)
        std::cerr << "AuthServer: serializer size mismatch" << std::endl;
    return offset;
}