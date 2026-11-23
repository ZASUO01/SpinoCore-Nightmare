#pragma once
#include "SpinoCore/Math/Vector.h"
#include "SpinoCore/Utils/Size2D.h"

namespace SpinoCore::Graphics {
    class PostProcessManager;
}

namespace Nightmare {
    class ScreenFx final {
    public:
        static void Install(SpinoCore::Graphics::PostProcessManager& postProcess);

        void Pulse(float strength);
        void Damage(float strength);
        void SetBlurTarget(float blur) { mBlurTarget = blur; }
        void KeepSharp(SpinoCore::Utils::Size2D screen, SpinoCore::Math::Vector2f center, SpinoCore::Math::Vector2f size);
        void SetDesaturateTarget(float amount) { mDesaturateTarget = amount; }
        void SetFadeTarget(float fade) { mFadeTarget = fade; }
        void SetFade(float fade) { mFade = fade; mFadeTarget = fade; }
        void Update(float deltaTime, SpinoCore::Graphics::PostProcessManager& postProcess);

    private:
        float mPulse{0.0f};
        float mDamage{0.0f};
        float mBlur{0.0f};
        float mBlurTarget{0.0f};
        float mDesaturate{0.0f};
        float mDesaturateTarget{0.0f};
        float mFade{1.0f};
        float mFadeTarget{0.0f};
        SpinoCore::Math::Vector4f mSharpRect{0.0f, 0.0f, 0.0f, 0.0f};
    };
}
