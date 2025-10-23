#pragma once
#include "types.hpp"
#include "string.hpp"
#define CHARACTER_ROWS 25
#define CHARACTER_COLUMNS 160

class IO final
{
public:
    IO() = delete;
    static void initScreen();
    static void initKeyboard();
    static void putln();
    static void puts(const char* message);
    static void puts(const String& message);
    static void putc(u8 character);
    static void putu(u32 number, u8 base = 10);
    static void puti(i32 number, u8 base = 10);
    static void putf(f32 number, u8 precision = 6);
    static void putuln(u32 number, u8 base = 10);
    static void putiln(i32 number, u8 base = 10);
    static void putfln(f32 number, u8 precision = 6);
    static void putsln(const char* message);
    static void putsln(const String& message);
    static bool isprint(char character);
    static u8 readc();
    static void reads(char* buffer, usize buffer_count);
    static void wait();
    static u16 characterCursor;
    static bool isShift, isCapsLock;
    static u8 colorMode;
private:
    static u8 normalizeKey(u8 key);
    static u8* const s_vgaText;
    static const char* const s_digitBuffer;
};