#pragma once

// clang-format on
namespace BKits {
namespace Version {

#define BKITS_MAJOR 1
#define BKITS_MINOR 0
#define BKITS_PATCH 0

// 辅助宏: 将数字转化为字符串
#define BKITS_STR_HELPER(x) #x
#define BKITS_STR(x) BKITS_STR_HELPER(x)

static constexpr const char *VERSION_MAJOR = BKITS_STR(BKITS_MAJOR);
static constexpr const char *VERSION_MINOR = BKITS_STR(BKITS_MINOR);
static constexpr const char *VERSION_PATCH = BKITS_STR(BKITS_PATCH);

// 自动拼接: C++ 编译器会自动合并
static constexpr const char *VERSION = BKITS_STR(BKITS_MAJOR) "." BKITS_STR(
    BKITS_MINOR) "." BKITS_STR(BKITS_PATCH);

} // namespace Version
} // namespace BKits
// clang-format on
