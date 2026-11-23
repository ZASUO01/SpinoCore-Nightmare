#include "ScoreRecords.h"
#include <algorithm>
#include <charconv>
#include <fstream>
#include <iterator>
#include <system_error>
#include <utility>

namespace Nightmare {
    namespace {
        constexpr char Separator = '\t';

        [[nodiscard]] bool IsBlank(const char character) {
            return character == ' ' || character == '\t' || character == '\r' || character == '\n';
        }

        [[nodiscard]] std::string_view Trim(std::string_view text) {
            while (!text.empty() && IsBlank(text.front())) text.remove_prefix(1);
            while (!text.empty() && IsBlank(text.back())) text.remove_suffix(1);
            return text;
        }

        [[nodiscard]] std::size_t ByteLengthOf(const std::string_view text, const std::size_t codePoints) {
            std::size_t count = 0;
            for (std::size_t index = 0; index < text.size(); ++index) {
                if ((static_cast<unsigned char>(text[index]) & 0xC0u) == 0x80u) continue;
                if (count == codePoints) return index;
                ++count;
            }
            return text.size();
        }
    }

    std::size_t CodePointCount(const std::string_view text) {
        std::size_t count = 0;
        for (const char character : text) {
            if ((static_cast<unsigned char>(character) & 0xC0u) != 0x80u) ++count;
        }
        return count;
    }

    std::string SanitizeName(const std::string_view name) {
        std::string clean;
        for (const char character : Trim(name)) {
            clean.push_back(character == '\t' || character == '\n' || character == '\r' ? ' ' : character);
        }
        clean.resize(ByteLengthOf(clean, MaxNameLength));
        return std::string(Trim(clean));
    }

    std::vector<ScoreRecord> ParseScoreRecords(const std::string_view text) {
        std::vector<ScoreRecord> records;
        std::size_t start = 0;
        while (start < text.size()) {
            std::size_t end = text.find('\n', start);
            if (end == std::string_view::npos) end = text.size();
            const std::string_view line = Trim(text.substr(start, end - start));
            start = end + 1;

            const std::size_t separator = line.find(Separator);
            if (separator == std::string_view::npos) continue;

            const std::string_view number = line.substr(0, separator);
            int score = 0;
            const auto [last, error] = std::from_chars(number.data(), number.data() + number.size(), score);
            if (error != std::errc{} || last != number.data() + number.size() || score < 0) continue;

            std::string name = SanitizeName(line.substr(separator + 1));
            if (name.empty()) continue;
            records.push_back(ScoreRecord{.name = std::move(name), .score = score});
        }

        std::ranges::stable_sort(records, std::ranges::greater{}, &ScoreRecord::score);
        if (records.size() > MaxScoreRecords) records.resize(MaxScoreRecords);
        return records;
    }

    std::string SerializeScoreRecords(const std::vector<ScoreRecord>& records) {
        std::string text;
        for (const ScoreRecord& record : records) {
            text += std::to_string(record.score);
            text += Separator;
            text += SanitizeName(record.name);
            text += '\n';
        }
        return text;
    }

    void InsertScoreRecord(std::vector<ScoreRecord>& records, ScoreRecord record, const std::size_t limit) {
        record.name = SanitizeName(record.name);
        if (record.name.empty() || limit == 0) return;

        const auto position = std::ranges::upper_bound(records, record.score, std::ranges::greater{}, &ScoreRecord::score);
        records.insert(position, std::move(record));
        if (records.size() > limit) records.resize(limit);
    }

    ScoreStorage::ScoreStorage(std::filesystem::path file) : mFile(std::move(file)) {}

    std::vector<ScoreRecord> ScoreStorage::Load() const {
        std::ifstream input(mFile, std::ios::binary);
        if (!input) return {};

        const std::string text{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
        return ParseScoreRecords(text);
    }

    bool ScoreStorage::Save(const std::vector<ScoreRecord>& records) const {
        std::error_code error;
        if (mFile.has_parent_path()) std::filesystem::create_directories(mFile.parent_path(), error);

        std::ofstream output(mFile, std::ios::binary | std::ios::trunc);
        if (!output) return false;
        output << SerializeScoreRecords(records);
        return static_cast<bool>(output);
    }

    bool ScoreStorage::Add(ScoreRecord record) const {
        std::vector<ScoreRecord> records = Load();
        InsertScoreRecord(records, std::move(record));
        return Save(records);
    }
}
