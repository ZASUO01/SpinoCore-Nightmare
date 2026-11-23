#pragma once
#include "GameActor.h"
#include "SpinoCore/Math/Vector.h"

namespace Nightmare {
    class GlowEffect;
    class RigCamera;

    class SpeakerActor final : public GameActor {
    public:
        SpeakerActor(const RigCamera& camera, float ndcX, float ndcY, float depth, float baseScale);

        void Pulse(float strength);
        void SetShown(bool shown) { mShown = shown; }
        [[nodiscard]] SpinoCore::Math::Vector3f GetConePoint();

    protected:
        void OnUpdate(float deltaTime) override;

    private:
        const RigCamera& mCamera;
        GlowEffect* mGlow{nullptr};
        float mNdcX;
        float mNdcY;
        float mDepth;
        float mBaseScale;
        float mPulse{0.0f};
        float mAppear{0.0f};
        bool mShown{false};
    };
}
