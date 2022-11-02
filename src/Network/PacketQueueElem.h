#pragma once
#include "PacketQueueElemBase.h"

namespace suv
{
template <typename Ctx, typename Packet>
class PacketQueueElem : public PacketQueueElemBase<Ctx>
{
    typedef void (Ctx::*Handler)(const Packet *packet);
public:
    PacketQueueElem(Packet *packet, Handler handler)
        : m_packet(packet)
        , m_handler(handler) {}

    void Handle(Ctx *ctx) override { (ctx->*m_handler)(m_packet); }

private:
    Packet *m_packet;
    Handler m_handler;
};
} // namespace suv