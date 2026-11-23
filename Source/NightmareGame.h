#pragma once
#include "SpinoCore/Game/GameApplication.h"

namespace SpinoCore::Game {
    struct InitializationContext;
}

namespace Nightmare {
    class NightmareGame final : public SpinoCore::Game::GameApplication {
    public:
        void Initialize(const SpinoCore::Game::InitializationContext& ctx) override;
    };
}
