#include "SplashScreen.h"
#include <algorithm>
#include <memory>
#include "SpinoCore/Core/UI/Elements/Rect.h"
#include "SpinoCore/Core/UI/Elements/Text.h"
#include "SpinoCore/Graphics/Resources/FontSize.h"
#include "SpinoCore/Math/Vector.h"
#include "UiKit.h"
#include "Visuals/Palette.h"

namespace Nightmare {
    namespace {
        namespace UI = SpinoCore::Core::UI::Elements;
        using SpinoCore::Graphics::Resources::FontSize;
        using SpinoCore::Math::Vector4f;

        [[nodiscard]] float Smooth(const float value) {
            const float x = std::clamp(value, 0.0f, 1.0f);
            return x * x * (3.0f - 2.0f * x);
        }

        [[nodiscard]] Vector4f WithAlpha(const Vector4f& color, const float alpha) {
            return Vector4f{color.x, color.y, color.z, color.w * alpha};
        }
    }

    SplashScreen::SplashScreen(const SpinoCore::Utils::Size2D size) {
        const auto width = static_cast<float>(size.width);
        const auto height = static_cast<float>(size.height);
        const float cx = width * 0.5f;
        const float cy = height * 0.5f;

        (void)AddElement(UiKit::Box({cx, cy}, {width, height}, Vector4f{0.02f, 0.01f, 0.04f, 1.0f}, UiKit::BackdropOrder));

        auto title = std::make_unique<UI::Text>(UiKit::Label({cx, cy - 20.0f}, {900.0f, 90.0f}, "SPINOCORE", FontSize::Size72, Palette::Ink));
        mTitle = title.get();
        (void)AddElement(std::move(title));

        auto line = UiKit::Box({cx, cy + 32.0f}, {420.0f, 3.0f}, Palette::Accent, UiKit::ContentOrder, 1.5f);
        mLine = line.get();
        (void)AddElement(std::move(line));

        auto subtitle = std::make_unique<UI::Text>(UiKit::Label({cx, cy + 62.0f}, {600.0f, 36.0f}, "E  N  G  I  N  E", FontSize::Size24, Palette::Cyan));
        mSubtitle = subtitle.get();
        (void)AddElement(std::move(subtitle));

        auto credit = std::make_unique<UI::Text>(UiKit::Label({cx, height - 48.0f}, {600.0f, 24.0f}, "SDL3  ·  OpenGL 4.6", FontSize::Size16, Palette::Muted));
        mCredit = credit.get();
        (void)AddElement(std::move(credit));

        auto curtain = UiKit::Box({cx, cy}, {width, height}, Vector4f{0.0f, 0.0f, 0.0f, 1.0f}, UiKit::OverlayOrder);
        mCurtain = curtain.get();
        (void)AddElement(std::move(curtain));

        Animate(0.0f, 1.0f);
    }

    void SplashScreen::Animate(const float seconds, const float duration) {
        const float appear = Smooth(seconds / 0.8f);
        const float leave = Smooth((seconds - (duration - 0.6f)) / 0.6f);
        mCurtain->SetBackgroundColor(Vector4f{0.0f, 0.0f, 0.0f, std::max(1.0f - appear, leave)});
        mTitle->SetScale(0.92f + 0.08f * appear);
        mTitle->SetColor(WithAlpha(Palette::Ink, appear));
        mLine->SetSize({60.0f + 420.0f * Smooth((seconds - 0.4f) / 0.9f), 3.0f});
        mSubtitle->SetColor(WithAlpha(Palette::Cyan, Smooth((seconds - 0.7f) / 0.6f)));
        mCredit->SetColor(WithAlpha(Palette::Muted, Smooth((seconds - 1.0f) / 0.6f)));
    }
}
