#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include "Rules/Melody.h"

namespace {
    using Nightmare::MelodyNote;
    using Nightmare::MelodySchedule;

    TEST(MelodyScheduleTests, NotesAreReleasedWhenTheirBeatArrives) {
        constexpr std::array notes{MelodyNote{0.0f, 60}, MelodyNote{1.0f, 62}, MelodyNote{2.0f, 64}};
        MelodySchedule schedule{notes};

        const auto due = schedule.TakeDue(1.0f);

        ASSERT_EQ(due.size(), 2u);
        EXPECT_EQ(due[0].midi, 60);
        EXPECT_EQ(due[1].midi, 62);
    }

    TEST(MelodyScheduleTests, ANoteIsReleasedOnlyOnce) {
        constexpr std::array notes{MelodyNote{0.0f, 60}};
        MelodySchedule schedule{notes};
        (void)schedule.TakeDue(0.0f);

        const auto due = schedule.TakeDue(5.0f);

        EXPECT_TRUE(due.empty());
    }

    TEST(MelodyScheduleTests, NotesWrittenOutOfOrderAreReleasedInTimeOrder) {
        constexpr std::array notes{MelodyNote{2.0f, 64}, MelodyNote{1.0f, 62}};
        MelodySchedule schedule{notes};

        const auto due = schedule.TakeDue(1.5f);

        ASSERT_EQ(due.size(), 1u);
        EXPECT_EQ(due[0].midi, 62);
    }

    TEST(MelodyScheduleTests, TheScheduleIsExhaustedAfterTheLastNote) {
        constexpr std::array notes{MelodyNote{0.0f, 60}, MelodyNote{3.0f, 67}};
        MelodySchedule schedule{notes};

        (void)schedule.TakeDue(3.0f);

        EXPECT_TRUE(schedule.IsExhausted());
    }

    using Nightmare::BackingEvent;
    using Nightmare::BackingVoice;

    [[nodiscard]] bool InAMinor(const int midi) {
        const int pitchClass = midi % 12;
        return pitchClass != 1 && pitchClass != 3 && pitchClass != 6 && pitchClass != 10;
    }

    TEST(LevelMelodyTests, EveryNoteBelongsToTheAMinorKeyOfTheSong) {
        for (const MelodyNote& note : Nightmare::LevelMelody()) {
            EXPECT_TRUE(InAMinor(note.midi)) << "midi " << note.midi << " at beat " << note.beat;
        }
    }

    TEST(LevelMelodyTests, NotesFallOnTheEighthNoteGrid) {
        for (const MelodyNote& note : Nightmare::LevelMelody()) {
            const float eighths = note.beat * 2.0f;
            EXPECT_FLOAT_EQ(eighths, static_cast<float>(static_cast<int>(eighths))) << "beat " << note.beat;
        }
    }

    TEST(LevelBackingTests, EventsArePlayedInTimeOrder) {
        const auto backing = Nightmare::LevelBacking();

        EXPECT_TRUE(std::ranges::is_sorted(backing, {}, &BackingEvent::beat));
    }

    TEST(LevelBackingTests, EveryPitchedEventStaysInTheKeyOfTheSong) {
        for (const BackingEvent& event : Nightmare::LevelBacking()) {
            if (event.voice != BackingVoice::Pad && event.voice != BackingVoice::Bass) continue;
            for (const int midi : event.Notes()) EXPECT_TRUE(InAMinor(midi)) << "midi " << midi << " at beat " << event.beat;
        }
    }

    TEST(LevelBackingTests, TheDrumsStartBeforeTheFirstEnemyNote) {
        const auto backing = Nightmare::LevelBacking();
        const auto firstKick = std::ranges::find(backing, BackingVoice::Kick, &BackingEvent::voice);

        ASSERT_NE(firstKick, backing.end());
        EXPECT_LT(firstKick->beat, Nightmare::LevelMelody().front().beat);
    }

    TEST(LevelBackingTests, ThePadHoldsUntilTheLastMelodyNoteEnds) {
        float padEnd = 0.0f;
        for (const BackingEvent& event : Nightmare::LevelBacking()) {
            if (event.voice == BackingVoice::Pad) padEnd = std::max(padEnd, event.beat + event.length);
        }
        const MelodyNote& finalNote = Nightmare::LevelMelody().back();

        EXPECT_GE(padEnd, finalNote.beat + finalNote.length);
    }

    TEST(LevelBackingTests, MelodyNotesThatStartAChordAreTonesOfThatChord) {
        for (const BackingEvent& chord : Nightmare::LevelBacking()) {
            if (chord.voice != BackingVoice::Pad) continue;
            for (const MelodyNote& note : Nightmare::LevelMelody()) {
                if (note.beat != chord.beat) continue;
                const bool chordTone = std::ranges::any_of(chord.Notes(), [&note](const int tone) { return tone % 12 == note.midi % 12; });
                EXPECT_TRUE(chordTone) << "midi " << note.midi << " at beat " << note.beat;
            }
        }
    }
}
