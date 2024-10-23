#pragma once

#include <string>
#include <utility>
#include <vector>

namespace Input {

#define PAIR_99(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_100(N, __VA_ARGS__))
#define PAIR_98(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_99(N, __VA_ARGS__))
#define PAIR_97(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_98(N, __VA_ARGS__))
#define PAIR_96(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_97(N, __VA_ARGS__))
#define PAIR_95(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_96(N, __VA_ARGS__))
#define PAIR_94(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_95(N, __VA_ARGS__))
#define PAIR_93(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_94(N, __VA_ARGS__))
#define PAIR_92(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_93(N, __VA_ARGS__))
#define PAIR_91(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_92(N, __VA_ARGS__))
#define PAIR_90(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_91(N, __VA_ARGS__))
#define PAIR_89(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_90(N, __VA_ARGS__))
#define PAIR_88(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_89(N, __VA_ARGS__))
#define PAIR_87(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_88(N, __VA_ARGS__))
#define PAIR_86(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_87(N, __VA_ARGS__))
#define PAIR_85(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_86(N, __VA_ARGS__))
#define PAIR_84(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_85(N, __VA_ARGS__))
#define PAIR_83(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_84(N, __VA_ARGS__))
#define PAIR_82(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_83(N, __VA_ARGS__))
#define PAIR_81(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_82(N, __VA_ARGS__))
#define PAIR_80(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_81(N, __VA_ARGS__))
#define PAIR_79(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_80(N, __VA_ARGS__))
#define PAIR_78(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_79(N, __VA_ARGS__))
#define PAIR_77(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_78(N, __VA_ARGS__))
#define PAIR_76(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_77(N, __VA_ARGS__))
#define PAIR_75(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_76(N, __VA_ARGS__))
#define PAIR_74(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_75(N, __VA_ARGS__))
#define PAIR_73(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_74(N, __VA_ARGS__))
#define PAIR_72(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_73(N, __VA_ARGS__))
#define PAIR_71(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_72(N, __VA_ARGS__))
#define PAIR_70(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_71(N, __VA_ARGS__))
#define PAIR_69(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_70(N, __VA_ARGS__))
#define PAIR_68(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_69(N, __VA_ARGS__))
#define PAIR_67(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_68(N, __VA_ARGS__))
#define PAIR_66(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_67(N, __VA_ARGS__))
#define PAIR_65(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_66(N, __VA_ARGS__))
#define PAIR_64(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_65(N, __VA_ARGS__))
#define PAIR_63(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_64(N, __VA_ARGS__))
#define PAIR_62(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_63(N, __VA_ARGS__))
#define PAIR_61(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_62(N, __VA_ARGS__))
#define PAIR_60(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_61(N, __VA_ARGS__))
#define PAIR_59(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_60(N, __VA_ARGS__))
#define PAIR_58(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_59(N, __VA_ARGS__))
#define PAIR_57(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_58(N, __VA_ARGS__))
#define PAIR_56(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_57(N, __VA_ARGS__))
#define PAIR_55(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_56(N, __VA_ARGS__))
#define PAIR_54(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_55(N, __VA_ARGS__))
#define PAIR_53(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_54(N, __VA_ARGS__))
#define PAIR_52(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_53(N, __VA_ARGS__))
#define PAIR_51(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_52(N, __VA_ARGS__))
#define PAIR_50(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_51(N, __VA_ARGS__))
#define PAIR_49(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_50(N, __VA_ARGS__))
#define PAIR_48(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_49(N, __VA_ARGS__))
#define PAIR_47(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_48(N, __VA_ARGS__))
#define PAIR_46(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_47(N, __VA_ARGS__))
#define PAIR_45(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_46(N, __VA_ARGS__))
#define PAIR_44(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_45(N, __VA_ARGS__))
#define PAIR_43(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_44(N, __VA_ARGS__))
#define PAIR_42(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_43(N, __VA_ARGS__))
#define PAIR_41(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_42(N, __VA_ARGS__))
#define PAIR_40(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_41(N, __VA_ARGS__))
#define PAIR_39(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_40(N, __VA_ARGS__))
#define PAIR_38(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_39(N, __VA_ARGS__))
#define PAIR_37(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_38(N, __VA_ARGS__))
#define PAIR_36(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_37(N, __VA_ARGS__))
#define PAIR_35(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_36(N, __VA_ARGS__))
#define PAIR_34(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_35(N, __VA_ARGS__))
#define PAIR_33(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_34(N, __VA_ARGS__))
#define PAIR_32(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_33(N, __VA_ARGS__))
#define PAIR_31(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_32(N, __VA_ARGS__))
#define PAIR_30(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_31(N, __VA_ARGS__))
#define PAIR_29(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_30(N, __VA_ARGS__))
#define PAIR_28(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_29(N, __VA_ARGS__))
#define PAIR_27(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_28(N, __VA_ARGS__))
#define PAIR_26(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_27(N, __VA_ARGS__))
#define PAIR_25(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_26(N, __VA_ARGS__))
#define PAIR_24(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_25(N, __VA_ARGS__))
#define PAIR_23(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_24(N, __VA_ARGS__))
#define PAIR_22(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_23(N, __VA_ARGS__))
#define PAIR_21(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_22(N, __VA_ARGS__))
#define PAIR_20(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_21(N, __VA_ARGS__))
#define PAIR_19(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_20(N, __VA_ARGS__))
#define PAIR_18(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_19(N, __VA_ARGS__))
#define PAIR_17(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_18(N, __VA_ARGS__))
#define PAIR_16(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_17(N, __VA_ARGS__))
#define PAIR_15(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_16(N, __VA_ARGS__))
#define PAIR_14(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_15(N, __VA_ARGS__))
#define PAIR_13(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_14(N, __VA_ARGS__))
#define PAIR_12(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_13(N, __VA_ARGS__))
#define PAIR_11(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_12(N, __VA_ARGS__))
#define PAIR_10(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_11(N, __VA_ARGS__))
#define PAIR_09(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_10(N, __VA_ARGS__))
#define PAIR_08(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_09(N, __VA_ARGS__))
#define PAIR_07(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_08(N, __VA_ARGS__))
#define PAIR_06(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_07(N, __VA_ARGS__))
#define PAIR_05(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_06(N, __VA_ARGS__))
#define PAIR_04(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_05(N, __VA_ARGS__))
#define PAIR_03(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_04(N, __VA_ARGS__))
#define PAIR_02(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_03(N, __VA_ARGS__))
#define PAIR_01(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_02(N, __VA_ARGS__))
#define PAIR(N, X, ...) {#X, N::X} __VA_OPT__(, PAIR_01(N, __VA_ARGS__))

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

ENUM(Button, None, Invalid,
     // Mouse
     Left, Right, Middle, Side, Extra, Forward, Back, ScrollUp, ScrollDown,
     // Keyboard
     Esc, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, PrintScreen, ScrollLock, Pause, Insert,
     Home, PageUp, PageDown, Delete, End, KeyUp, KeyDown, KeyLeft, KeyRight, Tilde, One, Two, Three,
     Four, Five, Six, Seven, Eight, Nine, Zero, Minus, Equal, BackSpace, Tab, Q, W, E, R, T, Y, U,
     I, O, P, LeftBrace, RightBracket, Backslash, CapsLock, A, S, D, F, G, H, J, K, L, SemiColon,
     Apostrophe, Enter, LeftShift, Z, X, C, V, B, N, M, Comma, Dot, Slash, RightShift, LeftControl,
     LeftMeta, LeftAlt, Space, RightAlt, RightMeta, Compose, RightControl);

} // namespace Input