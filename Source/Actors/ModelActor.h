#pragma once
#include <memory>
#include <string>
#include <utility>
#include "GameActor.h"
#include "SpinoCore/Core/Actors/Components/ModelComponent.h"

namespace Nightmare {
    class ModelActor final : public GameActor {
    public:
        ModelActor(const SpinoCore::Core::Actors::ActorInput& input, std::string modelId) : GameActor(input) {
            auto model = std::make_unique<SpinoCore::Core::Actors::Components::ModelComponent>(*this, std::move(modelId));
            mModel = model.get();
            AddComponent(std::move(model));
        }

        [[nodiscard]] SpinoCore::Core::Actors::Components::ModelComponent& GetModel() const { return *mModel; }

    private:
        SpinoCore::Core::Actors::Components::ModelComponent* mModel{nullptr};
    };
}
