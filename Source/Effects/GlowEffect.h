#pragma once
#include <span>
#include <vector>
#include "SpinoCore/Core/Actors/Components/ParticleEffectsComponent.h"
#include "SpinoCore/Math/Vector.h"

namespace SpinoCore::Core::Actors {
    class Actor;
}

namespace Nightmare {
    struct GlowPoint final {
        SpinoCore::Math::Vector3f offset{SpinoCore::Math::Vector3f::Zero()};
        float size{1.0f};
        SpinoCore::Math::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
        float flicker{0.0f};
    };

    class GlowEffect final : public SpinoCore::Core::Actors::Components::ParticleEffectsComponent {
    public:
        GlowEffect(SpinoCore::Core::Actors::Actor& actor, std::vector<GlowPoint> points);

        void SetIntensity(const float intensity) { mIntensity = intensity; }
        void SetColor(const SpinoCore::Math::Vector4f& color);

    protected:
        void Emit(float deltaTime, std::span<SpinoCore::Core::Actors::Components::Particle> particles) override;

    private:
        std::vector<GlowPoint> mPoints;
        float mTime{0.0f};
        float mIntensity{1.0f};
    };
}
