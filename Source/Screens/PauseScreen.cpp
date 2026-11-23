#include "PauseScreen.h"
#include <memory>
#include <utility>
#include "Config.h"
#include "SpinoCore/Core/UI/Elements/Button.h"
#include "SpinoCore/Core/UI/Elements/Cursor.h"
#include "SpinoCore/Core/UI/Elements/Rect.h"
#include "SpinoCore/Core/UI/Elements/Text.h"
#include "SpinoCore/Graphics/Resources/FontSize.h"
#include "SpinoCore/Sound/SoundManager.h"
#include "UiKit.h"
#include "Visuals/Palette.h"

namespace Nightmare {
    namespace {
        namespace UI = SpinoCore::Core::UI::Elements;
        using SpinoCore::Graphics::Resources::FontSize;
    }

    PauseScreen::PauseScreen(const SpinoCore::Utils::Size2D size, SpinoCore::Sound::SoundManager& sounds) {
        const auto width = static_cast<float>(size.width);
        const auto height = static_cast<float>(size.height);
        const float cx = width * 0.5f;
        const float cy = height * 0.5f;

        (void)AddElement(UiKit::Box({cx, cy}, {width, height}, Palette::Shade, UiKit::BackdropOrder));
        auto panel = UiKit::Box({cx, cy}, PanelSize, Palette::Panel, UiKit::PanelOrder, 20.0f);
        panel->SetBorder(2.0f, Palette::PanelEdge);
        (void)AddElement(std::move(panel));
        (void)AddElement(std::make_unique<UI::Text>(UiKit::Label({cx, cy - 90.0f}, {380.0f, 56.0f}, "PAUSADO", FontSize::Size42, Palette::Ink)));

        auto resume = UiKit::MakeButton({cx, cy + 5.0f}, {300.0f, 50.0f}, "RESUMIR", UiKit::Primary, sounds);
        resume->SetOnClick([this, &sounds] {
            sounds.Play(Config::ClickSound, false, 0.8f);
            mChoice = PauseChoice::Resume;
        });
        mResume = resume.get();
        (void)AddElement(std::move(resume));

        auto quit = UiKit::MakeButton({cx, cy + 75.0f}, {300.0f, 50.0f}, "SAIR", UiKit::Secondary, sounds);
        quit->SetOnClick([this, &sounds] {
            sounds.Play(Config::ClickSound, false, 0.8f);
            mChoice = PauseChoice::Quit;
        });
        mQuit = quit.get();
        (void)AddElement(std::move(quit));
        (void)AddElement(UiKit::PointerCursor());
    }

    void PauseScreen::Refresh() {
        UiKit::UpdateHover(*mResume, UiKit::Primary, true);
        UiKit::UpdateHover(*mQuit, UiKit::Secondary, true);
    }

    PauseChoice PauseScreen::TakeChoice() {
        return std::exchange(mChoice, PauseChoice::None);
    }
}
