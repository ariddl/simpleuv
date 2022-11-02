#pragma once
#include "PacketFunctor.h"
#include <queue>

namespace suv
{
template <typename Ctx, typename Enum, uint32 Count>
class PacketHandler
{
    template <typename Packet>
    using Handler = void (Ctx::*)(const Packet *packet);
    using Functor = PacketFunctorBase<Ctx, Enum>;
    using QueueElem = PacketQueueElemBase<Ctx>;
public:
    using Queue = std::queue<QueueElem *>;

    PacketHandler()
        : m_functors{0}
        , m_skipQueue{0} {}

    template <typename Packet>
    void Register(Handler<Packet> handler, bool noQueue = true)
    {
        using F = PacketFunctor<Ctx, Enum, Packet>;
        static F localFunctors[Count];
        
        Packet p;
        uint32 index = p.GetType();
        
        localFunctors[index] = F(handler);
        m_functors[index] = &localFunctors[index];
        m_skipQueue[index] = noQueue;
    }

    bool Handle(serialize::Serializer *serializer, Queue *queue, Ctx *ctx)
    {
        uint16 type;
        if (!serializer->Value("type", type) || type >= Count || !m_functors[type])
            return false;

        Functor *f = m_functors[type];
        if (m_skipQueue[type])
            return (*f)(serializer, static_cast<Enum>(type), ctx);

        QueueElem *elem = (*f)(serializer, static_cast<Enum>(type));
        if (!elem || !queue)
            return false;
        queue->push(elem);
        return true;
    }

private:
    Functor *m_functors[Count];
    bool m_skipQueue[Count];
};
} // namespace suv