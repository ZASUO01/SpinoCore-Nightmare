#pragma once
#include <string>
#include "SpinoCore/Core/UI/Screen.h"
#include "SpinoCore/Math/Vector.h"
#include "SpinoCore/Utils/Size2D.h"

namespace SpinoCore::Core::UI::Elements {
    class Button;
    class InputField;
    class Text;
}

namespace SpinoCore::Sound {
    class SoundManager;
}

namespace Nightmare {
    enum class MenuChoice {
        None,
        Start,
        Scores,
        Quit,
    };

    class MenuScreen final : public SpinoCore::Core::UI::Screen {
    public:
        MenuScreen(SpinoCore::Utils::Size2D size, SpinoCore::Sound::SoundManager& sounds, const std::string& name);

        void Refresh(float seconds);
        [[nodiscard]] MenuChoice TakeChoice();
        void Choose(MenuChoice choice);
        [[nodiscard]] bool CanStart() const;
        [[nodiscard]] std::string GetName() const;
        [[nodiscard]] bool IsTyping() const;
        [[nodiscard]] MenuChoice ButtonAt(const SpinoCore::Math::Vector2f& point) const;
        void SetShown(bool shown);

        void OnEnter() override {}
        void OnPause() override;
        void OnResume() override;
        void OnExit() override {}

    private:
        SpinoCore::Sound::SoundManager& mSounds;
        SpinoCore::Core::UI::Elements::InputField* mName{nullptr};
        SpinoCore::Core::UI::Elements::Button* mStart{nullptr};
        SpinoCore::Core::UI::Elements::Button* mScores{nullptr};
        SpinoCore::Core::UI::Elements::Button* mQuit{nullptr};
        SpinoCore::Core::UI::Elements::Text* mTitle{nullptr};
        SpinoCore::Core::UI::Elements::Text* mHint{nullptr};
        MenuChoice mChoice{MenuChoice::None};
    };
}
