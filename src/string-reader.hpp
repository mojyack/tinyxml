#pragma once
#include "macros/unwrap.hpp"

struct StringReader {
    size_t           cursor;
    std::string_view str;

    template <class T, class... Args>
    static auto contains(const T value, const Args... args) -> bool {
        return ((value == args) || ...);
    }

    auto peek() const -> std::optional<char> {
        ensure(cursor < str.size());
        return str[cursor];
    }

    auto read() -> std::optional<char> {
        unwrap(c, peek());
        cursor += 1;
        return c;
    }

    auto is_eof() const -> bool {
        return cursor >= str.size();
    }

    template <class... Args>
    auto skip_while(const Args... args) -> void {
        while(contains(peek(), args...)) {
            cursor += 1;
        }
    }

    template <class... Args>
    auto read_until(const Args... args) -> std::optional<std::string_view> {
        auto begin = cursor;
    loop:
        unwrap(c, read());
        if(contains(c, args...)) {
            cursor -= 1;
            return str.substr(begin, cursor - begin);
        }
        goto loop;
    }
};
