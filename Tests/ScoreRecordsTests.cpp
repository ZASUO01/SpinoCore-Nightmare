#include <filesystem>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "Rules/ScoreRecords.h"

namespace {
    using Nightmare::ScoreRecord;
    using Nightmare::ScoreStorage;

    TEST(ScoreRecordsTests, RecordsWrittenCanBeReadBack) {
        const std::vector<ScoreRecord> records{{.name = "Ana", .score = 40}, {.name = "Bia", .score = 12}};

        const auto parsed = Nightmare::ParseScoreRecords(Nightmare::SerializeScoreRecords(records));

        ASSERT_EQ(parsed.size(), 2u);
        EXPECT_EQ(parsed[0].name, "Ana");
        EXPECT_EQ(parsed[0].score, 40);
        EXPECT_EQ(parsed[1].name, "Bia");
    }

    TEST(ScoreRecordsTests, ReadRecordsAreSortedFromTheHighestScore) {
        const auto parsed = Nightmare::ParseScoreRecords("5\tLow\n90\tHigh\n30\tMid\n");

        ASSERT_EQ(parsed.size(), 3u);
        EXPECT_EQ(parsed[0].name, "High");
        EXPECT_EQ(parsed[2].name, "Low");
    }

    TEST(ScoreRecordsTests, BrokenLinesAreSkipped) {
        const auto parsed = Nightmare::ParseScoreRecords("abc\tName\n12\n-3\tNeg\n7\tGood\n\t\n");

        ASSERT_EQ(parsed.size(), 1u);
        EXPECT_EQ(parsed[0].name, "Good");
    }

    TEST(ScoreRecordsTests, ANewRecordTakesItsPlaceByScore) {
        std::vector<ScoreRecord> records{{.name = "A", .score = 50}, {.name = "C", .score = 10}};

        Nightmare::InsertScoreRecord(records, ScoreRecord{.name = "B", .score = 30});

        ASSERT_EQ(records.size(), 3u);
        EXPECT_EQ(records[1].name, "B");
    }

    TEST(ScoreRecordsTests, OnlyTheBestTenRecordsAreKept) {
        std::vector<ScoreRecord> records;
        for (int score = 1; score <= 10; ++score) Nightmare::InsertScoreRecord(records, ScoreRecord{.name = "P", .score = score * 10});

        Nightmare::InsertScoreRecord(records, ScoreRecord{.name = "Worst", .score = 1});

        ASSERT_EQ(records.size(), 10u);
        EXPECT_EQ(records.back().score, 10);
    }

    TEST(ScoreRecordsTests, ATieKeepsTheOlderRecordFirst) {
        std::vector<ScoreRecord> records{{.name = "Old", .score = 20}};

        Nightmare::InsertScoreRecord(records, ScoreRecord{.name = "New", .score = 20});

        EXPECT_EQ(records[0].name, "Old");
    }

    TEST(ScoreRecordsTests, NamesAreTrimmedAndCannotBreakTheFileFormat) {
        EXPECT_EQ(Nightmare::SanitizeName("  Jo\tão\n "), "Jo ão");
    }

    TEST(ScoreRecordsTests, LongNamesAreCutWithoutSplittingACharacter) {
        const std::string name = Nightmare::SanitizeName("ááááááááááááááááááááá");

        EXPECT_EQ(name, "áááááááááááááááá");
    }

    TEST(ScoreRecordsTests, AccentedLettersCountAsOneCharacterEach) {
        EXPECT_EQ(Nightmare::CodePointCount("João"), 4u);
    }

    TEST(ScoreRecordsTests, ABlankNameIsNotRecorded) {
        std::vector<ScoreRecord> records;

        Nightmare::InsertScoreRecord(records, ScoreRecord{.name = "   ", .score = 99});

        EXPECT_TRUE(records.empty());
    }

    class ScoreStorageTests : public testing::Test {
    protected:
        void SetUp() override {
            const auto* info = testing::UnitTest::GetInstance()->current_test_info();
            directory = std::filesystem::temp_directory_path() / "nightmare-score-tests" / info->name();
            std::filesystem::remove_all(directory);
        }

        void TearDown() override {
            std::filesystem::remove_all(directory);
        }

        std::filesystem::path directory;
    };

    TEST_F(ScoreStorageTests, AMissingFileHasNoRecords) {
        const ScoreStorage storage{directory / "scores.txt"};

        EXPECT_TRUE(storage.Load().empty());
    }

    TEST_F(ScoreStorageTests, AddedRecordsAreKeptOnDisk) {
        const ScoreStorage storage{directory / "nested" / "scores.txt"};
        ASSERT_TRUE(storage.Add(ScoreRecord{.name = "Ana", .score = 10}));
        ASSERT_TRUE(storage.Add(ScoreRecord{.name = "Bia", .score = 25}));

        const auto records = ScoreStorage{directory / "nested" / "scores.txt"}.Load();

        ASSERT_EQ(records.size(), 2u);
        EXPECT_EQ(records[0].name, "Bia");
        EXPECT_EQ(records[1].name, "Ana");
    }
}
