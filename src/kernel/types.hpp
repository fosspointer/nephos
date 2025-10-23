#pragma once
using u64 = unsigned long long;
using u32 = unsigned int;
using u16 = unsigned short;
using u8 = unsigned char;

using i64 = signed long long;
using i32 = signed int;
using i16 = signed short;
using i8 = signed char;

using usize = unsigned long;
using isize = signed long;

using f32 = float;
using f64 = double;

static_assert(sizeof(u64) == 8);
static_assert(sizeof(i64) == 8);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(u8) == 1);
static_assert(sizeof(i8) == 1);