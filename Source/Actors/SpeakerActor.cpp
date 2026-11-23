#include "SpeakerActor.h"
#include <algorithm>
#include <memory>
#include <vector>
#include "Cameras/RigCamera.h"
#include "Config.h"
#include "Effects/GlowEffect.h"
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Core/Actors/Components/ModelComponent.h"
#include "SpinoCore/Math/Matrix.h"
#include "SpinoCore/Math/Quaternion.h"

namespace Nightmare {
    namespace {
        using SpinoCore::Math::Quaternionf;
        using SpinoCore::Math::Vector3f;
    }

    SpeakerActor::SpeakerActor(const RigCamera& camera, const float ndcX, const float ndcY, const float depth, const float baseScale)
        : GameActor(SpinoCore::Core::Actors::ActorInput{.scale = Vector3f::Zero()})
        , mCamera(camera)
        , mNdcX(ndcX)
        , mNdcY(ndcY)
        , mDepth(depth)
        , mBaseScale(baseScale) {
        AddComponent(std::make_unique<SpinoCore::Core::Actors::Components::ModelComponent>(*this, Config::SpeakerModel));
        auto glow = std::make_unique<GlowEffect>(*this, std::vector<GlowPoint>{
            GlowPoint{.offset = Vector3f{0.0f, 0.55f, 0.55f}, .size = 0.38f, .color = SpinoCore::Math::Vector4f{0.86f, 0.18f, 0.62f, 0.3f}, .flicker = 0.0f},
            GlowPoint{.offset = Vector3f{0.0f, 1.3f, 0.55f}, .size = 0.2f, .color = SpinoCore::Math::Vector4f{0.2f, 0.95f, 1.0f, 0.25f}, .flicker = 0.0f},
        });
        mGlow = glow.get();
        AddComponent(std::move(glow));
        SetUpdateOrder(500);
    }

    void SpeakerActor::Pulse(const float strength) {
        mPulse = std::max(mPulse, strength);
    }

    Vector3f SpeakerActor::GetConePoint() {
        return GetWorldTransform().TransformPoint(Vector3f{0.0f, 0.95f, 0.5f});
    }

    void SpeakerActor::OnUpdate(const float deltaTime) {
        mPulse = std::max(0.0f, mPulse - deltaTime * 5.0f);
        mAppear = std::clamp(mAppear + (mShown ? deltaTime : -deltaTime) * 3.0f, 0.0f, 1.0f);
        const float pop = mAppear * mAppear * (3.0f - 2.0f * mAppear);

        const Vector3f forward = mCamera.GetForward();
        const float inward = mNdcX < 0.0f ? -0.35f : 0.35f;
        const Quaternionf face = Quaternionf::LookRotation(Vector3f{forward.x, 0.0f, forward.z}.Normalized()) * Quaternionf{Vector3f::UnitY(), inward};

        const float scale = mBaseScale * pop * (1.0f + 0.07f * mPulse);
        SetPosition(mCamera.ScreenAnchor(mNdcX, mNdcY, mDepth));
        SetRotation(face);
        SetScale(Vector3f{scale, scale * (1.0f + 0.04f * mPulse), scale});
        mGlow->SetIntensity(pop * (0.6f + 0.6f * mPulse));
    }
}
