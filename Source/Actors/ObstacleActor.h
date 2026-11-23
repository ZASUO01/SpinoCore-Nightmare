#pragma once
#include "ScrollActor.h"
#include "SpinoCore/Math/Vector.h"

namespace Nightmare {
    class ObstacleActor final : public ScrollActor {
    public:
        ObstacleActor(const SpinoCore::Math::Vector3f& position, const SpinoCore::Math::Vector3f& halfSize, const WorldMotion& motion);

        void MarkHit() { mHit = true; }
        [[nodiscard]] bool WasHit() const { return mHit; }
        [[nodiscard]] bool IsExpired() const override;

    private:
        bool mHit{false};
    };
}
