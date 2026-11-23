#pragma once
#include "SpinoCore/Core/Scenes/Scene.h"
#include "SpinoCore/Utils/Size2D.h"

namespace SpinoCore::Core::Scenes {
    class ScenesManager;
    struct StartContext;
    struct ControlContext;
}

namespace SpinoCore::Files {
    class FileManager;
}

namespace Nightmare {
    class SplashScreen;

    class SplashScene final : public SpinoCore::Core::Scenes::Scene {
    public:
        SplashScene(SpinoCore::Core::Scenes::ScenesManager& scenes, SpinoCore::Utils::Size2D size);

        void OnStart(const SpinoCore::Core::Scenes::StartContext& ctx) override;
        void OnSetup(const SpinoCore::Core::Scenes::ControlContext& ctx) override;
        void OnUpdate(const SpinoCore::Core::Scenes::ControlContext& ctx) override;
        void OnFinish(const SpinoCore::Core::Scenes::ControlContext&) override {}

    private:
        SpinoCore::Core::Scenes::ScenesManager& mScenes;
        SpinoCore::Utils::Size2D mSize;
        SpinoCore::Files::FileManager* mFiles{nullptr};
        SplashScreen* mScreen{nullptr};
        float mTime{0.0f};
        bool mRequested{false};
    };
}
