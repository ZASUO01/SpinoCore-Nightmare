#include "RunnerActor.h"
#include <algorithm>
#include <memory>
#include "Config.h"
#include "Effects/StreamEffect.h"
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Core/Actors/Components/AnimatorComponent.h"
#include "SpinoCore/Core/Actors/Components/CapsuleColliderComponent.h"
#include "SpinoCore/Core/Actors/Components/ModelComponent.h"
#include "SpinoCore/Core/Actors/Components/ParticleEffectsComponent.h"
#include "SpinoCore/Math/Math.h"
#include "SpinoCore/Math/Matrix.h"
#include "SpinoCore/Math/Quaternion.h"

namespace Nightmare {
    namespace {
        namespace Components = SpinoCore::Core::Actors::Components;
        using SpinoCore::Math::Quaternionf;
        using SpinoCore::Math::Vector3f;
        using SpinoCore::Math::Vector4f;

        constexpr float StumbleSeconds = 0.45f;
        constexpr float LeanRadians = 0.32f;
        constexpr float HopHeight = 0.28f;

        [[nodiscard]] Quaternionf Facing() {
            return Quaternionf{Vector3f::UnitY(), SpinoCore::Math::Constants::PI<float>};
        }
    }

    RunnerActor::RunnerActor(const float startX)
        : GameActor(SpinoCore::Core::Actors::ActorInput{.position = Vector3f{startX, 0.0f, 0.0f}, .rotation = Facing()})
        , mFromX(startX)
        , mToX(startX) {
        AddComponent(std::make_unique<Components::ModelComponent>(*this, Config::CharacterModel));

        auto animator = std::make_unique<Components::AnimatorComponent>(*this);
        mAnimator = animator.get();
        AddComponent(std::move(animator));

        Components::CapsuleColliderInput capsule{};
        capsule.radius = 0.32f;
        capsule.halfHeight = 0.6f;
        capsule.isStatic = false;
        capsule.isTrigger = true;
        auto collider = std::make_unique<Components::CapsuleColliderComponent>(*this, capsule);
        mCollider = collider.get();
        AddComponent(std::move(collider));

        Components::ParticleEffectsInput dustInput{};
        dustInput.capacity = 90;
        dustInput.blend = Components::ParticleBlend::Alpha;
        auto dust = std::make_unique<StreamEffect>(*this, dustInput, StreamSettings{
            .rate = 55.0f,
            .offset = Vector3f{0.0f, 0.08f, 0.0f},
            .spawnExtent = Vector3f{0.25f, 0.04f, 0.15f},
            .velocity = Vector3f{0.0f, 0.7f, 9.0f},
            .velocityJitter = Vector3f{0.8f, 0.4f, 1.5f},
            .gravity = 0.4f,
            .minLife = 0.25f,
            .maxLife = 0.55f,
            .startSize = 0.07f,
            .endSize = 0.3f,
            .startColor = Vector4f{0.55f, 0.45f, 0.7f, 0.28f},
            .endColor = Vector4f{0.25f, 0.18f, 0.35f, 0.0f},
        });
        mDust = dust.get();
        mDust->SetEmitting(false);
        AddComponent(std::move(dust));

        Idle();
    }

    void RunnerActor::Attach(GameActor& gun) {
        mGun = &gun;
        mGunScale = gun.GetScale();
        gun.SetScale(Vector3f::Zero());
    }

    void RunnerActor::Play(const std::string& clip, const bool loop) {
        if (mAnimator->GetCurrentClip() == clip && mAnimator->IsPlaying()) return;
        mAnimator->Play(clip, loop);
    }

    void RunnerActor::MoveToLane(const float x) {
        mFromX = GetPosition().x;
        mToX = x;
        mMoveTime = 0.0f;
    }

    void RunnerActor::SetRunning(const bool running) {
        mRunning = running;
        mDust->SetEmitting(running);
        if (running && mAlive) Play(Config::ClipRun, true);
        if (mGun && mAlive) mGun->SetScale(running ? mGunScale : Vector3f::Zero());
    }

    void RunnerActor::Idle() {
        mRunning = false;
        mDust->SetEmitting(false);
        Play(Config::ClipIdle, true);
    }

    void RunnerActor::Stumble() {
        if (!mAlive) return;
        mStumbleTime = StumbleSeconds;
        mAnimator->Play(Config::ClipHit, false);
    }

    void RunnerActor::Die() {
        mAlive = false;
        mRunning = false;
        mDust->SetEmitting(false);
        mAnimator->Play(Config::ClipDeath, false);
        if (mGun) mGun->SetScale(Vector3f::Zero());
    }

    void RunnerActor::Celebrate() {
        mRunning = false;
        mDust->SetEmitting(false);
        mAnimator->Play(Config::ClipWave, true);
        if (mGun) mGun->SetScale(Vector3f::Zero());
    }

    Vector3f RunnerActor::GetMuzzle() {
        if (mGun) return mGun->GetWorldTransform().TransformPoint(Vector3f{0.0f, 0.0f, -4.2f});
        return GetPosition() + Vector3f{0.25f, 1.3f, -0.6f};
    }

    void RunnerActor::OnUpdate(const float deltaTime) {
        mMoveTime = std::min(mMoveTime + deltaTime / Config::LaneChangeSeconds, 1.0f);
        const float eased = 1.0f - (1.0f - mMoveTime) * (1.0f - mMoveTime);
        const float x = mFromX + (mToX - mFromX) * eased;
        const float travel = mToX - mFromX;
        const float arc = SpinoCore::Math::Operations::Sin(mMoveTime * SpinoCore::Math::Constants::PI<float>);
        const float lean = travel == 0.0f ? 0.0f : (travel > 0.0f ? -1.0f : 1.0f) * LeanRadians * arc;

        SetPosition(Vector3f{x, HopHeight * arc * (travel == 0.0f ? 0.0f : 1.0f), 0.0f});
        SetRotation(Quaternionf{Vector3f::UnitZ(), lean} * Facing());

        if (mStumbleTime > 0.0f) {
            mStumbleTime -= deltaTime;
            if (mStumbleTime <= 0.0f && mRunning && mAlive) mAnimator->Play(Config::ClipRun, true);
        }
    }
}
