#include "BurstEffect.h"
#include <algorithm>
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Math/Math.h"
#include "SpinoCore/Utils/Random.h"

namespace Nightmare {
    namespace {
        using SpinoCore::Core::Actors::Components::Particle;
        using SpinoCore::Math::Vector3f;
        using SpinoCore::Math::Vector4f;
        namespace Random = SpinoCore::Utils::Random;

        [[nodiscard]] Vector3f RandomDirection() {
            const float z = Random::GetFloatRange(-1.0f, 1.0f);
            const float angle = Random::GetFloatRange(0.0f, SpinoCore::Math::Constants::TwoPI<float>);
            const float radius = SpinoCore::Math::Operations::Sqrt(std::max(0.0f, 1.0f - z * z));
            return Vector3f{radius * SpinoCore::Math::Operations::Cos(angle), radius * SpinoCore::Math::Operations::Sin(angle), z};
        }

        [[nodiscard]] Vector4f Mix(const Vector4f& a, const Vector4f& b, const float t) {
            return a + (b - a) * t;
        }
    }

    BurstEffect::BurstEffect(SpinoCore::Core::Actors::Actor& actor, const SpinoCore::Core::Actors::Components::ParticleEffectsInput& input,
                             const BurstSettings& settings)
        : ParticleEffectsComponent(actor, input)
        , mSettings(settings)
        , mVelocities(GetCapacity())
        , mAges(GetCapacity(), 0.0f)
        , mLifetimes(GetCapacity(), 1.0f) {}

    void BurstEffect::Spawn(const std::span<Particle> particles) {
        const Vector3f origin = GetActor()->GetPosition();
        const std::size_t count = std::min(mSettings.count, particles.size());
        for (std::size_t index = 0; index < count; ++index) {
            Particle& particle = particles[index];
            const float along = count > 1 ? static_cast<float>(index) / static_cast<float>(count - 1) : 0.0f;
            const Vector3f base = mSettings.alongLine ? Vector3f::Lerp(origin, mSettings.lineEnd, along) : origin;
            particle.alive = true;
            particle.position = base + RandomDirection() * Random::GetFloatRange(0.0f, mSettings.spawnRadius);
            particle.size = mSettings.startSize;
            particle.color = mSettings.startColor;
            particle.rotation = Random::GetFloatRange(0.0f, 6.28f);
            mVelocities[index] = RandomDirection() * Random::GetFloatRange(mSettings.minSpeed, mSettings.maxSpeed) + mSettings.bias;
            mAges[index] = 0.0f;
            mLifetimes[index] = Random::GetFloatRange(mSettings.minLife, mSettings.maxLife);
        }
        mSpawned = true;
    }

    void BurstEffect::Emit(const float deltaTime, const std::span<Particle> particles) {
        if (!mSpawned) Spawn(particles);

        mAlive = 0;
        const float damping = std::max(0.0f, 1.0f - mSettings.drag * deltaTime);
        for (std::size_t index = 0; index < particles.size(); ++index) {
            Particle& particle = particles[index];
            if (!particle.alive) continue;

            mAges[index] += deltaTime;
            const float life = mAges[index] / mLifetimes[index];
            if (life >= 1.0f) {
                particle.alive = false;
                continue;
            }

            mVelocities[index].y -= mSettings.gravity * deltaTime;
            mVelocities[index] = mVelocities[index] * damping;
            particle.position += (mVelocities[index] + mSettings.drift) * deltaTime;
            particle.color = Mix(mSettings.startColor, mSettings.endColor, life);
            particle.size = mSettings.startSize + (mSettings.endSize - mSettings.startSize) * life;
            ++mAlive;
        }
    }
}
