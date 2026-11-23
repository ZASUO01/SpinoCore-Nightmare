#include "EmberField.h"
#include <array>
#include <cstddef>
#include "SpinoCore/Math/Math.h"
#include "SpinoCore/Math/Vector.h"
#include "SpinoCore/Utils/Random.h"

namespace Nightmare {
    namespace {
        using SpinoCore::Core::Actors::Components::Particle;
        using SpinoCore::Math::Vector4f;
        namespace Random = SpinoCore::Utils::Random;
        namespace Operations = SpinoCore::Math::Operations;

        constexpr float NearZ = 8.0f;
        constexpr float FarZ = -90.0f;
        constexpr float HalfWidth = 16.0f;
        constexpr float MaxHeight = 12.0f;
        constexpr std::array Colors{
            Vector4f{0.85f, 0.3f, 1.0f, 0.75f},
            Vector4f{0.3f, 0.85f, 1.0f, 0.7f},
            Vector4f{1.0f, 0.45f, 0.25f, 0.7f},
            Vector4f{1.0f, 0.25f, 0.6f, 0.7f},
        };

        [[nodiscard]] SpinoCore::Core::Actors::Components::ParticleEffectsInput EmberInput(const std::size_t count) {
            SpinoCore::Core::Actors::Components::ParticleEffectsInput input{};
            input.capacity = count;
            input.blend = SpinoCore::Core::Actors::Components::ParticleBlend::Additive;
            return input;
        }
    }

    EmberField::EmberField(SpinoCore::Core::Actors::Actor& actor, const std::size_t count)
        : ParticleEffectsComponent(actor, EmberInput(count))
        , mPhases(count, 0.0f)
        , mBaseSizes(count, 0.1f) {}

    void EmberField::Respawn(Particle& particle, const std::size_t index, const bool anywhere) {
        float x = Random::GetFloatRange(-HalfWidth, HalfWidth);
        if (Operations::Abs(x) < 4.5f) x += x < 0.0f ? -4.5f : 4.5f;
        particle.alive = true;
        particle.position = {x * 0.8f, Random::GetFloatRange(0.6f, MaxHeight), anywhere ? Random::GetFloatRange(FarZ, NearZ) : FarZ};
        particle.color = Colors[static_cast<std::size_t>(Random::GetIntRange(0, static_cast<int>(Colors.size()) - 1))];
        mPhases[index] = Random::GetFloatRange(0.0f, 6.28f);
        mBaseSizes[index] = Random::GetFloatRange(0.05f, 0.16f);
        particle.size = mBaseSizes[index];
    }

    void EmberField::Emit(const float deltaTime, const std::span<Particle> particles) {
        mTime += deltaTime;
        for (std::size_t index = 0; index < particles.size(); ++index) {
            Particle& particle = particles[index];
            if (!particle.alive) {
                Respawn(particle, index, true);
                continue;
            }

            const float phase = mPhases[index];
            particle.position.z += (mFlowSpeed * 0.9f + 0.6f) * deltaTime;
            particle.position.y += (0.35f + 0.25f * Operations::Sin(mTime * 1.3f + phase)) * deltaTime;
            particle.position.x += 0.4f * Operations::Sin(mTime * 0.9f + phase * 2.0f) * deltaTime;
            particle.size = mBaseSizes[index] * (0.75f + 0.25f * Operations::Sin(mTime * 5.0f + phase));

            if (particle.position.z > NearZ || particle.position.y > MaxHeight) Respawn(particle, index, false);
        }
    }
}
