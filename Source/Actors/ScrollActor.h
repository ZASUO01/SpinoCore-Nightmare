#pragma once
#include "GameActor.h"
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Math/Vector.h"

namespace Nightmare {
    struct WorldMotion final {
        float speed{0.0f};
    };

    class ScrollActor : public GameActor {
    public:
        ScrollActor(const SpinoCore::Core::Actors::ActorInput& input, const WorldMotion& motion) : GameActor(input), mMotion(motion) {}

    protected:
        void OnUpdate(const float deltaTime) override {
            auto position = GetPosition();
            position.z += mMotion.speed * deltaTime;
            SetPosition(position);
            OnScroll(deltaTime);
        }

        virtual void OnScroll(float) {}

        [[nodiscard]] const WorldMotion& GetMotion() const { return mMotion; }

    private:
        const WorldMotion& mMotion;
    };
}
