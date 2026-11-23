#pragma once
#include <cstddef>
#include <span>
#include <vector>
#include "SpinoCore/Core/Actors/Components/ParticleEffectsComponent.h"
#include "SpinoCore/Math/Vector.h"

namespace SpinoCore::Core::Actors {
    class Actor;
}

namespace Nightmare {
    struct StreamSettings final {
        float rate{30.0f};
        SpinoCore::Math::Vector3f offset{SpinoCore::Math::Vector3f::Zero()};
        SpinoCore::Math::Vector3f spawnExtent{0.1f, 0.1f, 0.1f};
        SpinoCore::Math::Vector3f velocity{SpinoCore::Math::Vector3f::Zero()};
        SpinoCore::Math::Vector3f velocityJitter{0.5f, 0.5f, 0.5f};
        float gravity{0.0f};
        float minLife{0.3f};
        float maxLife{0.7f};
        float startSize{0.2f};
        float endSize{0.05f};
        SpinoCore::Math::Vector4f startColor{1.0f, 1.0f, 1.0f, 1.0f};
        SpinoCore::Math::Vector4f endColor{1.0f, 1.0f, 1.0f, 0.0f};
    };

    class StreamEffect final : public SpinoCore::Core::Actors::Components::ParticleEffectsComponent {
    public:
        StreamEffect(SpinoCore::Core::Actors::Actor& actor, const SpinoCore::Core::Actors::Components::ParticleEffectsInput& input, const StreamSettings& settings);

        void SetEmitting(const bool emitting) { mEmitting = emitting; }
        void SetRate(const float rate) { mSettings.rate = rate; }
        void SetColors(const SpinoCore::Math::Vector4f& start, const SpinoCore::Math::Vector4f& end);
        void SetVelocity(const SpinoCore::Math::Vector3f& velocity) { mSettings.velocity = velocity; }

    protected:
        void Emit(float deltaTime, std::span<SpinoCore::Core::Actors::Components::Particle> particles) override;

    private:
        void Spawn(SpinoCore::Core::Actors::Components::Particle& particle, std::size_t index);

        StreamSettings mSettings;
        std::vector<SpinoCore::Math::Vector3f> mVelocities;
        std::vector<float> mAges;
        std::vector<float> mLifetimes;
        float mBudget{0.0f};
        bool mEmitting{true};
    };
}
