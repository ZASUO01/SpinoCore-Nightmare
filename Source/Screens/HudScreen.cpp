#include "HudScreen.h"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>
#include "Config.h"
#include "Rules/ScoreBoard.h"
#include "SpinoCore/Core/UI/Elements/Element.h"
#include "SpinoCore/Core/UI/Elements/Image.h"
#include "SpinoCore/Core/UI/Elements/Rect.h"
#include "SpinoCore/Core/UI/Elements/Text.h"
#include "SpinoCore/Graphics/Resources/FontSize.h"
#include "UiKit.h"
#include "Visuals/Palette.h"

namespace Nightmare {
    namespace {
        namespace UI = SpinoCore::Core::UI::Elements;
        using SpinoCore::Graphics::Resources::FontSize;
        using SpinoCore::Math::Vector2f;
        using SpinoCore::Math::Vector4f;

        constexpr float CellWidth = 26.0f;
        constexpr float CellGap = 6.0f;

        [[nodiscard]] Vector4f WithAlpha(const Vector4f& color, const float alpha) {
            return Vector4f{color.x, color.y, color.z, color.w * alpha};
        }
    }

    HudScreen::HudScreen(const SpinoCore::Utils::Size2D size, const Vector2f leftAnchor, const Vector2f rightAnchor) {
        const auto width = static_cast<float>(size.width);
        const auto height = static_cast<float>(size.height);
        const float cx = width * 0.5f;

        const auto keep = [this](std::unique_ptr<UI::Element> element) -> UI::Element& {
            UI::Element& added = AddElement(std::move(element));
            mHudElements.push_back(&added);
            return added;
        };

        mProgressWidth = width - 80.0f;
        mProgressLeft = 40.0f;
        keep(UiKit::Box({cx, 14.0f}, {mProgressWidth, 4.0f}, Vector4f{1.0f, 1.0f, 1.0f, 0.12f}, UiKit::PanelOrder, 2.0f));
        mProgress = static_cast<UI::Rect*>(&keep(UiKit::Box({mProgressLeft, 14.0f}, {0.0f, 4.0f}, Palette::Accent, UiKit::ContentOrder, 2.0f)));

        const float lifeWidth = static_cast<float>(mLifeCells.size()) * (CellWidth + CellGap) - CellGap;
        keep(UiKit::Box({cx, 46.0f}, {lifeWidth + 110.0f, 38.0f}, Palette::Panel, UiKit::PanelOrder, 12.0f));
        keep(std::make_unique<UI::Text>(UiKit::Label({cx - lifeWidth * 0.5f - 26.0f, 46.0f}, {60.0f, 24.0f}, "VIDA", FontSize::Size12, Palette::Muted)));
        for (std::size_t index = 0; index < mLifeCells.size(); ++index) {
            const float x = cx - lifeWidth * 0.5f + CellWidth * 0.5f + static_cast<float>(index) * (CellWidth + CellGap) + 22.0f;
            mLifeCells[index] = static_cast<UI::Rect*>(&keep(UiKit::Box({x, 46.0f}, {CellWidth, 16.0f}, Palette::LifeOn, UiKit::ContentOrder, 4.0f)));
        }

        keep(std::make_unique<UI::Text>(UiKit::Label({leftAnchor.x, leftAnchor.y - 46.0f}, {220.0f, 22.0f}, "SCORE", FontSize::Size16, Palette::Cyan)));
        mScore = static_cast<UI::Text*>(&keep(std::make_unique<UI::Text>(UiKit::Label(leftAnchor, {240.0f, 64.0f}, "0", FontSize::Size54, Palette::Ink))));

        keep(std::make_unique<UI::Text>(UiKit::Label({rightAnchor.x, rightAnchor.y - 46.0f}, {220.0f, 22.0f}, "MULTIPLICADOR", FontSize::Size16, Palette::Accent)));
        mMultiplier = static_cast<UI::Text*>(&keep(std::make_unique<UI::Text>(UiKit::Label(rightAnchor, {240.0f, 64.0f}, "x1", FontSize::Size54, Palette::Ink))));
        mCombo = static_cast<UI::Text*>(&keep(std::make_unique<UI::Text>(UiKit::Label({rightAnchor.x, rightAnchor.y + 40.0f}, {240.0f, 22.0f}, "combo 0", FontSize::Size16, Palette::Muted))));

        auto banner = std::make_unique<UI::Text>(UiKit::Label({cx, height * 0.36f}, {1000.0f, 80.0f}, "", FontSize::Size60, Palette::Ink));
        banner->SetDrawOrder(UiKit::OverlayOrder);
        mBanner = banner.get();
        (void)AddElement(std::move(banner));
        auto bannerSub = std::make_unique<UI::Text>(UiKit::Label({cx, height * 0.36f + 56.0f}, {1000.0f, 32.0f}, "", FontSize::Size24, Palette::Ink));
        bannerSub->SetDrawOrder(UiKit::OverlayOrder);
        mBannerSub = bannerSub.get();
        (void)AddElement(std::move(bannerSub));

        UI::ImageInput crosshair;
        crosshair.anchor = {cx, height * 0.5f};
        crosshair.size = {64.0f, 64.0f};
        crosshair.backgroundColor = Palette::Clear;
        crosshair.drawOrder = UiKit::CursorOrder;
        crosshair.name = Config::AimTexture;
        auto image = std::make_unique<UI::Image>(crosshair);
        mCrosshair = image.get();
        (void)AddElement(std::move(image));

        SetHudShown(false);
    }

