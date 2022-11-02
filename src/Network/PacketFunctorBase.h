#pragma once
#include "PacketQueueElem.h"
#include "Serializer.h"

namespace suv
{
template <typename Ctx, typename Enum>
struct PacketFunctorBase
{
    virtual ~PacketFunctorBase() = default;

    virtual PacketQueueElemBase<Ctx> *operator()(X2::Serializer *serializer, Enum type) = 0;
    virtual bool operator()(X2::Serializer *serializer, Enum type, Ctx *ctx) = 0;
};
} // namespace suv