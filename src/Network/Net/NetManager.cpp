#include "NetManager.h"
#include "NetConnection.h"

namespace suv
{
NetManager::NetManager(Loop loop, INetHandler::Ptr handler, uint16 maxConnections)
    : m_uvLoop(loop)
    , m_listenHandle(nullptr)
    , m_handler(handler)
    , m_connections()
    , m_maxConnections(maxConnections) {}

void NetManager::Listen(const std::string &address, uint16 port)
{
    // Check if already listening
    if (m_listenHandle)
        return;

    // Create listening handle
    m_listenHandle = m_uvLoop->resource<uvw::TCPHandle>();

    // First, bind events
    m_listenHandle->on<uvw::ErrorEvent>(std::bind(&NetManager::HandleListenErrorEvent, shared_from_this(),
        std::placeholders::_1, std::placeholders::_2));
    m_listenHandle->on<uvw::ListenEvent>(std::bind(&NetManager::HandleListenEvent, shared_from_this(),
        std::placeholders::_1, std::placeholders::_2));

    // Now bind the socket to the address and port provided and begin listening
    m_listenHandle->bind(address, port);
    m_listenHandle->listen();
}

NetConnection::Ptr NetManager::CreateConnection(INetHandler::Ptr handler)
{
    // Check if we have too many connections
    // Note: We keep track of our own connections instead of just letting the uvw::Loop handle that
    // since we don't own the loop and the application could be using it for other managers.
    if (m_connections.size() >= m_maxConnections)
        return nullptr;

    NetConnection::Ptr connection = std::make_shared<NetConnection>(shared_from_this(), handler);
    m_connections.insert(connection);
    return connection;
}

NetConnection::Ptr NetManager::CreateConnection(const std::string &address, uint16 port, INetHandler::Ptr handler)
{
    // Check if custom handler provided, if not use default handler
    if (!handler)
        handler = m_handler;

    // Get a client connection
    NetConnection::Ptr connection = CreateConnection(handler);
    if (!connection)
        return nullptr;

    // Start connecting and return the new connection
    connection->Connect(address, port);
    return connection;
}

void NetManager::RemoveConnection(NetConnection::Ptr connection)
{
    if (!m_connections.erase(connection))
        return;
    // todo: do stuff maybe?
}

void NetManager::HandleListenEvent(const uvw::ListenEvent &e, uvw::TCPHandle &srv)
{
    NetConnection::Ptr connection = CreateConnection(m_handler);
    if (!connection)
        return;

    // Create a new handle and initialize the connection (start reading) with it
    TCPHandle handle = m_uvLoop->resource<uvw::TCPHandle>();
    connection->Init(handle, &srv);
}

void NetManager::HandleListenErrorEvent(const uvw::ErrorEvent &e, uvw::TCPHandle &handle)
{
    switch (e.translate(e.code()))
    {
        case UV_EADDRINUSE:
            m_listenHandle->close();
            m_listenHandle = nullptr;
            m_handler->HandleBindFail();
            break;
    }
}

void NetManager::Shutdown()
{
    // Shut down listening handle if we're listening
    if (m_listenHandle)
    {
        m_listenHandle->close();
        m_listenHandle->clear();
        m_listenHandle = nullptr;
    }

    // Disconnect current connections
    while (!m_connections.empty())
        (*m_connections.begin())->Disconnect();
}
} // namespace suv