#include "EffectActor.h"
#include <memory>
#include "Effects/BurstEffect.h"
#include "SpinoCore/Core/Actors/Components/Component.h"

namespace Nightmare {
    bool EffectActor::IsExpired() const {
        for (const auto& component : GetComponents()) {
            const auto* burst = dynamic_cast<const BurstEffect*>(component.get());
            if (burst && !burst->IsFinished()) return false;
        }
        return true;
    }
}
