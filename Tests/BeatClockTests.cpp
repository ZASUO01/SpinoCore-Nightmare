#include <gtest/gtest.h>
#include "Rules/BeatClock.h"

namespace {
    using Nightmare::BeatClock;

    constexpr float Tolerance = 1e-4f;

    TEST(BeatClockTests, OneHundredTwentyBeatsPerMinuteIsTwoBeatsPerSecond) {
        BeatClock clock{120.0f, 0.0f};

        clock.Advance(1.0f);

        EXPECT_NEAR(clock.GetBeat(), 2.0f, Tolerance);
    }

    TEST(BeatClockTests, BeatsAreCountedFromTheFirstBeatOfTheTrack) {
        BeatClock clock{120.0f, 0.25f};

        clock.Advance(0.75f);

        EXPECT_NEAR(clock.GetBeat(), 1.0f, Tolerance);
    }

    TEST(BeatClockTests, BeforeTheFirstBeatTheBeatIsNegative) {
        const BeatClock clock{120.0f, 0.25f};

        EXPECT_LT(clock.GetBeat(), 0.0f);
    }

    TEST(BeatClockTests, ABeatHappensAtTheFirstBeatPlusWholeBeats) {
        const BeatClock clock{120.0f, 0.25f};

        EXPECT_NEAR(clock.SecondsAtBeat(4.0f), 2.25f, Tolerance);
    }

    TEST(BeatClockTests, TheNextSixteenthIsTheClosestGridPointAhead) {
        BeatClock clock{120.0f, 0.0f};
        clock.Advance(0.2f);

        EXPECT_NEAR(clock.NextSubdivisionSeconds(4), 0.25f, Tolerance);
    }

    TEST(BeatClockTests, OnAGridPointTheNextSubdivisionIsNow) {
        BeatClock clock{120.0f, 0.0f};
        clock.Advance(0.5f);

        EXPECT_NEAR(clock.NextSubdivisionSeconds(4), 0.5f, Tolerance);
    }

    TEST(BeatClockTests, ThePhaseGoesFromZeroToOneWithinEachBeat) {
        BeatClock clock{60.0f, 0.0f};

        clock.Advance(3.25f);

        EXPECT_NEAR(clock.BeatPhase(), 0.25f, Tolerance);
    }

    TEST(BeatClockTests, ResettingGoesBackToTheStartOfTheTrack) {
        BeatClock clock{120.0f, 0.0f};
        clock.Advance(10.0f);

        clock.Reset();

        EXPECT_NEAR(clock.GetSeconds(), 0.0f, Tolerance);
    }
}
