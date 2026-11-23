#include "Melody.h"
#include <algorithm>

namespace Nightmare {
    namespace {
        constexpr int GS4 = 68;
        constexpr int A4 = 69;
        constexpr int B4 = 71;
        constexpr int C5 = 72;
        constexpr int D5 = 74;
        constexpr int E5 = 76;
        constexpr int F5 = 77;
        constexpr int G5 = 79;
        constexpr int GS5 = 80;
        constexpr int A5 = 81;
        constexpr int B5 = 83;
        constexpr int C6 = 84;
        constexpr int D6 = 86;
        constexpr int E6 = 88;

        constexpr int KickNote = 33;
        constexpr std::array<int, 3> HatTones{97, 100, 106};
        constexpr std::array<int, 3> SnareTones{55, 86, 91};
        constexpr float PhraseBeats = 32.0f;
        constexpr float ChordBeats = 8.0f;
        constexpr float SongBeats = PhraseBeats * 4.0f;

        struct Chord final {
            std::array<int, 3> pad;
            int bass;
        };

        constexpr Chord AMinor{{45, 48, 52}, 45};
        constexpr Chord FMajor{{41, 45, 48}, 41};
        constexpr Chord CMajor{{48, 52, 55}, 48};
        constexpr Chord GMajor{{43, 47, 50}, 43};
        constexpr Chord DMinor{{38, 41, 45}, 38};
        constexpr Chord EMajor{{40, 44, 47}, 40};

        constexpr std::array<std::array<Chord, 4>, 4> Progressions{{
            {AMinor, FMajor, CMajor, GMajor},
            {AMinor, FMajor, CMajor, EMajor},
            {DMinor, AMinor, FMajor, EMajor},
            {AMinor, FMajor, GMajor, AMinor},
        }};

        constexpr std::array Theme{
            MelodyNote{0.0f, A4}, MelodyNote{1.0f, C5}, MelodyNote{2.0f, E5, 1.5f}, MelodyNote{3.5f, D5, 0.5f},
            MelodyNote{4.0f, C5}, MelodyNote{5.0f, B4}, MelodyNote{6.0f, A4, 2.0f},
            MelodyNote{8.0f, F5, 1.5f}, MelodyNote{9.5f, E5, 0.5f}, MelodyNote{10.0f, C5}, MelodyNote{11.0f, A4},
            MelodyNote{12.0f, C5}, MelodyNote{13.0f, F5}, MelodyNote{14.0f, E5, 2.0f},
            MelodyNote{16.0f, G5, 1.5f}, MelodyNote{17.5f, E5, 0.5f}, MelodyNote{18.0f, C5}, MelodyNote{19.0f, E5},
            MelodyNote{20.0f, G5}, MelodyNote{21.0f, C6}, MelodyNote{22.0f, B5, 2.0f},
            MelodyNote{24.0f, D6, 1.5f}, MelodyNote{25.5f, B5, 0.5f}, MelodyNote{26.0f, G5}, MelodyNote{27.0f, D5},
            MelodyNote{28.0f, B4, 2.0f}, MelodyNote{30.0f, D5}, MelodyNote{31.0f, E5},
        };

        constexpr std::array Rise{
            MelodyNote{0.0f, A4}, MelodyNote{1.0f, C5}, MelodyNote{2.0f, E5, 1.5f}, MelodyNote{3.5f, D5, 0.5f},
            MelodyNote{4.0f, C5}, MelodyNote{5.0f, B4}, MelodyNote{6.0f, A4, 2.0f},
            MelodyNote{8.0f, F5, 1.5f}, MelodyNote{9.5f, E5, 0.5f}, MelodyNote{10.0f, C5}, MelodyNote{11.0f, A4},
            MelodyNote{12.0f, C5}, MelodyNote{13.0f, F5}, MelodyNote{14.0f, E5, 2.0f},
            MelodyNote{16.0f, G5, 1.5f}, MelodyNote{17.5f, E5, 0.5f}, MelodyNote{18.0f, C5}, MelodyNote{19.0f, E5},
            MelodyNote{20.0f, G5}, MelodyNote{21.0f, C6}, MelodyNote{22.0f, B5, 2.0f},
            MelodyNote{24.0f, E5, 1.5f}, MelodyNote{25.5f, GS5, 0.5f}, MelodyNote{26.0f, B5}, MelodyNote{27.0f, GS5},
            MelodyNote{28.0f, E5, 2.0f}, MelodyNote{30.0f, B4}, MelodyNote{31.0f, GS4},
        };

