#include <gtest/gtest.h>
#include "Rules/ScoreBoard.h"

namespace {
    using Nightmare::ScoreBoard;

    TEST(ScoreBoardTests, ANewRunStartsWithFullLifeAndNoPoints) {
        const ScoreBoard board;

        EXPECT_EQ(board.GetLife(), 10);
        EXPECT_EQ(board.GetScore(), 0);
        EXPECT_EQ(board.GetMultiplier(), 1);
    }

    TEST(ScoreBoardTests, EachHitBeforeTheFirstMultiplierScoresOnePoint) {
        ScoreBoard board;

        for (int hit = 0; hit < 10; ++hit) board.RegisterHit();

        EXPECT_EQ(board.GetScore(), 10);
    }

    TEST(ScoreBoardTests, TenHitsInARowDoubleThePointsOfTheNextHit) {
        ScoreBoard board;
        for (int hit = 0; hit < 10; ++hit) board.RegisterHit();

        board.RegisterHit();

        EXPECT_EQ(board.GetMultiplier(), 2);
        EXPECT_EQ(board.GetScore(), 12);
    }

    TEST(ScoreBoardTests, TheMultiplierStopsGrowingAtFour) {
        ScoreBoard board;

        for (int hit = 0; hit < 80; ++hit) board.RegisterHit();

        EXPECT_EQ(board.GetMultiplier(), 4);
    }

    TEST(ScoreBoardTests, AMissBreaksTheComboAndTheMultiplier) {
        ScoreBoard board;
        for (int hit = 0; hit < 25; ++hit) board.RegisterHit();

        board.RegisterMiss();

        EXPECT_EQ(board.GetCombo(), 0);
        EXPECT_EQ(board.GetMultiplier(), 1);
    }

    TEST(ScoreBoardTests, AMissKeepsThePointsAlreadyEarned) {
        ScoreBoard board;
        for (int hit = 0; hit < 3; ++hit) board.RegisterHit();

        board.RegisterMiss();

        EXPECT_EQ(board.GetScore(), 3);
    }

    TEST(ScoreBoardTests, AMissCostsOneLife) {
        ScoreBoard board;

        board.RegisterMiss();

        EXPECT_EQ(board.GetLife(), 9);
    }

    TEST(ScoreBoardTests, AnObstacleCostsTwoLives) {
        ScoreBoard board;

        board.RegisterObstacle();

        EXPECT_EQ(board.GetLife(), 8);
    }

    TEST(ScoreBoardTests, AnObstacleDoesNotBreakTheCombo) {
        ScoreBoard board;
        for (int hit = 0; hit < 12; ++hit) board.RegisterHit();

        board.RegisterObstacle();

        EXPECT_EQ(board.GetCombo(), 12);
        EXPECT_EQ(board.GetMultiplier(), 2);
    }

    TEST(ScoreBoardTests, FiveObstaclesEndTheRun) {
        ScoreBoard board;

        for (int obstacle = 0; obstacle < 5; ++obstacle) board.RegisterObstacle();

        EXPECT_TRUE(board.IsDead());
        EXPECT_EQ(board.GetLife(), 0);
    }

    TEST(ScoreBoardTests, LifeNeverGoesBelowZero) {
        ScoreBoard board;
        for (int miss = 0; miss < 9; ++miss) board.RegisterMiss();

        board.RegisterObstacle();

        EXPECT_EQ(board.GetLife(), 0);
    }

    TEST(ScoreBoardTests, HitsAfterDeathScoreNothing) {
        ScoreBoard board;
        for (int obstacle = 0; obstacle < 5; ++obstacle) board.RegisterObstacle();

        board.RegisterHit();

        EXPECT_EQ(board.GetScore(), 0);
    }

    TEST(ScoreBoardTests, TheBestComboSurvivesAMiss) {
        ScoreBoard board;
        for (int hit = 0; hit < 7; ++hit) board.RegisterHit();
        board.RegisterMiss();

        board.RegisterHit();

        EXPECT_EQ(board.GetBestCombo(), 7);
        EXPECT_EQ(board.GetCombo(), 1);
    }
}
