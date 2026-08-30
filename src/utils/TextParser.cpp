#include "TextParser.hpp"

#include <Geode/utils/general.hpp>

#include <cctype>
#include <string>
#include <string_view>

namespace {
    constexpr std::string_view STAT_WORDS[] = {
        "att", "creator", "cp", "obj", "like", "download",
        "star", "moon", "demon", "dia", "coin", "level", "easy", "medium", "hard", "insane", "extreme",
        "day", "month", "mes", "year", 
        "fps",
        "peso", "dol", "eu", "rub",
        "spark", "ruby", // rated layouts stats
    };

    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    char toLower(char c) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    char charAt(std::string_view text, long long index) {
        if (index < 0 || index >= static_cast<long long>(text.size())) return '\0';
        return text[static_cast<size_t>(index)];
    }

    std::string firstWord(std::string_view source, size_t from) {
        while (from < source.size() && source[from] == ' ') from++;

        std::string word;
        for (size_t i = from; i < source.size() && source[i] != ' '; i++) {
            word.push_back(toLower(source[i]));
        }
        return word;
    }

    std::string wordAfter(std::string_view text, size_t pos, std::string_view following) {
        auto word = firstWord(text, pos);

        if (word.empty()) word = firstWord(following, 0);

        return word;
    }

    bool isStatWord(std::string_view word) {
        for (auto stat : STAT_WORDS) {
            if (word.starts_with(stat)) return true;
        }
        return false;
    }
}

std::vector<TextParser::Shortcut> TextParser::findShortcuts(
    std::string_view text, 
    int minDigits,
    std::string_view following
) {
    std::vector<Shortcut> shortcuts;

    if (minDigits < 1) minDigits = DEFAULT_MIN_DIGITS;

    size_t i = 0;
    while (i < text.size()) {
        if (!isDigit(text[i])) {
            i++;
            continue;
        }

        size_t start = i;
        while (i < text.size() && isDigit(text[i])) i++;
        size_t end = i;
        size_t length = end - start;

        if (static_cast<int>(length) < minDigits) continue;
        if (static_cast<int>(length) > MAX_DIGITS) continue;

        char before = charAt(text, static_cast<long long>(start) - 1);
        char after = charAt(text, static_cast<long long>(end));
        char beforeBefore = charAt(text, static_cast<long long>(start) - 2);
        char afterAfter = charAt(text, static_cast<long long>(end) + 1);

        if (before == '#' || before == '$') continue;
        if (after == '%') continue;
        if (before == '/' || after == '/') continue;
        if (before == ':' || after == ':') continue;
        if ((before == '.' || before == ',') && isDigit(beforeBefore)) continue;
        if ((after == '.' || after == ',') && isDigit(afterAfter)) continue;
        if (isStatWord(wordAfter(text, end, following))) continue;

        auto parsed = geode::utils::numFromString<int>(text.substr(start, length));
        if (parsed.isErr()) continue;

        int id = parsed.unwrap();
        if (id <= 0) continue;

        shortcuts.push_back(Shortcut {
            .start = static_cast<int>(start),
            .end = static_cast<int>(end),
            .id = id,
        });
    }

    return shortcuts;
}
