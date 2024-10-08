#pragma once

#include <string>
#include <utility>
#include <vector>

namespace Input {

#define PAIR_8(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_9(N, __VA_ARGS__))
#define PAIR_7(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_8(N, __VA_ARGS__))
#define PAIR_6(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_7(N, __VA_ARGS__))
#define PAIR_5(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_6(N, __VA_ARGS__))
#define PAIR_4(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_5(N, __VA_ARGS__))
#define PAIR_3(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_4(N, __VA_ARGS__))
#define PAIR_2(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_3(N, __VA_ARGS__))
#define PAIR_1(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_2(N, __VA_ARGS__))
#define PAIR(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_1(N, __VA_ARGS__))

#define ENUM(NAME, ...)                                                                            \
    enum class NAME : u_int32_t { __VA_ARGS__ };                                                   \
    template <>                                                                                    \
    inline std::vector<std::pair<std::string, NAME>> EnumMetadata<NAME>::Canonicalizations() {     \
        return {PAIR(NAME, __VA_ARGS__)};                                                          \
    }                                                                                              \
    template <> inline u_int32_t EnumMetadata<NAME>::Index() {                                     \
        return __COUNTER__;                                                                        \
    }

template <typename T> struct EnumMetadata {
    static std::vector<std::pair<std::string, T>> Canonicalizations();
    static u_int32_t Index();
};

template <typename Type> inline std::string CanonicalizeEnum(Type id) {
    const auto group = EnumMetadata<Type>::Canonicalizations();
    for (auto &[name, value] : group) {
        if (value == id) {
            return name;
        }
    }
    return "unknown";
}

template <typename Type> inline Type ToEnum(const std::string &canonicalization) {
    const auto group = EnumMetadata<Type>::Canonicalizations();
    for (auto &[name, value] : group) {
        if (name == canonicalization) {
            return value;
        }
    }
    return {};
}

ENUM(Button, None, Left, Right, Middle, Side, Extra, Forward, Back, Invalid);

} // namespace Input