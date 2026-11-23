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
    struct BurstSettings final {
        std::size_t count{48};
        float minSpeed{2.0f};
        float maxSpeed{8.0f};
        SpinoCore::Math::Vector3f bias{SpinoCore::Math::Vector3f::Zero()};
        SpinoCore::Math::Vector3f drift{SpinoCore::Math::Vector3f::Zero()};
        float gravity{4.0f};
        float drag{1.5f};
        float minLife{0.4f};
        float maxLife{1.0f};
        float startSize{0.3f};
        float endSize{0.05f};
        SpinoCore::Math::Vector4f startColor{1.0f, 1.0f, 1.0f, 1.0f};
        SpinoCore::Math::Vector4f endColor{1.0f, 0.3f, 0.1f, 0.0f};
        float spawnRadius{0.1f};
        bool alongLine{false};
        SpinoCore::Math::Vector3f lineEnd{SpinoCore::Math::Vector3f::Zero()};
    };

    class BurstEffect final : public SpinoCore::Core::Actors::Components::ParticleEffectsComponent {
    public:
        BurstEffect(SpinoCore::Core::Actors::Actor& actor, const SpinoCore::Core::Actors::Components::ParticleEffectsInput& input, const BurstSettings& settings);

        [[nodiscard]] bool IsFinished() const { return mSpawned && mAlive == 0; }

    protected:
        void Emit(float deltaTime, std::span<SpinoCore::Core::Actors::Components::Particle> particles) override;

    private:
        void Spawn(std::span<SpinoCore::Core::Actors::Components::Particle> particles);

        BurstSettings mSettings;
        std::vector<SpinoCore::Math::Vector3f> mVelocities;
        std::vector<float> mAges;
        std::vector<float> mLifetimes;
        std::size_t mAlive{0};
        bool mSpawned{false};
    };
}
