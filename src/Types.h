#pragma once

#include <cstdint>
#include <random>

/**
* Integer typedefs
*/

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/**
* RNG typedefs
*/

typedef std::mt19937 Rng;
typedef std::mt19937::result_type RngInt;