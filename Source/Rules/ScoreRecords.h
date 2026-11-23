#pragma once
#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace Nightmare {
    struct ScoreRecord final {
        std::string name;
        int score{0};
    };

    inline constexpr std::size_t MaxScoreRecords = 10;
    inline constexpr std::size_t MaxNameLength = 16;

    [[nodiscard]] std::size_t CodePointCount(std::string_view text);
    [[nodiscard]] std::string SanitizeName(std::string_view name);
    [[nodiscard]] std::vector<ScoreRecord> ParseScoreRecords(std::string_view text);
    [[nodiscard]] std::string SerializeScoreRecords(const std::vector<ScoreRecord>& records);
    void InsertScoreRecord(std::vector<ScoreRecord>& records, ScoreRecord record, std::size_t limit = MaxScoreRecords);

    class ScoreStorage final {
    public:
        explicit ScoreStorage(std::filesystem::path file);

        [[nodiscard]] std::vector<ScoreRecord> Load() const;
        bool Save(const std::vector<ScoreRecord>& records) const;
        bool Add(ScoreRecord record) const;

    private:
        std::filesystem::path mFile;
    };
}
