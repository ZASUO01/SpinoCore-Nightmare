#include "MenuScreen.h"
#include <memory>
#include <utility>
#include "Config.h"
#include "Rules/ScoreRecords.h"
#include "SpinoCore/Core/UI/Elements/Button.h"
#include "SpinoCore/Core/UI/Elements/Cursor.h"
#include "SpinoCore/Core/UI/Elements/InputField.h"
#include "SpinoCore/Core/UI/Elements/Rect.h"
#include "SpinoCore/Core/UI/Elements/Text.h"
#include "SpinoCore/Graphics/Resources/FontSize.h"
#include "SpinoCore/Math/Math.h"
#include "SpinoCore/Sound/SoundManager.h"
#include "UiKit.h"
#include "Visuals/Palette.h"

namespace Nightmare {
    namespace {
        namespace UI = SpinoCore::Core::UI::Elements;
        using SpinoCore::Graphics::Resources::FontSize;
        using SpinoCore::Math::Vector2f;
        using SpinoCore::Math::Vector4f;
    }

    MenuScreen::MenuScreen(const SpinoCore::Utils::Size2D size, SpinoCore::Sound::SoundManager& sounds, const std::string& name) : mSounds(sounds) {
        const auto width = static_cast<float>(size.width);
        const auto height = static_cast<float>(size.height);
        const float panelX = width * 0.74f;
        const float panelY = height * 0.56f;

        auto title = std::make_unique<UI::Text>(UiKit::Label({panelX, 92.0f}, {640.0f, 80.0f}, "SPINOCORE", FontSize::Size60, Palette::Ink));
        mTitle = title.get();
        (void)AddElement(std::move(title));
        (void)AddElement(std::make_unique<UI::Text>(UiKit::Label({panelX, 150.0f}, {640.0f, 50.0f}, "N I G H T M A R E", FontSize::Size36, Palette::Accent)));

        auto panel = UiKit::Box({panelX, panelY + 40.0f}, {400.0f, 330.0f}, Palette::Panel, UiKit::PanelOrder, 18.0f);
        panel->SetBorder(1.5f, Palette::PanelEdge);
        (void)AddElement(std::move(panel));

        (void)AddElement(std::make_unique<UI::Text>(UiKit::Label({panelX, panelY - 90.0f}, {320.0f, 24.0f}, "Seu nome", FontSize::Size16, Palette::Muted, UI::TextAlignment::Left)));

        UI::InputFieldInput field;
        field.anchor = {panelX, panelY - 55.0f};
        field.size = {320.0f, 44.0f};
        field.backgroundColor = Palette::Field;
        field.textColor = Palette::Ink;
        field.font = Config::FontId;
        field.fontSize = FontSize::Size24;
        field.padding = 12.0f;
        field.drawOrder = UiKit::ContentOrder;
        field.text = name;
        auto nameField = std::make_unique<UI::InputField>(field);
        nameField->SetBorderRadius(10.0f);
        nameField->SetBorder(1.5f, Palette::PanelEdge);
        mName = nameField.get();
        (void)AddElement(std::move(nameField));

        auto start = UiKit::MakeButton({panelX, panelY + 15.0f}, {320.0f, 50.0f}, "INICIAR", UiKit::Primary, sounds);
        start->SetOnClick([this] { Choose(MenuChoice::Start); });
        mStart = start.get();
        (void)AddElement(std::move(start));

        auto scores = UiKit::MakeButton({panelX, panelY + 77.0f}, {320.0f, 46.0f}, "PONTUAÇÕES", UiKit::Secondary, sounds);
        scores->SetOnClick([this] { Choose(MenuChoice::Scores); });
        mScores = scores.get();
        (void)AddElement(std::move(scores));

        auto quit = UiKit::MakeButton({panelX, panelY + 135.0f}, {320.0f, 46.0f}, "SAIR", UiKit::Secondary, sounds);
        quit->SetOnClick([this] { Choose(MenuChoice::Quit); });
        mQuit = quit.get();
        (void)AddElement(std::move(quit));

        auto hint = std::make_unique<UI::Text>(UiKit::Label({panelX, height - 40.0f}, {560.0f, 22.0f}, "A/D ou setas: trocar de faixa   ·   mouse: mirar e atirar   ·   Enter: pausar", FontSize::Size12, Palette::Muted));
        mHint = hint.get();
        (void)AddElement(std::move(hint));

        (void)AddElement(UiKit::PointerCursor());
        Refresh(0.0f);
    }

    void MenuScreen::Refresh(const float seconds) {
        if (CodePointCount(mName->GetText()) > MaxNameLength) mName->SetText(SanitizeName(mName->GetText()));

        UiKit::UpdateHover(*mStart, UiKit::Primary, CanStart());
        UiKit::UpdateHover(*mScores, UiKit::Secondary, true);
        UiKit::UpdateHover(*mQuit, UiKit::Secondary, true);
        mName->SetBorder(mName->IsFocused() ? 2.5f : 1.5f, mName->IsFocused() ? Palette::Cyan : Palette::PanelEdge);

        const float glow = 0.5f + 0.5f * SpinoCore::Math::Operations::Sin(seconds * 2.2f);
        mTitle->SetColor(Vector4f{0.92f + 0.08f * glow, 0.86f + 0.1f * glow, 1.0f, 1.0f});
    }

    bool MenuScreen::CanStart() const {
        return !SanitizeName(mName->GetText()).empty();
    }

    std::string MenuScreen::GetName() const {
        return SanitizeName(mName->GetText());
    }

    bool MenuScreen::IsTyping() const {
        return mName->IsFocused();
    }

    MenuChoice MenuScreen::ButtonAt(const Vector2f& point) const {
        if (CanStart() && mStart->Contains(point)) return MenuChoice::Start;
        if (mScores->Contains(point)) return MenuChoice::Scores;
        if (mQuit->Contains(point)) return MenuChoice::Quit;
        return MenuChoice::None;
    }

    void MenuScreen::Choose(const MenuChoice choice) {
        if (choice == MenuChoice::Start && !CanStart()) return;
        mSounds.Play(choice == MenuChoice::Start ? Config::SelectSound : Config::ClickSound, false, 0.8f);
        mChoice = choice;
    }

    MenuChoice MenuScreen::TakeChoice() {
        return std::exchange(mChoice, MenuChoice::None);
    }

    void MenuScreen::SetShown(const bool shown) {
        SetIsVisible(shown);
    }

    void MenuScreen::OnPause() {
        mName->SetFocused(false);
    }

    void MenuScreen::OnResume() {}
}
