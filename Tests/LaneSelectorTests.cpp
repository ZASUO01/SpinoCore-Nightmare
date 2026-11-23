#include <gtest/gtest.h>
#include "Rules/LaneSelector.h"

namespace {
    using Nightmare::LaneSelector;

    TEST(LaneSelectorTests, MovingRightGoesToTheNextLane) {
        LaneSelector lanes{4, 1};

        const bool moved = lanes.MoveRight();

        EXPECT_TRUE(moved);
        EXPECT_EQ(lanes.GetLane(), 2);
    }

    TEST(LaneSelectorTests, MovingLeftGoesToThePreviousLane) {
        LaneSelector lanes{4, 2};

        const bool moved = lanes.MoveLeft();

        EXPECT_TRUE(moved);
        EXPECT_EQ(lanes.GetLane(), 1);
    }

    TEST(LaneSelectorTests, TheRunnerCannotLeaveTheRoadOnTheLeft) {
        LaneSelector lanes{4, 0};

        const bool moved = lanes.MoveLeft();

        EXPECT_FALSE(moved);
        EXPECT_EQ(lanes.GetLane(), 0);
    }

    TEST(LaneSelectorTests, TheRunnerCannotLeaveTheRoadOnTheRight) {
        LaneSelector lanes{4, 3};

        const bool moved = lanes.MoveRight();

        EXPECT_FALSE(moved);
        EXPECT_EQ(lanes.GetLane(), 3);
    }

    TEST(LaneSelectorTests, AStartLaneOutsideTheRoadIsMovedOntoIt) {
        const LaneSelector lanes{4, 9};

        EXPECT_EQ(lanes.GetLane(), 3);
    }
}
