#include "ObstacleActor.h"
#include <memory>
#include "Config.h"
#include "SpinoCore/Core/Actors/Actor.h"
#include "SpinoCore/Core/Actors/Components/BoxColliderComponent.h"

namespace Nightmare {
    ObstacleActor::ObstacleActor(const SpinoCore::Math::Vector3f& position, const SpinoCore::Math::Vector3f& halfSize, const WorldMotion& motion)
        : ScrollActor(SpinoCore::Core::Actors::ActorInput{.position = position}, motion) {
        SpinoCore::Core::Actors::Components::BoxColliderInput box{};
        box.extent = halfSize;
        box.isStatic = true;
        box.isTrigger = true;
        AddComponent(std::make_unique<SpinoCore::Core::Actors::Components::BoxColliderComponent>(*this, box));
    }

    bool ObstacleActor::IsExpired() const {
        return mHit || GetPosition().z > Config::RecycleZ;
    }
}
