#include "NightmareGame.h"
#include <memory>
#include "Scenes/SplashScene.h"
#include "SpinoCore/Core/Scenes/ScenesManager.h"
#include "SpinoCore/Core/Window/WindowManager.h"
#include "SpinoCore/Game/InitializationContext.h"

namespace Nightmare {
    void NightmareGame::Initialize(const SpinoCore::Game::InitializationContext& ctx) {
        ctx.scenesManager.PushScene(std::make_unique<SplashScene>(ctx.scenesManager, ctx.windowManager.GetSize()));
    }
}
