#pragma once
#ifndef RIFT_UTIL_HPP
#define RIFT_UTIL_HPP

#include <Geode/Result.hpp>
#include <charconv>
#include <string>
#include <string_view>
#include <utility>
#include <functional>

namespace rift::util {

    template <typename T>
    concept Number = std::is_integral_v<T> || std::is_floating_point_v<T>;

    /// @brief Read a number from a string.
    /// @note This code is taken from the
    /// <a href="https://github.com/geode-sdk/geode/blob/fd2a457e76a2d4ef6958ea83d0d5a006ac1e2dfc/loader/include/Geode/utils/general.hpp#L128">
    /// Geode SDK `numFromString` function.
    /// </a>
    template <Number Num>
    geode::Result<Num> readNumber(std::string_view const str) {
        if constexpr (std::is_floating_point_v<Num>
            #if defined(__cpp_lib_to_chars)
                && false
            #endif
        ) {
            Num val;
            char* strEnd;
            errno = 0;
            if (std::setlocale(LC_NUMERIC, "C")) {
                if constexpr (std::is_same_v<Num, float>) val = std::strtof(str.data(), &strEnd);
                else if constexpr (std::is_same_v<Num, double>) val = std::strtod(str.data(), &strEnd);
                else if constexpr (std::is_same_v<Num, long double>) val = std::strtold(str.data(), &strEnd);
                if (errno == ERANGE) return geode::Err("Number is too large to fit");
                if (strEnd == str.data()) return geode::Err("String is not a number");
                return geode::Ok(val);
            }
            return geode::Err("Failed to set locale");
        } else {
            Num result;
            std::from_chars_result res;
            if constexpr (std::is_floating_point_v<Num>) res = std::from_chars(str.data(), str.data() + str.size(), result);
            else res = std::from_chars(str.data(), str.data() + str.size(), result);

            auto [ptr, ec] = res;
            if (ec == std::errc()) return geode::Ok(result);
            if (ptr != str.data() + str.size()) return geode::Err("String contains trailing extra data");
            if (ec == std::errc::invalid_argument) return geode::Err("String is not a number");
            if (ec == std::errc::result_out_of_range) return geode::Err("Number is too large to fit");
            return geode::Err("Unknown error");
        }
    }

    template <typename T>
        constexpr bool isStringType() {
        using type = std::remove_cvref_t<T>;
        return std::is_same_v<type, std::string> ||
               std::is_same_v<type, std::string_view> ||
               std::is_same_v<type, char*> ||
               std::is_same_v<type, const char*>;
    }

}


#endif // RIFT_UTIL_HPP