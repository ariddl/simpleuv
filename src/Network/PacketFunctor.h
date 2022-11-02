#pragma once
#include "PacketFunctorBase.h"
#include "PacketQueueElem.h"

namespace suv
{
template <typename Ctx, typename Enum, typename Packet>
class PacketFunctor : public PacketFunctorBase<Ctx, Enum>
{
    typedef void (Ctx::*Handler)(const Packet *packet);
    using QueueElem = PacketQueueElem<Ctx, Packet>;

public:
    PacketFunctor(Handler handler = nullptr)
        : m_handler(handler) {}

    PacketQueueElemBase<Ctx> *operator()(serialize::Serializer *serializer, Enum type) override
    {
        Packet *packet = new Packet();
        packet->m_type_u32 = static_cast<uint32>(type);
        packet->SerializeBody(serializer);
        if (serializer->Ok())
            return new QueueElem(packet, m_handler);
        delete packet;
        return nullptr;
    }
    
    bool operator()(serialize::Serializer *serializer, Enum type, Ctx *ctx) override
    {
        Packet packet;
        packet.m_type_u32 = static_cast<uint32>(type);
        packet.SerializeBody(serializer);
        return serializer->Ok() ? (ctx->*m_handler)(&packet), true : false;
    }

private:
    Handler m_handler;
};
} // namespace suv