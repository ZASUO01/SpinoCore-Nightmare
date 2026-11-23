#include "ActorRegistry.h"
#include <algorithm>

namespace Nightmare {
    using State = SpinoCore::Core::Actors::Actor::State;

    void ActorRegistry::Destroy(GameActor* actor) {
        if (!actor || std::ranges::find(mTracked, actor) == mTracked.end()) return;
        Release(actor);
    }

    void ActorRegistry::DestroyExpired() {
        std::vector<GameActor*> expired;
        for (GameActor* actor : mTracked) {
            if (actor->IsExpired()) expired.push_back(actor);
        }
        for (GameActor* actor : expired) Destroy(actor);
    }

    void ActorRegistry::SetPaused(const bool paused) {
        mPaused = paused;
        for (GameActor* actor : mTracked) actor->SetState(paused ? State::Paused : State::Active);
    }

    void ActorRegistry::Release(GameActor* actor) {
        const std::vector<SpinoCore::Core::Actors::Actor*> children = actor->GetChildren();
        for (SpinoCore::Core::Actors::Actor* child : children) {
            const auto tracked = std::ranges::find(mTracked, child);
            if (tracked != mTracked.end()) Release(*tracked);
        }
        std::erase(mTracked, actor);
        actor->SetState(State::Destroy);
    }
}
