#pragma once

namespace Nightmare {
    class BeatClock final {
    public:
        BeatClock(float beatsPerMinute, float firstBeatSeconds);

        void Advance(float seconds);
        void Reset();

        [[nodiscard]] float GetSeconds() const { return mSeconds; }
        [[nodiscard]] float GetSecondsPerBeat() const { return mSecondsPerBeat; }
        [[nodiscard]] float GetBeat() const;
        [[nodiscard]] float SecondsAtBeat(float beat) const;
        [[nodiscard]] float NextSubdivisionSeconds(int subdivisionsPerBeat) const;
        [[nodiscard]] float BeatPhase() const;

    private:
        float mSecondsPerBeat;
        float mFirstBeatSeconds;
        float mSeconds{0.0f};
    };
}