        constexpr std::array Climax{
            MelodyNote{0.0f, A5}, MelodyNote{1.0f, F5}, MelodyNote{2.0f, D5}, MelodyNote{3.0f, F5},
            MelodyNote{4.0f, A5, 1.5f}, MelodyNote{5.5f, C6, 0.5f}, MelodyNote{6.0f, D6, 2.0f},
            MelodyNote{8.0f, C6}, MelodyNote{9.0f, B5}, MelodyNote{10.0f, A5}, MelodyNote{11.0f, E5},
            MelodyNote{12.0f, A5, 2.0f}, MelodyNote{14.0f, C6, 2.0f},
            MelodyNote{16.0f, C6}, MelodyNote{17.0f, A5}, MelodyNote{18.0f, F5}, MelodyNote{19.0f, A5},
            MelodyNote{20.0f, C6, 1.5f}, MelodyNote{21.5f, D6, 0.5f}, MelodyNote{22.0f, E6, 2.0f},
            MelodyNote{24.0f, E6}, MelodyNote{25.0f, D6}, MelodyNote{26.0f, B5}, MelodyNote{27.0f, GS5},
            MelodyNote{28.0f, E5, 2.0f}, MelodyNote{30.0f, GS5}, MelodyNote{31.0f, B5},
        };

        constexpr std::array Finale{
            MelodyNote{0.0f, A5, 1.5f}, MelodyNote{1.5f, G5, 0.5f}, MelodyNote{2.0f, E5}, MelodyNote{3.0f, C5},
            MelodyNote{4.0f, E5}, MelodyNote{5.0f, A5}, MelodyNote{6.0f, C6, 2.0f},
            MelodyNote{8.0f, C6}, MelodyNote{9.0f, A5}, MelodyNote{10.0f, F5}, MelodyNote{11.0f, A5},
            MelodyNote{12.0f, C6, 1.5f}, MelodyNote{13.5f, B5, 0.5f}, MelodyNote{14.0f, A5, 2.0f},
            MelodyNote{16.0f, B5}, MelodyNote{17.0f, G5}, MelodyNote{18.0f, D5}, MelodyNote{19.0f, G5},
            MelodyNote{20.0f, B5, 1.5f}, MelodyNote{21.5f, C6, 0.5f}, MelodyNote{22.0f, D6, 2.0f},
            MelodyNote{24.0f, E6, 2.0f}, MelodyNote{26.0f, C6}, MelodyNote{27.0f, B5},
            MelodyNote{28.0f, A5, 4.0f},
        };

        template <std::size_t Size>
        void Append(std::vector<MelodyNote>& notes, const std::array<MelodyNote, Size>& phrase, const int index) {
            for (const MelodyNote& note : phrase) {
                notes.push_back(MelodyNote{.beat = note.beat + PhraseBeats * static_cast<float>(index), .midi = note.midi, .length = note.length});
            }
        }

        [[nodiscard]] std::vector<MelodyNote> ComposeMelody() {
            std::vector<MelodyNote> notes;
            Append(notes, Theme, 0);
            Append(notes, Rise, 1);
            Append(notes, Climax, 2);
            Append(notes, Finale, 3);
            return notes;
        }

        [[nodiscard]] BackingEvent Hit(const float beat, const BackingVoice voice, const int note, const float velocity, const float length) {
            return BackingEvent{.beat = beat, .voice = voice, .notes = {note, 0, 0}, .noteCount = 1, .velocity = velocity, .length = length};
        }

        void AddBar(std::vector<BackingEvent>& events, const float start, const Chord& chord, const bool driving, const bool fill) {
            for (int step = 0; step < 8; ++step) {
                const float beat = start + 0.5f * static_cast<float>(step);
                const bool onBeat = step % 2 == 0;
                const int bassNote = step % 4 == 2 ? chord.bass + 12 : chord.bass;
                events.push_back(Hit(beat, BackingVoice::Bass, bassNote, onBeat ? 1.0f : 0.75f, 0.45f));
                events.push_back(BackingEvent{.beat = beat, .voice = BackingVoice::Hat, .notes = HatTones, .noteCount = 3, .velocity = onBeat ? 0.55f : 0.9f, .length = 0.1f});
            }

            for (int beatIndex = 0; beatIndex < 4; ++beatIndex) {
                const float beat = start + static_cast<float>(beatIndex);
                if (driving || beatIndex == 0 || beatIndex == 2) events.push_back(Hit(beat, BackingVoice::Kick, KickNote, 1.0f, 0.3f));
            }
            if (!driving) events.push_back(Hit(start + 2.5f, BackingVoice::Kick, KickNote, 0.8f, 0.3f));

            if (fill) {
                for (int step = 0; step < 8; ++step) {
                    const float beat = start + 2.0f + 0.25f * static_cast<float>(step);
                    events.push_back(BackingEvent{.beat = beat, .voice = BackingVoice::Snare, .notes = SnareTones, .noteCount = 3,
                                                  .velocity = 0.45f + 0.07f * static_cast<float>(step), .length = 0.15f});
                }
                events.push_back(BackingEvent{.beat = start + 1.0f, .voice = BackingVoice::Snare, .notes = SnareTones, .noteCount = 3, .velocity = 1.0f, .length = 0.2f});
                return;
            }
            for (const float backbeat : {1.0f, 3.0f}) {
                events.push_back(BackingEvent{.beat = start + backbeat, .voice = BackingVoice::Snare, .notes = SnareTones, .noteCount = 3, .velocity = 1.0f, .length = 0.2f});
            }
        }

