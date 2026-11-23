#include "ScoresScreen.h"
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include "Config.h"
#include "Rules/ScoreRecords.h"
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
        using SpinoCore::Math::Vector4f;

        [[nodiscard]] Vector4f RankColor(const std::size_t rank) {
            if (rank == 0) return Palette::Gold;
            if (rank == 1) return Vector4f{0.85f, 0.88f, 0.95f, 1.0f};
            if (rank == 2) return Vector4f{0.95f, 0.6f, 0.35f, 1.0f};
            return Palette::Ink;
        }
    }

    ScoresScreen::ScoresScreen(const SpinoCore::Utils::Size2D size, SpinoCore::Sound::SoundManager& sounds, const std::vector<ScoreRecord>& records) {
        const auto width = static_cast<float>(size.width);
        const auto height = static_cast<float>(size.height);
        const float cx = width * 0.5f;
        const float cy = height * 0.5f;

        (void)AddElement(UiKit::Box({cx, cy}, {width, height}, Palette::Shade, UiKit::BackdropOrder));
        auto panel = UiKit::Box({cx, cy}, PanelSize, Palette::Panel, UiKit::PanelOrder, 20.0f);
        panel->SetBorder(2.0f, Palette::PanelEdge);
        (void)AddElement(std::move(panel));

        (void)AddElement(std::make_unique<UI::Text>(UiKit::Label({cx, cy - 235.0f}, {500.0f, 44.0f}, "MELHORES PONTUAÇÕES", FontSize::Size30, Palette::Ink)));
        (void)AddElement(UiKit::Box({cx, cy - 205.0f}, {440.0f, 2.0f}, Palette::Accent, UiKit::ContentOrder));

        const float top = cy - 170.0f;
        constexpr float RowHeight = 34.0f;
        if (records.empty()) {
            (void)AddElement(std::make_unique<UI::Text>(UiKit::Label({cx, cy - 20.0f}, {460.0f, 30.0f}, "Nenhuma fase concluída ainda.", FontSize::Size18, Palette::Muted)));
        }
        for (std::size_t index = 0; index < records.size() && index < MaxScoreRecords; ++index) {
            const float y = top + RowHeight * static_cast<float>(index);
            const Vector4f color = RankColor(index);
            if (index % 2 == 0) (void)AddElement(UiKit::Box({cx, y}, {470.0f, RowHeight - 4.0f}, Vector4f{1.0f, 1.0f, 1.0f, 0.04f}, UiKit::PanelOrder + 1, 6.0f));
            (void)AddElement(std::make_unique<UI::Text>(UiKit::Label({cx - 200.0f, y}, {50.0f, 28.0f}, std::to_string(index + 1) + ".", FontSize::Size18, color, UI::TextAlignment::Right)));
            (void)AddElement(std::make_unique<UI::Text>(UiKit::Label({cx - 10.0f, y}, {300.0f, 28.0f}, records[index].name, FontSize::Size18, color, UI::TextAlignment::Left)));
            (void)AddElement(std::make_unique<UI::Text>(UiKit::Label({cx + 185.0f, y}, {110.0f, 28.0f}, std::to_string(records[index].score), FontSize::Size18, color, UI::TextAlignment::Right)));
        }

        auto back = UiKit::MakeButton({cx, cy + 230.0f}, {240.0f, 46.0f}, "VOLTAR", UiKit::Primary, sounds);
        back->SetOnClick([this, &sounds] {
            sounds.Play(Config::ClickSound, false, 0.8f);
            mClose = true;
        });
        mBack = back.get();
        (void)AddElement(std::move(back));
        (void)AddElement(UiKit::PointerCursor());
    }

    void ScoresScreen::Refresh() {
        UiKit::UpdateHover(*mBack, UiKit::Primary, true);
    }

    bool ScoresScreen::TakeClose() {
        return std::exchange(mClose, false);
    }
}
