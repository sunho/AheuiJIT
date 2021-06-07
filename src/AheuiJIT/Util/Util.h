#pragma once

#include <codecvt>
#include <iostream>
#include <locale>

namespace aheuijit {

static std::string covnert_utf16_to_utf8(const std::u16string &str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
    return conv.to_bytes(str);
}

static std::string covnert_utf16_to_utf8(const char16_t &ch) {
    std::u16string str;
    str.push_back(ch);
    return covnert_utf16_to_utf8(std::u16string(str));
}

static std::u16string covnert_utf8_to_utf16(const std::string &str) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
    return conv.from_bytes(str.c_str());
}

#define ASSERT(pred)                                                                               \
    if (!(pred)) {                                                                                 \
        std::cout << "Assertion failed: " << #pred;                                                \
        std::terminate();                                                                          \
    }

#ifdef AHEUI_JIT_LOG
#define LOG(format, ...) fmt::print(format + std::string("\n"), ##__VA_ARGS__);
#else
#define LOG(...)
#endif

}  // namespace aheuijit
