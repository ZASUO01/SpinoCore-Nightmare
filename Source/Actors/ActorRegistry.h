#pragma once
#include <memory>
#include <utility>
#include <vector>
#include "GameActor.h"
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Core/Actors/ActorsManager.h"

namespace Nightmare {
    class ActorRegistry final {
    public:
        explicit ActorRegistry(SpinoCore::Core::Actors::ActorsManager& actors) : mActors(actors) {}

        template <typename T>
        T* Spawn(std::unique_ptr<T> actor, GameActor* parent = nullptr) {
            T* added = actor.get();
            if (parent) added->SetParent(parent);
            if (mPaused) added->SetState(SpinoCore::Core::Actors::Actor::State::Paused);
            mTracked.push_back(added);
            mActors.AddActor(std::move(actor));
            return added;
        }

        void Destroy(GameActor* actor);
        void DestroyExpired();
        void SetPaused(bool paused);
        [[nodiscard]] bool IsPaused() const { return mPaused; }

    private:
        void Release(GameActor* actor);

        SpinoCore::Core::Actors::ActorsManager& mActors;
        std::vector<GameActor*> mTracked;
        bool mPaused{false};
    };
}
