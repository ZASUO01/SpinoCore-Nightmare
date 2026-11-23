#include "BeatClock.h"
#include <cmath>

namespace Nightmare {
    BeatClock::BeatClock(const float beatsPerMinute, const float firstBeatSeconds)
        : mSecondsPerBeat(60.0f / beatsPerMinute)
        , mFirstBeatSeconds(firstBeatSeconds) {}

    void BeatClock::Advance(const float seconds) {
        mSeconds += seconds;
    }

    void BeatClock::Reset() {
        mSeconds = 0.0f;
    }

    float BeatClock::GetBeat() const {
        return (mSeconds - mFirstBeatSeconds) / mSecondsPerBeat;
    }

    float BeatClock::SecondsAtBeat(const float beat) const {
        return mFirstBeatSeconds + beat * mSecondsPerBeat;
    }

    float BeatClock::NextSubdivisionSeconds(const int subdivisionsPerBeat) const {
        const float slots = GetBeat() * static_cast<float>(subdivisionsPerBeat);
        const float next = std::ceil(slots - 1e-4f);
        return SecondsAtBeat(next / static_cast<float>(subdivisionsPerBeat));
    }

    float BeatClock::BeatPhase() const {
        const float beat = GetBeat();
        return beat - std::floor(beat);
    }
}
