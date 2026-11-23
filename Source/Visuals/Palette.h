#pragma once
#include <array>
#include <cstddef>
#include "SpinoCore/Math/Vector.h"

namespace Nightmare::Palette {
    using SpinoCore::Math::Vector4f;

    inline constexpr Vector4f Clear{0.0f, 0.0f, 0.0f, 0.0f};
    inline constexpr Vector4f Ink{0.96f, 0.94f, 1.0f, 1.0f};
    inline constexpr Vector4f Muted{0.68f, 0.62f, 0.82f, 1.0f};
    inline constexpr Vector4f Panel{0.07f, 0.04f, 0.12f, 0.86f};
    inline constexpr Vector4f PanelEdge{0.62f, 0.25f, 0.95f, 0.9f};
    inline constexpr Vector4f Field{0.14f, 0.09f, 0.22f, 1.0f};
    inline constexpr Vector4f Accent{0.86f, 0.18f, 0.62f, 1.0f};
    inline constexpr Vector4f AccentHover{1.0f, 0.36f, 0.76f, 1.0f};
    inline constexpr Vector4f Secondary{0.24f, 0.16f, 0.42f, 1.0f};
    inline constexpr Vector4f SecondaryHover{0.36f, 0.24f, 0.6f, 1.0f};
    inline constexpr Vector4f Disabled{0.2f, 0.17f, 0.26f, 1.0f};
    inline constexpr Vector4f DisabledText{0.48f, 0.44f, 0.56f, 1.0f};
    inline constexpr Vector4f Cyan{0.2f, 0.95f, 1.0f, 1.0f};
    inline constexpr Vector4f Gold{1.0f, 0.82f, 0.25f, 1.0f};
    inline constexpr Vector4f Danger{1.0f, 0.2f, 0.3f, 1.0f};
    inline constexpr Vector4f LifeOn{0.25f, 1.0f, 0.62f, 1.0f};
    inline constexpr Vector4f LifeLow{1.0f, 0.3f, 0.35f, 1.0f};
    inline constexpr Vector4f LifeOff{0.18f, 0.12f, 0.24f, 0.9f};
    inline constexpr Vector4f Shade{0.02f, 0.0f, 0.05f, 0.55f};

    inline constexpr std::array<Vector4f, 12> NoteColors{{
        {1.0f, 0.25f, 0.45f, 1.0f}, {1.0f, 0.4f, 0.3f, 1.0f}, {1.0f, 0.62f, 0.2f, 1.0f}, {1.0f, 0.85f, 0.2f, 1.0f},
        {0.75f, 1.0f, 0.25f, 1.0f}, {0.35f, 1.0f, 0.45f, 1.0f}, {0.2f, 1.0f, 0.8f, 1.0f}, {0.2f, 0.85f, 1.0f, 1.0f},
        {0.3f, 0.55f, 1.0f, 1.0f}, {0.55f, 0.4f, 1.0f, 1.0f}, {0.8f, 0.35f, 1.0f, 1.0f}, {1.0f, 0.3f, 0.85f, 1.0f},
    }};

    [[nodiscard]] inline Vector4f ForNote(const int midi) {
        return NoteColors[static_cast<std::size_t>(((midi % 12) + 12) % 12)];
    }
}
