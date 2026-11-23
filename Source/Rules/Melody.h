#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace Nightmare {
    struct MelodyNote final {
        float beat{0.0f};
        int midi{60};
        float length{1.0f};
    };

    enum class BackingVoice : std::uint8_t {
        Pad,
        Bass,
        Kick,
        Snare,
        Hat,
    };

    struct BackingEvent final {
        float beat{0.0f};
        BackingVoice voice{BackingVoice::Pad};
        std::array<int, 3> notes{};
        std::size_t noteCount{1};
        float velocity{1.0f};
        float length{1.0f};

        [[nodiscard]] std::span<const int> Notes() const { return std::span<const int>(notes).first(noteCount); }
    };

    inline constexpr float SongIntroBeats = 8.0f;

    [[nodiscard]] std::span<const MelodyNote> LevelMelody();
    [[nodiscard]] std::span<const BackingEvent> LevelBacking();

    class MelodySchedule final {
    public:
        explicit MelodySchedule(std::span<const MelodyNote> notes);

        [[nodiscard]] std::vector<MelodyNote> TakeDue(float beat);
        [[nodiscard]] bool IsExhausted() const { return mNext >= mNotes.size(); }
        [[nodiscard]] std::size_t GetCount() const { return mNotes.size(); }
        [[nodiscard]] std::size_t GetReleased() const { return mNext; }
        [[nodiscard]] float GetLastBeat() const;

    private:
        std::vector<MelodyNote> mNotes;
        std::size_t mNext{0};
    };
}
