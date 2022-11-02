#pragma once
#include <unordered_set>
#include <memory>
#include <uvw.hpp>
#include "SerializeTypes.h"
#include "NetConnection.h"
#include "INetHandler.h"

namespace suv
{
class NetManager : public std::enable_shared_from_this<NetManager>
{
    friend NetConnection;
public:
    using Ptr = std::shared_ptr<NetManager>;
    using Loop = std::shared_ptr<uvw::Loop>;
    using TCPHandle = std::shared_ptr<uvw::TCPHandle>;

    NetManager(Loop loop, INetHandler::Ptr handler, uint16 maxConnections);

    static Ptr Create(Loop loop, INetHandler::Ptr handler, uint16 maxConnections)
    {
        // NetManager MUST be a kept alive by a shared_ptr for events
        return std::make_shared<NetManager>(loop, handler, maxConnections);
    }

    void Listen(const std::string &address, uint16 port);

    NetConnection::Ptr CreateConnection(const std::string &address, uint16 port,
        INetHandler::Ptr handler = nullptr);

    size_t NumConnections() const { return m_connections.size(); }
    uint16 MaxConnections() const { return m_maxConnections; }

private:
    NetConnection::Ptr CreateConnection(INetHandler::Ptr handler);
    void RemoveConnection(NetConnection::Ptr connection);

    void Shutdown();

    void HandleListenEvent(const uvw::ListenEvent &e, uvw::TCPHandle &srv);
    void HandleListenErrorEvent(const uvw::ErrorEvent &e, uvw::TCPHandle &handle);

    Loop m_uvLoop;
    TCPHandle m_listenHandle;
    INetHandler::Ptr m_handler;
    std::unordered_set<NetConnection::Ptr> m_connections;
    uint16 m_maxConnections;
};
} // namespace suv