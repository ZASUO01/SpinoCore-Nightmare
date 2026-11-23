#include "StreamEffect.h"
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Math/Matrix.h"
#include "SpinoCore/Utils/Random.h"

namespace Nightmare {
    namespace {
        using SpinoCore::Core::Actors::Components::Particle;
        using SpinoCore::Math::Vector3f;
        namespace Random = SpinoCore::Utils::Random;

        [[nodiscard]] Vector3f Jitter(const Vector3f& extent) {
            return Vector3f{Random::GetFloatRange(-extent.x, extent.x), Random::GetFloatRange(-extent.y, extent.y), Random::GetFloatRange(-extent.z, extent.z)};
        }
    }

    StreamEffect::StreamEffect(SpinoCore::Core::Actors::Actor& actor, const SpinoCore::Core::Actors::Components::ParticleEffectsInput& input,
                               const StreamSettings& settings)
        : ParticleEffectsComponent(actor, input)
        , mSettings(settings)
        , mVelocities(GetCapacity())
        , mAges(GetCapacity(), 0.0f)
        , mLifetimes(GetCapacity(), 1.0f) {}

    void StreamEffect::SetColors(const SpinoCore::Math::Vector4f& start, const SpinoCore::Math::Vector4f& end) {
        mSettings.startColor = start;
        mSettings.endColor = end;
    }

    void StreamEffect::Spawn(Particle& particle, const std::size_t index) {
        const Vector3f origin = GetActor()->GetWorldTransform().TransformPoint(mSettings.offset);
        particle.alive = true;
        particle.position = origin + Jitter(mSettings.spawnExtent);
        particle.size = mSettings.startSize;
        particle.color = mSettings.startColor;
        particle.rotation = Random::GetFloatRange(0.0f, 6.28f);
        mVelocities[index] = mSettings.velocity + Jitter(mSettings.velocityJitter);
        mAges[index] = 0.0f;
        mLifetimes[index] = Random::GetFloatRange(mSettings.minLife, mSettings.maxLife);
    }

    void StreamEffect::Emit(const float deltaTime, const std::span<Particle> particles) {
        if (mEmitting) mBudget += mSettings.rate * deltaTime;

        for (std::size_t index = 0; index < particles.size(); ++index) {
            Particle& particle = particles[index];
            if (!particle.alive) {
                if (mBudget >= 1.0f) {
                    mBudget -= 1.0f;
                    Spawn(particle, index);
                }
                continue;
            }

            mAges[index] += deltaTime;
            const float life = mAges[index] / mLifetimes[index];
            if (life >= 1.0f) {
                particle.alive = false;
                continue;
            }

            mVelocities[index].y -= mSettings.gravity * deltaTime;
            particle.position += mVelocities[index] * deltaTime;
            particle.color = mSettings.startColor + (mSettings.endColor - mSettings.startColor) * life;
            particle.size = mSettings.startSize + (mSettings.endSize - mSettings.startSize) * life;
        }
        if (mBudget > 1.0f) mBudget = 1.0f;
    }
}
