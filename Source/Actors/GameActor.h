#pragma once
#include "SpinoCore/Core/Actors/Actor.h"

namespace SpinoCore::Core::Input {
    class InputState;
}

namespace Nightmare {
    class GameActor : public SpinoCore::Core::Actors::Actor {
    public:
        explicit GameActor(const SpinoCore::Core::Actors::ActorInput& input = {}) : Actor(input) {}

        [[nodiscard]] virtual bool IsExpired() const { return false; }

    protected:
        void OnUpdate(float) override {}
        void OnProcessInput(const SpinoCore::Core::Input::InputState&) override {}
    };
}
