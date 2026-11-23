#include "TargetActor.h"
#include <memory>
#include <vector>
#include "Config.h"
#include "Effects/GlowEffect.h"
#include "Effects/StreamEffect.h"
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Core/Actors/Components/ParticleEffectsComponent.h"
#include "SpinoCore/Core/Actors/Components/SphereColliderComponent.h"
#include "SpinoCore/Math/Math.h"
#include "SpinoCore/Utils/Random.h"
#include "Visuals/Palette.h"

namespace Nightmare {
    namespace {
        namespace Components = SpinoCore::Core::Actors::Components;
        using SpinoCore::Math::Vector3f;
        using SpinoCore::Math::Vector4f;
        namespace Operations = SpinoCore::Math::Operations;
    }

    TargetActor::TargetActor(const Vector3f& start, const float speed, const int midi, const float lengthBeats)
        : GameActor(SpinoCore::Core::Actors::ActorInput{.position = start})
        , mBase(start)
        , mColor(Palette::ForNote(midi))
        , mSpeed(speed)
        , mPhase(SpinoCore::Utils::Random::GetFloatRange(0.0f, 6.28f))
        , mMidi(midi)
        , mLengthBeats(lengthBeats) {
        Components::SphereColliderInput sphere{};
        sphere.radius = Config::TargetHitRadius;
        sphere.isStatic = true;
        sphere.isTrigger = true;
        AddComponent(std::make_unique<Components::SphereColliderComponent>(*this, sphere));

        AddComponent(std::make_unique<GlowEffect>(*this, std::vector<GlowPoint>{
            GlowPoint{.offset = Vector3f::Zero(), .size = 2.6f, .color = Vector4f{mColor.x, mColor.y, mColor.z, 0.35f}, .flicker = 0.25f},
            GlowPoint{.offset = Vector3f{0.0f, 0.1f, 0.2f}, .size = 0.9f, .color = Vector4f{mColor.x, mColor.y, mColor.z, 0.8f}, .flicker = 0.1f},
        }));

        Components::ParticleEffectsInput trailInput{};
        trailInput.capacity = 70;
        trailInput.blend = Components::ParticleBlend::Additive;
        AddComponent(std::make_unique<StreamEffect>(*this, trailInput, StreamSettings{
            .rate = 60.0f,
            .offset = Vector3f{0.0f, 0.0f, -0.3f},
            .spawnExtent = Vector3f{0.5f, 0.3f, 0.2f},
            .velocity = Vector3f{0.0f, 0.4f, -2.0f},
            .velocityJitter = Vector3f{0.4f, 0.4f, 0.4f},
            .minLife = 0.3f,
            .maxLife = 0.6f,
            .startSize = 0.28f,
            .endSize = 0.02f,
            .startColor = Vector4f{mColor.x, mColor.y, mColor.z, 0.8f},
            .endColor = Vector4f{mColor.x * 0.5f, mColor.y * 0.3f, mColor.z, 0.0f},
        }));
    }

    bool TargetActor::HasPassed() const {
        return GetPosition().z > Config::TargetPassZ;
    }

    void TargetActor::OnUpdate(const float deltaTime) {
        mTime += deltaTime;
        mBase.z += mSpeed * deltaTime;
        const Vector3f sway{0.45f * Operations::Sin(mTime * 2.1f + mPhase), 0.3f * Operations::Sin(mTime * 3.3f + mPhase * 1.7f), 0.0f};
        SetPosition(mBase + sway);
    }
}
