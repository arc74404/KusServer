#ifndef DEFINES_H
#define DEFINES_H

#include <cstdint>

#define KB(x) ((uint64_t)1024 * x)
#define MB(x) ((uint64_t)1024 * KB(x))
#define GB(x) ((uint64_t)1024 * MB(x))

#define ArraySize(arr) sizeof((arr)) / sizeof((arr[0]))

#define INVALID_IDX UINT32_MAX

#define internal static

#endif // DEFINES_H