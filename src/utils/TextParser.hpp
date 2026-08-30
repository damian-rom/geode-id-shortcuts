#pragma once

#include <string_view>
#include <vector>

namespace TextParser {
    inline constexpr int DEFAULT_MIN_DIGITS = 5;

    inline constexpr int MAX_DIGITS = 9;

    struct Shortcut {
        int start = 0;
        int end = 0;
        int id = 0;
    };

    std::vector<Shortcut> findShortcuts(
        std::string_view text,
        int minDigits = DEFAULT_MIN_DIGITS,
        std::string_view following = {}
    );
}
