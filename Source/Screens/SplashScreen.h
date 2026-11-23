#pragma once
#include "SpinoCore/Core/UI/Screen.h"
#include "SpinoCore/Utils/Size2D.h"

namespace SpinoCore::Core::UI::Elements {
    class Rect;
    class Text;
}

namespace Nightmare {
    class SplashScreen final : public SpinoCore::Core::UI::Screen {
    public:
        explicit SplashScreen(SpinoCore::Utils::Size2D size);

        void Animate(float seconds, float duration);

        void OnEnter() override {}
        void OnPause() override {}
        void OnResume() override {}
        void OnExit() override {}

    private:
        SpinoCore::Core::UI::Elements::Text* mTitle{nullptr};
        SpinoCore::Core::UI::Elements::Text* mSubtitle{nullptr};
        SpinoCore::Core::UI::Elements::Text* mCredit{nullptr};
        SpinoCore::Core::UI::Elements::Rect* mLine{nullptr};
        SpinoCore::Core::UI::Elements::Rect* mCurtain{nullptr};
    };
}
