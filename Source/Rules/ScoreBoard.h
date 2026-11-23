#pragma once

namespace Nightmare {
    class ScoreBoard final {
    public:
        static constexpr int MaxLife = 10;
        static constexpr int ObstacleDamage = 2;
        static constexpr int MissDamage = 1;
        static constexpr int HitsPerMultiplier = 10;
        static constexpr int MaxMultiplier = 4;

        void RegisterHit();
        void RegisterMiss();
        void RegisterObstacle();

        [[nodiscard]] int GetLife() const { return mLife; }
        [[nodiscard]] int GetScore() const { return mScore; }
        [[nodiscard]] int GetCombo() const { return mCombo; }
        [[nodiscard]] int GetBestCombo() const { return mBestCombo; }
        [[nodiscard]] int GetMultiplier() const { return mMultiplier; }
        [[nodiscard]] int GetHits() const { return mHits; }
        [[nodiscard]] int GetMisses() const { return mMisses; }
        [[nodiscard]] bool IsDead() const { return mLife <= 0; }

    private:
        void TakeDamage(int damage);

        int mLife{MaxLife};
        int mScore{0};
        int mCombo{0};
        int mBestCombo{0};
        int mMultiplier{1};
        int mHits{0};
        int mMisses{0};
    };
}
