#pragma once
#include <string>
#include "GameActor.h"
#include "SpinoCore/Math/Vector.h"

namespace SpinoCore::Core::Actors::Components {
    class AnimatorComponent;
    class CapsuleColliderComponent;
}

namespace Nightmare {
    class StreamEffect;

    class RunnerActor final : public GameActor {
    public:
        explicit RunnerActor(float startX);

        void Attach(GameActor& gun);
        void MoveToLane(float x);
        void SetRunning(bool running);
        void Idle();
        void Stumble();
        void Die();
        void Celebrate();

        [[nodiscard]] SpinoCore::Math::Vector3f GetMuzzle();
        [[nodiscard]] SpinoCore::Core::Actors::Components::CapsuleColliderComponent& GetCollider() const { return *mCollider; }

    protected:
        void OnUpdate(float deltaTime) override;

    private:
        void Play(const std::string& clip, bool loop);

        SpinoCore::Core::Actors::Components::AnimatorComponent* mAnimator{nullptr};
        SpinoCore::Core::Actors::Components::CapsuleColliderComponent* mCollider{nullptr};
        StreamEffect* mDust{nullptr};
        GameActor* mGun{nullptr};
        SpinoCore::Math::Vector3f mGunScale{SpinoCore::Math::Vector3f::One()};
        float mFromX{0.0f};
        float mToX{0.0f};
        float mMoveTime{1.0f};
        float mStumbleTime{0.0f};
        bool mRunning{false};
        bool mAlive{true};
    };
}