        [[nodiscard]] std::vector<BackingEvent> ComposeBacking() {
            std::vector<BackingEvent> events;

            events.push_back(Hit(-SongIntroBeats, BackingVoice::Kick, KickNote, 1.0f, 0.3f));
            events.push_back(Hit(-SongIntroBeats + 2.0f, BackingVoice::Kick, KickNote, 1.0f, 0.3f));
            for (int step = 0; step < 8; ++step) {
                events.push_back(Hit(-SongIntroBeats + 0.5f * static_cast<float>(step), BackingVoice::Bass, AMinor.bass, step % 2 == 0 ? 0.9f : 0.6f, 0.45f));
            }
            AddBar(events, -SongIntroBeats + 4.0f, AMinor, false, true);
            events.push_back(BackingEvent{.beat = -SongIntroBeats, .voice = BackingVoice::Pad, .notes = AMinor.pad, .noteCount = 3, .velocity = 0.6f, .length = SongIntroBeats});

            for (std::size_t phrase = 0; phrase < Progressions.size(); ++phrase) {
                for (std::size_t chordIndex = 0; chordIndex < 4; ++chordIndex) {
                    const Chord& chord = Progressions[phrase][chordIndex];
                    const float chordStart = PhraseBeats * static_cast<float>(phrase) + ChordBeats * static_cast<float>(chordIndex);
                    const float padLength = phrase == Progressions.size() - 1 && chordIndex == 3 ? ChordBeats + 4.0f : ChordBeats;
                    events.push_back(BackingEvent{.beat = chordStart, .voice = BackingVoice::Pad, .notes = chord.pad, .noteCount = 3, .velocity = 0.8f, .length = padLength});

                    const bool driving = phrase == 2 || (phrase == 3 && chordIndex < 3);
                    for (int bar = 0; bar < 2; ++bar) {
                        const bool lastBarOfPhrase = chordIndex == 3 && bar == 1;
                        const bool fill = lastBarOfPhrase && phrase + 1 < Progressions.size();
                        if (phrase == Progressions.size() - 1 && lastBarOfPhrase) continue;
                        AddBar(events, chordStart + 4.0f * static_cast<float>(bar), chord, driving, fill);
                    }
                }
            }

            events.push_back(Hit(SongBeats - 4.0f, BackingVoice::Kick, KickNote, 1.0f, 0.5f));
            events.push_back(Hit(SongBeats - 4.0f, BackingVoice::Bass, AMinor.bass - 12, 1.0f, 3.5f));
            events.push_back(BackingEvent{.beat = SongBeats - 4.0f, .voice = BackingVoice::Snare, .notes = SnareTones, .noteCount = 3, .velocity = 1.0f, .length = 0.3f});

            std::ranges::stable_sort(events, {}, &BackingEvent::beat);
            return events;
        }
    }

    std::span<const MelodyNote> LevelMelody() {
        static const std::vector<MelodyNote> melody = ComposeMelody();
        return melody;
    }

    std::span<const BackingEvent> LevelBacking() {
        static const std::vector<BackingEvent> backing = ComposeBacking();
        return backing;
    }

    MelodySchedule::MelodySchedule(const std::span<const MelodyNote> notes) : mNotes(notes.begin(), notes.end()) {
        std::ranges::stable_sort(mNotes, {}, &MelodyNote::beat);
    }

    std::vector<MelodyNote> MelodySchedule::TakeDue(const float beat) {
        std::vector<MelodyNote> due;
        while (mNext < mNotes.size() && mNotes[mNext].beat <= beat) {
            due.push_back(mNotes[mNext]);
            ++mNext;
        }
        return due;
    }

    float MelodySchedule::GetLastBeat() const {
        return mNotes.empty() ? 0.0f : mNotes.back().beat;
    }
}
