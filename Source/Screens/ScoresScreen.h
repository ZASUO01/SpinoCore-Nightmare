#pragma once
#include <vector>
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
    struct ScoreRecord;

    class ScoresScreen final : public SpinoCore::Core::UI::Screen {
    public:
        static constexpr SpinoCore::Math::Vector2f PanelSize{560.0f, 560.0f};
        [[nodiscard]] static SpinoCore::Math::Vector2f PanelCenter(SpinoCore::Utils::Size2D size) {
            return {static_cast<float>(size.width) * 0.5f, static_cast<float>(size.height) * 0.5f};
        }

        ScoresScreen(SpinoCore::Utils::Size2D size, SpinoCore::Sound::SoundManager& sounds, const std::vector<ScoreRecord>& records);

        void Refresh();
        [[nodiscard]] bool TakeClose();

        void OnEnter() override {}
        void OnPause() override {}
        void OnResume() override {}
        void OnExit() override {}

    private:
        SpinoCore::Core::UI::Elements::Button* mBack{nullptr};
        bool mClose{false};
    };
}
