#include "GlowEffect.h"
#include <cstddef>
#include <utility>
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Math/Math.h"
#include "SpinoCore/Math/Matrix.h"

namespace Nightmare {
    namespace {
        [[nodiscard]] SpinoCore::Core::Actors::Components::ParticleEffectsInput GlowInput(const std::size_t count) {
            SpinoCore::Core::Actors::Components::ParticleEffectsInput input{};
            input.capacity = count;
            input.blend = SpinoCore::Core::Actors::Components::ParticleBlend::Additive;
            return input;
        }
    }

    GlowEffect::GlowEffect(SpinoCore::Core::Actors::Actor& actor, std::vector<GlowPoint> points)
        : ParticleEffectsComponent(actor, GlowInput(points.size()))
        , mPoints(std::move(points)) {}

    void GlowEffect::SetColor(const SpinoCore::Math::Vector4f& color) {
        for (GlowPoint& point : mPoints) point.color = color;
    }

    void GlowEffect::Emit(const float deltaTime, const std::span<SpinoCore::Core::Actors::Components::Particle> particles) {
        mTime += deltaTime;
        const SpinoCore::Math::Matrix4f& world = GetActor()->GetWorldTransform();
        for (std::size_t index = 0; index < particles.size() && index < mPoints.size(); ++index) {
            const GlowPoint& point = mPoints[index];
            const float wave = 1.0f - point.flicker * (0.5f + 0.5f * SpinoCore::Math::Operations::Sin(mTime * 9.0f + static_cast<float>(index) * 1.7f));
            auto& particle = particles[index];
            particle.alive = mIntensity > 0.0f;
            particle.position = world.TransformPoint(point.offset);
            particle.size = point.size * (0.85f + 0.15f * wave) * mIntensity;
            particle.color = SpinoCore::Math::Vector4f{point.color.x, point.color.y, point.color.z, point.color.w * wave};
        }
    }
}
