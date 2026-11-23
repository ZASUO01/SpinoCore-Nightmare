#include "LaneSelector.h"
#include <algorithm>

namespace Nightmare {
    LaneSelector::LaneSelector(const int laneCount, const int startLane)
        : mLaneCount(std::max(1, laneCount))
        , mLane(std::clamp(startLane, 0, mLaneCount - 1)) {}

    bool LaneSelector::MoveLeft() {
        if (mLane == 0) return false;
        --mLane;
        return true;
    }

    bool LaneSelector::MoveRight() {
        if (mLane == mLaneCount - 1) return false;
        ++mLane;
        return true;
    }
}
