#include "NetConnection.h"
#include "NetManager.h"
#include "INetHandler.h"

namespace suv
{
NetConnection::NetConnection(NetManagerPtr manager, INetHandlerPtr handler)
    : m_netManager(manager)
    , m_uvLoop(manager->m_uvLoop)
    , m_tcpHandle(nullptr)
    , m_handler(handler)
    , m_readBuffer{0}
    , m_readPendingBytes(0)
    , m_writePendingPackets(0)
    , m_state(State::None)
    , m_disconnectReason(DisconnectReason::None) {}

void NetConnection::Init(TCPHandle &handle, uvw::TCPHandle *acceptHandle)
{
    // Set our tcp handle and bind events
    m_tcpHandle = handle;
    m_tcpHandle->on<uvw::DataEvent>(std::bind(&NetConnection::HandleDataEvent, shared_from_this(),
        std::placeholders::_1, std::placeholders::_2));
    m_tcpHandle->on<uvw::WriteEvent>(std::bind(&NetConnection::HandleWriteEvent, shared_from_this(),
        std::placeholders::_1, std::placeholders::_2));
    m_tcpHandle->once<uvw::EndEvent>(std::bind(&NetConnection::HandleEndEvent, shared_from_this(),
        std::placeholders::_1, std::placeholders::_2));
    m_tcpHandle->on<uvw::ErrorEvent>(std::bind(&NetConnection::HandleErrorEvent, shared_from_this(),
        std::placeholders::_1, std::placeholders::_2));

    // Set socket options
    m_tcpHandle->keepAlive(true, uvw::TCPHandle::Time{30});

    // If this is NOT an incoming connection
    if (!acceptHandle)
        return;

    // This is an incoming connection, accept it and start reading now
    acceptHandle->accept(*m_tcpHandle);
    Start();
    m_handler->HandleConnectionAccept(shared_from_this());
}

void NetConnection::Connect(const std::string &host, uint16 port)
{
    // Check if we aren't already connected/connecting
    if (m_state == State::Connecting || m_state == State::Connected || m_tcpHandle)
        return;
    m_state = State::Connecting;

    // Resolve host to address asynchronously
    std::shared_ptr<uvw::GetAddrInfoReq> req = m_uvLoop->resource<uvw::GetAddrInfoReq>();
    req->once<uvw::AddrInfoEvent>(std::bind(&NetConnection::HandleAddrInfoEvent, shared_from_this(),
        std::placeholders::_1, std::placeholders::_2));
    req->once<uvw::ErrorEvent>(std::bind(&NetConnection::HandleAddrInfoErrorEvent, shared_from_this(),
        std::placeholders::_1, std::placeholders::_2));
    req->addrInfo(host, std::to_string(port));
}

void NetConnection::Start()
{
    m_state = State::Connected;
    m_tcpHandle->read();
}

void NetConnection::Send(const uint8 *data, size_t size)
{
    if (!IsConnected() || !data || !size || size > MAX_DATA_SZ)
        return;

    // Must hold onto the data until the write finishes, so copy to unique_ptr buf
    // (and also have to prepend the 16-bit packet size)
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(size + sizeof(uint16));
    uint16 *ptr = reinterpret_cast<uint16 *>(buf.get());
    *ptr++ = size & 0xffff;
    std::memcpy(reinterpret_cast<char *>(ptr), data, size);

    // Write the packet to the Auth
    m_tcpHandle->write(std::move(buf), size + sizeof(uint16));
    ++m_writePendingPackets;
}

void NetConnection::Disconnect(DisconnectReason reason)
{
    // Check if we are already disconnected (have set a reason)
    if (reason == DisconnectReason::None || m_disconnectReason != DisconnectReason::None)
        return;

    m_disconnectReason = reason;

    // Check if we don't have a tcp handle or if we aren't waiting for any data.
    if (!m_tcpHandle || !IsDataPending())
    {
        // We're not waiting on any data to send/receive; close the socket now.
        Close();
        return;
    }

    // We're initialized and have data either outgoing or incoming, so let's just notify the
    // other side that we are aborting and wait for all the data to be sent/received.
    m_tcpHandle->once<uvw::ShutdownEvent>(std::bind(&NetConnection::HandleShutdownEvent,
        shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    m_tcpHandle->shutdown();
}

void NetConnection::Close()
{
    if (m_tcpHandle)
        m_tcpHandle->close();
    DisconnectReason reason = m_disconnectReason;
    NetConnection::Ptr instance = shared_from_this();
    if (m_tcpHandle || m_state == State::Connecting)
    {
        m_handler->HandleDisconnect(instance, reason);
        m_netManager->RemoveConnection(instance);
    }
    m_tcpHandle = nullptr;
    m_state = State::Disconnected;
}

void NetConnection::HandleAddrInfoEvent(const uvw::AddrInfoEvent &e, uvw::GetAddrInfoReq &req)
{
    // Create a new tcp handle and set it as ours
    TCPHandle handle = m_uvLoop->resource<uvw::TCPHandle>();
    Init(handle);
    m_tcpHandle->once<uvw::ConnectEvent>(std::bind(&NetConnection::HandleConnectEvent,
        shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    // Begin connecting to the address we just resolved
    const sockaddr addr = *(e.data)->ai_addr;
    m_tcpHandle->connect(addr);
}

void NetConnection::HandleAddrInfoErrorEvent(const uvw::ErrorEvent &e, uvw::GetAddrInfoReq &handle)
{
    Disconnect(NetConnection::DisconnectReason::BadAddr);
}

void NetConnection::HandleConnectEvent(const uvw::ConnectEvent &e, uvw::TCPHandle &handle)
{
    m_handler->HandleConnect(shared_from_this());
    Start();
}

void NetConnection::HandleDataEvent(const uvw::DataEvent &e, uvw::TCPHandle &handle)
{
    // Copy the new data into our buffer
    size_t bytes = std::min(static_cast<size_t>(BUF_SZ) - m_readPendingBytes, e.length);
    std::memcpy(m_readBuffer + m_readPendingBytes, e.data.get(), bytes);
    m_readPendingBytes += bytes;
    size_t bytesRemaining = m_readPendingBytes;
    size_t bytesIgnored = e.length - bytes;

    // Read as many fully-received app packets in our buffer as possible
    NetConnection::Ptr instance = shared_from_this();
    const uint16 *ptr;
    for (ptr = reinterpret_cast<const uint16 *>(m_readBuffer);
         bytesRemaining > sizeof(uint16) && bytesRemaining - sizeof(uint16) >= *ptr;
         bytesRemaining -= *ptr + sizeof(uint16),
         ptr = reinterpret_cast<const uint16 *>(
             reinterpret_cast<const uint8 *>(ptr) + *ptr + sizeof(uint16)))
    {
        m_handler->HandleData(instance, reinterpret_cast<const uint8 *>(ptr + 1), *ptr);
    }

    // If we have data for a pending segment
    if (ptr != reinterpret_cast<const uint16 *>(m_readBuffer))
    {
        std::memmove(m_readBuffer, ptr, bytesRemaining);
        m_readPendingBytes = bytesRemaining;
    }

    if (BUF_SZ - m_readPendingBytes < bytesIgnored)
    {
        Disconnect(NetConnection::DisconnectReason::RecvOverflow);
        return;
    }

    std::memcpy(m_readBuffer + m_readPendingBytes, e.data.get() + bytes, bytesIgnored);
    m_readPendingBytes += bytesIgnored;
}

void NetConnection::HandleWriteEvent(const uvw::WriteEvent &e, uvw::TCPHandle &handle)
{
    --m_writePendingPackets;
}

void NetConnection::HandleEndEvent(const uvw::EndEvent &, uvw::TCPHandle &handle)
{
    Close();
}

void NetConnection::HandleErrorEvent(const uvw::ErrorEvent &e, uvw::TCPHandle &handle)
{
    Close();
}

void NetConnection::HandleShutdownEvent(const uvw::ShutdownEvent &e, uvw::TCPHandle &handle)
{
    // All the data was sent/received, now close the socket gracefully.
    Close();
}
} // namespace suv