    void HudScreen::SetHudShown(const bool shown) {
        mShown = shown;
        for (UI::Element* element : mHudElements) element->SetIsVisible(shown);
        mCrosshair->SetIsVisible(shown);
    }

    void HudScreen::SetLife(const int life) {
        for (std::size_t index = 0; index < mLifeCells.size(); ++index) {
            const bool on = static_cast<int>(index) < life;
            mLifeCells[index]->SetBackgroundColor(on ? (life <= 3 ? Palette::LifeLow : Palette::LifeOn) : Palette::LifeOff);
        }
    }

    void HudScreen::SetScore(const int score) {
        if (score == mShownScore) return;
        mShownScore = score;
        mScore->SetText(std::to_string(score));
        mScoreKick = 1.0f;
    }

    void HudScreen::SetCombo(const int combo, const int multiplier) {
        mCombo->SetText("combo " + std::to_string(combo));
        if (multiplier != mShownMultiplier) {
            mMultiplierKick = 1.0f;
            mShownMultiplier = multiplier;
        }
        mMultiplier->SetText("x" + std::to_string(multiplier));
        mMultiplier->SetColor(multiplier >= ScoreBoard::MaxMultiplier ? Palette::Gold : (multiplier > 1 ? Palette::Accent : Palette::Ink));
    }

    void HudScreen::SetProgress(const float progress) {
        const float fill = mProgressWidth * std::clamp(progress, 0.0f, 1.0f);
        mProgress->SetSize({fill, 4.0f});
        mProgress->SetAnchor({mProgressLeft + fill * 0.5f, 14.0f});
    }

    void HudScreen::ShowBanner(const std::string& title, const std::string& subtitle, const Vector4f& color, const float seconds) {
        mBanner->SetText(title);
        mBannerSub->SetText(subtitle);
        mBannerColor = color;
        mBannerTime = 0.0f;
        mBannerDuration = seconds;
    }

    void HudScreen::Kick() {
        mCrosshairKick = 1.0f;
    }

    void HudScreen::Update(const float deltaTime) {
        mScoreKick = std::max(0.0f, mScoreKick - deltaTime * 5.0f);
        mMultiplierKick = std::max(0.0f, mMultiplierKick - deltaTime * 2.5f);
        mCrosshairKick = std::max(0.0f, mCrosshairKick - deltaTime * 7.0f);
        mScore->SetScale(1.0f + 0.18f * mScoreKick);
        mMultiplier->SetScale(1.0f + 0.5f * mMultiplierKick);
        mCrosshair->SetScale(1.0f - 0.25f * mCrosshairKick);
        mCrosshair->SetAngleDegrees(mCrosshairKick * 45.0f);

        mBannerTime += deltaTime;
        const float fadeIn = std::clamp(mBannerTime / 0.25f, 0.0f, 1.0f);
        const float fadeOut = mBannerDuration <= 0.0f ? 1.0f : std::clamp((mBannerDuration - mBannerTime) / 0.5f, 0.0f, 1.0f);
        const float alpha = fadeIn * fadeOut;
        mBanner->SetColor(WithAlpha(mBannerColor, alpha));
        mBanner->SetScale(0.85f + 0.15f * fadeIn);
        mBannerSub->SetColor(WithAlpha(Palette::Ink, alpha));
    }

    void HudScreen::SetAim(const Vector2f& position, const bool inside) {
        mCrosshair->SetAnchor(position);
        mCrosshair->SetIsVisible(mShown && inside);
    }
}
