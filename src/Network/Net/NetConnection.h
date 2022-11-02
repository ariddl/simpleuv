#pragma once
#include <memory>
#include <algorithm>
#include <uvw.hpp>
#include "SerializeTypes.h"
#ifdef min
    #undef min
#endif

namespace suv
{
class NetManager;
class INetHandler;
class NetConnection : public std::enable_shared_from_this<NetConnection>
{
    friend NetManager;
    using TCPHandle = std::shared_ptr<uvw::TCPHandle>;
    using Loop = std::shared_ptr<uvw::Loop>;
    using NetManagerPtr = std::shared_ptr<NetManager>;
    using INetHandlerPtr = std::shared_ptr<INetHandler>;
public:
    using Ptr = std::shared_ptr<NetConnection>;
    enum class State { Disconnected, Connecting, Connected, None };
    enum class DisconnectReason { None, EndedLocally, EndedRemotely, BadAddr, ConnectFail, Error, RecvOverflow };

    enum { BUF_SZ = 65536 };
    enum { MAX_DATA_SZ = BUF_SZ - sizeof(uint16) };

    NetConnection(NetManagerPtr manager, INetHandlerPtr handler);

    void Send(const uint8 *data, size_t size);
    void Disconnect() { Disconnect(DisconnectReason::EndedLocally); }

    void SetHandler(INetHandlerPtr handler) { m_handler = handler; }

    State GetState() const { return m_state; }
    bool IsConnected() const { return GetState() == State::Connected; }
    DisconnectReason GetDisconnectReason() const { return m_disconnectReason; }

private:
    void Init(TCPHandle &handle, uvw::TCPHandle *acceptHandle = nullptr);
    void Connect(const std::string &address, uint16 port);
    void Start();
    void Disconnect(DisconnectReason reason);
    void Close();

    void HandleAddrInfoEvent(const uvw::AddrInfoEvent &e, uvw::GetAddrInfoReq &req);
    void HandleAddrInfoErrorEvent(const uvw::ErrorEvent &e, uvw::GetAddrInfoReq &req);
    void HandleConnectEvent(const uvw::ConnectEvent &e, uvw::TCPHandle &handle);
    void HandleDataEvent(const uvw::DataEvent &e, uvw::TCPHandle &handle);
    void HandleWriteEvent(const uvw::WriteEvent &e, uvw::TCPHandle &handle);
    void HandleEndEvent(const uvw::EndEvent &e, uvw::TCPHandle &handle);
    void HandleErrorEvent(const uvw::ErrorEvent &e, uvw::TCPHandle &handle);
    void HandleShutdownEvent(const uvw::ShutdownEvent &e, uvw::TCPHandle &handle);

    bool IsDataPending() const { return m_readPendingBytes > 0 || m_writePendingPackets > 0; }

    NetManagerPtr m_netManager;
    Loop m_uvLoop;
    TCPHandle m_tcpHandle;
    INetHandlerPtr m_handler;
    uint8 m_readBuffer[BUF_SZ];
    uint32 m_readPendingBytes;
    uint32 m_writePendingPackets;
    State m_state;
    DisconnectReason m_disconnectReason;
};
} // namespace suv