#pragma once
#include <array>
#include <string>
#include <vector>
#include "SpinoCore/Core/UI/Screen.h"
#include "SpinoCore/Math/Vector.h"
#include "SpinoCore/Utils/Size2D.h"

namespace SpinoCore::Core::UI::Elements {
    class Element;
    class Image;
    class Rect;
    class Text;
}

namespace Nightmare {
    class HudScreen final : public SpinoCore::Core::UI::Screen {
    public:
        HudScreen(SpinoCore::Utils::Size2D size, SpinoCore::Math::Vector2f leftAnchor, SpinoCore::Math::Vector2f rightAnchor);

        void SetHudShown(bool shown);
        void SetLife(int life);
        void SetScore(int score);
        void SetCombo(int combo, int multiplier);
        void SetProgress(float progress);
        void ShowBanner(const std::string& title, const std::string& subtitle, const SpinoCore::Math::Vector4f& color, float seconds);
        void Kick();
        void SetAim(const SpinoCore::Math::Vector2f& position, bool inside);
        void Update(float deltaTime);

        void OnEnter() override {}
        void OnPause() override {}
        void OnResume() override {}
        void OnExit() override {}

    private:
        std::vector<SpinoCore::Core::UI::Elements::Element*> mHudElements;
        std::array<SpinoCore::Core::UI::Elements::Rect*, 10> mLifeCells{};
        SpinoCore::Core::UI::Elements::Text* mScore{nullptr};
        SpinoCore::Core::UI::Elements::Text* mMultiplier{nullptr};
        SpinoCore::Core::UI::Elements::Text* mCombo{nullptr};
        SpinoCore::Core::UI::Elements::Rect* mProgress{nullptr};
        SpinoCore::Core::UI::Elements::Text* mBanner{nullptr};
        SpinoCore::Core::UI::Elements::Text* mBannerSub{nullptr};
        SpinoCore::Core::UI::Elements::Image* mCrosshair{nullptr};
        SpinoCore::Math::Vector4f mBannerColor{1.0f, 1.0f, 1.0f, 1.0f};
        float mProgressWidth{0.0f};
        float mProgressLeft{0.0f};
        float mBannerTime{0.0f};
        float mBannerDuration{0.0f};
        float mScoreKick{0.0f};
        float mMultiplierKick{0.0f};
        float mCrosshairKick{0.0f};
        int mShownScore{0};
        int mShownMultiplier{1};
        bool mShown{false};
    };
}
