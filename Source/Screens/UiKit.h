#pragma once
#include <memory>
#include <string>
#include "SpinoCore/Core/UI/Elements/Text.h"
#include "SpinoCore/Graphics/Resources/FontSize.h"
#include "SpinoCore/Math/Vector.h"

namespace SpinoCore::Core::UI::Elements {
    class Button;
    class Cursor;
    class Rect;
}

namespace SpinoCore::Sound {
    class SoundManager;
}

namespace Nightmare::UiKit {
    inline constexpr int BackdropOrder = 5;
    inline constexpr int PanelOrder = 10;
    inline constexpr int ContentOrder = 20;
    inline constexpr int OverlayOrder = 40;
    inline constexpr int CursorOrder = 1000;

    [[nodiscard]] SpinoCore::Core::UI::Elements::TextInput Label(SpinoCore::Math::Vector2f center, SpinoCore::Math::Vector2f size, std::string text,
                                                                 SpinoCore::Graphics::Resources::FontSize fontSize, const SpinoCore::Math::Vector4f& color,
                                                                 SpinoCore::Core::UI::Elements::TextAlignment alignment = SpinoCore::Core::UI::Elements::TextAlignment::Center);

    [[nodiscard]] std::unique_ptr<SpinoCore::Core::UI::Elements::Rect> Box(SpinoCore::Math::Vector2f center, SpinoCore::Math::Vector2f size,
                                                                           const SpinoCore::Math::Vector4f& color, int drawOrder, float radius = 0.0f);

    struct ButtonStyle final {
        SpinoCore::Math::Vector4f color;
        SpinoCore::Math::Vector4f hover;
    };

    inline constexpr ButtonStyle Primary{{0.86f, 0.18f, 0.62f, 1.0f}, {1.0f, 0.36f, 0.76f, 1.0f}};
    inline constexpr ButtonStyle Secondary{{0.24f, 0.16f, 0.42f, 1.0f}, {0.36f, 0.24f, 0.6f, 1.0f}};

    [[nodiscard]] std::unique_ptr<SpinoCore::Core::UI::Elements::Button> MakeButton(SpinoCore::Math::Vector2f center, SpinoCore::Math::Vector2f size, std::string text,
                                                                                    const ButtonStyle& style, SpinoCore::Sound::SoundManager& sounds);

    void UpdateHover(SpinoCore::Core::UI::Elements::Button& button, const ButtonStyle& style, bool enabled);

    [[nodiscard]] std::unique_ptr<SpinoCore::Core::UI::Elements::Cursor> PointerCursor();
}
