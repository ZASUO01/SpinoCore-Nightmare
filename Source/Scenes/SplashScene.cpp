#include "SplashScene.h"
#include <memory>
#include <string>
#include "Config.h"
#include "RunScene.h"
#include "Screens/SplashScreen.h"
#include "SpinoCore/Core/Scenes/Contexts.h"
#include "SpinoCore/Core/Scenes/ScenesManager.h"
#include "SpinoCore/Core/UI/ScreensManager.h"
#include "SpinoCore/Core/Window/WindowManager.h"
#include "SpinoCore/Files/FileManager.h"

namespace Nightmare {
    namespace {
        constexpr float LoadAtSeconds = 1.3f;
        constexpr float DurationSeconds = 3.4f;
    }

    SplashScene::SplashScene(SpinoCore::Core::Scenes::ScenesManager& scenes, const SpinoCore::Utils::Size2D size) : mScenes(scenes), mSize(size) {}

    void SplashScene::OnStart(const SpinoCore::Core::Scenes::StartContext& ctx) {
        mFiles = &ctx.fileManager;
        mFiles->LoadFromSinglePath(SpinoCore::Files::AssetType::Font, Config::FontId, "Fonts/JetBrainsMono.ttf");
    }

    void SplashScene::OnSetup(const SpinoCore::Core::Scenes::ControlContext& ctx) {
        mSize = ctx.windowManager.GetSize();
        auto screen = std::make_unique<SplashScreen>(mSize);
        mScreen = screen.get();
        ctx.screensManager.PushScreen(std::move(screen));
    }

    void SplashScene::OnUpdate(const SpinoCore::Core::Scenes::ControlContext& ctx) {
        mTime += Config::StepSeconds;
        mScreen->Animate(mTime, DurationSeconds);

        if (!mRequested && mTime >= LoadAtSeconds) {
            mRequested = true;
            mFiles->LoadFromManifest("manifest.json");
            return;
        }

        if (mTime < DurationSeconds || !ctx.fileManager.IsReady()) return;

        SpinoCore::Core::Scenes::ScenesManager& scenes = mScenes;
        const SpinoCore::Utils::Size2D size = ctx.windowManager.GetSize();
        scenes.PopScene();
        scenes.PushScene(std::make_unique<RunScene>(scenes, size, std::string{}));
    }
}
