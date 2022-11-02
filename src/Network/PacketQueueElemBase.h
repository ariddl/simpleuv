#pragma once

namespace suv
{
template <typename Ctx>
struct PacketQueueElemBase
{
    virtual ~PacketQueueElemBase() = default;
    virtual void Handle(Ctx *ctx) = 0;
};
} // namespace suv