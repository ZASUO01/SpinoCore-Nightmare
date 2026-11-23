#include "ScoreBoard.h"
#include <algorithm>

namespace Nightmare {
    void ScoreBoard::RegisterHit() {
        if (IsDead()) return;

        mScore += mMultiplier;
        ++mCombo;
        ++mHits;
        mBestCombo = std::max(mBestCombo, mCombo);
        mMultiplier = std::min(MaxMultiplier, 1 + mCombo / HitsPerMultiplier);
    }

    void ScoreBoard::RegisterMiss() {
        if (IsDead()) return;

        ++mMisses;
        mCombo = 0;
        mMultiplier = 1;
        TakeDamage(MissDamage);
    }

    void ScoreBoard::RegisterObstacle() {
        if (IsDead()) return;

        TakeDamage(ObstacleDamage);
    }

    void ScoreBoard::TakeDamage(const int damage) {
        mLife = std::max(0, mLife - damage);
    }
}
