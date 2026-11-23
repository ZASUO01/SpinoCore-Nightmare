#include "UiKit.h"
#include <utility>
#include "Config.h"
#include "SpinoCore/Core/UI/Elements/Button.h"
#include "SpinoCore/Core/UI/Elements/Cursor.h"
#include "SpinoCore/Core/UI/Elements/Rect.h"
#include "SpinoCore/Sound/SoundManager.h"
#include "Visuals/Palette.h"

namespace Nightmare::UiKit {
    namespace UI = SpinoCore::Core::UI::Elements;
    using SpinoCore::Graphics::Resources::FontSize;
    using SpinoCore::Math::Vector2f;
    using SpinoCore::Math::Vector4f;

    UI::TextInput Label(const Vector2f center, const Vector2f size, std::string text, const FontSize fontSize, const Vector4f& color, const UI::TextAlignment alignment) {
        UI::TextInput input;
        input.anchor = center;
        input.size = size;
        input.text = std::move(text);
        input.font = Config::FontId;
        input.fontSize = fontSize;
        input.color = color;
        input.backgroundColor = Palette::Clear;
        input.alignment = alignment;
        input.drawOrder = ContentOrder;
        return input;
    }

    std::unique_ptr<UI::Rect> Box(const Vector2f center, const Vector2f size, const Vector4f& color, const int drawOrder, const float radius) {
        UI::RectInput input;
        input.anchor = center;
        input.size = size;
        input.backgroundColor = color;
        input.drawOrder = drawOrder;
        auto box = std::make_unique<UI::Rect>(input);
        box->SetBorderRadius(radius);
        return box;
    }

    std::unique_ptr<UI::Button> MakeButton(const Vector2f center, const Vector2f size, std::string text, const ButtonStyle& style,
                                           SpinoCore::Sound::SoundManager& sounds) {
        UI::TextInput input = Label(center, size, std::move(text), FontSize::Size24, Palette::Ink);
        input.backgroundColor = style.color;
        auto button = std::make_unique<UI::Button>(input);
        button->GetBackground().SetBorderRadius(10.0f);
        button->GetBackground().SetBorder(1.5f, Vector4f{1.0f, 1.0f, 1.0f, 0.18f});
        button->SetOnHover([&sounds] { sounds.PlayFor(Config::HoverSound, 0.3f, false, 0.35f); });
        return button;
    }

    void UpdateHover(UI::Button& button, const ButtonStyle& style, const bool enabled) {
        button.SetIsEnabled(enabled);
        if (!enabled) {
            button.GetBackground().SetBackgroundColor(Palette::Disabled);
            button.SetColor(Palette::DisabledText);
            button.SetScale(1.0f);
            return;
        }
        const bool hovered = button.IsHovered();
        button.GetBackground().SetBackgroundColor(hovered ? style.hover : style.color);
        button.SetColor(Palette::Ink);
        button.SetScale(hovered ? 1.04f : 1.0f);
    }

    std::unique_ptr<UI::Cursor> PointerCursor() {
        UI::RectInput input;
        input.size = {14.0f, 14.0f};
        input.backgroundColor = Palette::Ink;
        input.drawOrder = CursorOrder;
        auto cursor = std::make_unique<UI::Cursor>(input);
        cursor->SetBorderRadius(7.0f);
        cursor->SetBorder(2.0f, Palette::Accent);
        return cursor;
    }
}
