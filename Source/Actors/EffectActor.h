#pragma once
#include "GameActor.h"

namespace Nightmare {
    class EffectActor final : public GameActor {
    public:
        using GameActor::GameActor;

        [[nodiscard]] bool IsExpired() const override;
    };
}
