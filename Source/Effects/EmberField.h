#pragma once
#include <cstddef>
#include <span>
#include <vector>
#include "SpinoCore/Core/Actors/Components/ParticleEffectsComponent.h"

namespace SpinoCore::Core::Actors {
    class Actor;
}

namespace Nightmare {
    class EmberField final : public SpinoCore::Core::Actors::Components::ParticleEffectsComponent {
    public:
        EmberField(SpinoCore::Core::Actors::Actor& actor, std::size_t count);

        void SetFlowSpeed(const float speed) { mFlowSpeed = speed; }

    protected:
        void Emit(float deltaTime, std::span<SpinoCore::Core::Actors::Components::Particle> particles) override;

    private:
        void Respawn(SpinoCore::Core::Actors::Components::Particle& particle, std::size_t index, bool anywhere);

        std::vector<float> mPhases;
        std::vector<float> mBaseSizes;
        float mFlowSpeed{0.0f};
        float mTime{0.0f};
    };
}
