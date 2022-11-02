#pragma once
#include <memory>
#include "NetConnection.h"

namespace suv
{
class INetHandler
{
public:
    using Ptr = std::shared_ptr<INetHandler>;

    virtual ~INetHandler() = default;

    virtual void HandleBindFail() {}
    virtual void HandleConnectionAccept(NetConnection::Ptr connection) {}
    virtual void HandleConnect(NetConnection::Ptr connection) {}
    virtual void HandleDisconnect(NetConnection::Ptr connection, NetConnection::DisconnectReason reason) {}
    virtual void HandleData(NetConnection::Ptr connection, const uint8_t *data, size_t size) = 0;
};

template <class T>
class INetHandlerT : public INetHandler, public std::enable_shared_from_this<T>
{
public:
    using Ptr = std::shared_ptr<T>;
};
} // namespace suv