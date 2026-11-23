#pragma once

namespace Nightmare {
    class LaneSelector final {
    public:
        LaneSelector(int laneCount, int startLane);

        bool MoveLeft();
        bool MoveRight();

        [[nodiscard]] int GetLane() const { return mLane; }
        [[nodiscard]] int GetLaneCount() const { return mLaneCount; }

    private:
        int mLaneCount;
        int mLane;
    };
}
