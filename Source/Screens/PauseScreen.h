#pragma once
#include "SpinoCore/Core/UI/Screen.h"
#include "SpinoCore/Math/Vector.h"
#include "SpinoCore/Utils/Size2D.h"

namespace SpinoCore::Core::UI::Elements {
    class Button;
}

namespace SpinoCore::Sound {
    class SoundManager;
}

namespace Nightmare {
    enum class PauseChoice {
        None,
        Resume,
        Quit,
    };

    class PauseScreen final : public SpinoCore::Core::UI::Screen {
    public:
        static constexpr SpinoCore::Math::Vector2f PanelSize{420.0f, 300.0f};
        [[nodiscard]] static SpinoCore::Math::Vector2f PanelCenter(SpinoCore::Utils::Size2D size) {
            return {static_cast<float>(size.width) * 0.5f, static_cast<float>(size.height) * 0.5f};
        }

        PauseScreen(SpinoCore::Utils::Size2D size, SpinoCore::Sound::SoundManager& sounds);

        void Refresh();
        [[nodiscard]] PauseChoice TakeChoice();

        void OnEnter() override {}
        void OnPause() override {}
        void OnResume() override {}
        void OnExit() override {}

    private:
        SpinoCore::Core::UI::Elements::Button* mResume{nullptr};
        SpinoCore::Core::UI::Elements::Button* mQuit{nullptr};
        PauseChoice mChoice{PauseChoice::None};
    };
}
