#include "io.hpp"
#include "port.hpp"

#define IO_PORT_STATUS 0x1f0
#define FLT_MAX 3.402823e+38
#define FLT_MIN 1.175494e-38

u8* const IO::s_vgaText = reinterpret_cast<u8*>(0xb8000);
u16 IO::characterCursor = 0;
const char* const IO::s_digitBuffer = "0123456789ABCDEF";
bool IO::isShift = false, IO::isCapsLock = false;
u8 IO::colorMode = 0xf;

void IO::initScreen()
{
    // Disable cursor
    port::out(0x3D4, 0x0A);
    port::out(0x3D5, 0x20);
    
    // fill screen with color mode
    for(u16 i{0}; i < CHARACTER_COLUMNS * CHARACTER_ROWS; i += 2)
    {
        s_vgaText[i] = ' ';
        s_vgaText[i + 1] = colorMode;
    }

    characterCursor = 0;
}

void IO::initKeyboard()
{
    port::out(0x21, 0xfd);
}

void IO::putln()
{
    // Round up to the next multiple of CHARACTER_COLUMNS, which is the begging of the next row
    characterCursor = (characterCursor / CHARACTER_COLUMNS + 1) * CHARACTER_COLUMNS;
    if(characterCursor < CHARACTER_ROWS * CHARACTER_COLUMNS)
        return;
    
    // Scroll by copying all lines one row up
    for(u8 column = 0; column < CHARACTER_COLUMNS; ++column)
        for(u8 row = 1; row < CHARACTER_ROWS; ++row)
        {
            u8 byte = s_vgaText[column + row * CHARACTER_COLUMNS];
            s_vgaText[column + row * CHARACTER_COLUMNS] = s_vgaText[column + (row - 1) * CHARACTER_COLUMNS];
            s_vgaText[column + (row - 1) * CHARACTER_COLUMNS] = byte;
        }

    // Clear last row
    for(u8 column = 0; column < CHARACTER_COLUMNS; column += 2)
    {
        s_vgaText[column + (CHARACTER_ROWS - 1) * CHARACTER_COLUMNS] = ' ';
        s_vgaText[column + (CHARACTER_ROWS - 1) * CHARACTER_COLUMNS + 1] = 0x0f;
    }

    characterCursor -= CHARACTER_COLUMNS;
}

void IO::puts(const char* message)
{
    while(true)
    {
        auto byte = *(message++);
        if(!byte) return;
        else if(byte == '\n')
        {
            putln();
            continue;
        }
        s_vgaText[characterCursor + 0] = byte;
        s_vgaText[characterCursor + 1] = colorMode;
        characterCursor += 2;
    }
}

void IO::puts(const String& message)
{
    for(usize i = 0; i < message.getCount(); ++i)
    {
        auto byte = message[i];
        if(!byte) return;
        else if(byte == '\n')
        {
            putln();
            continue;
        }
        s_vgaText[characterCursor + 0] = byte;
        s_vgaText[characterCursor + 1] = colorMode;
        characterCursor += 2;
    }
}
void IO::putc(u8 character)
{
    if(character == '\n')
    {
        putln();
        return;
    }
    s_vgaText[characterCursor + 0] = character;
    s_vgaText[characterCursor + 1] = colorMode;
    characterCursor += 2;
}


void IO::putu(u32 number, u8 base)
{
    static u8 buffer[32];
    usize position = 0;
    if(number == 0)
    {
        s_vgaText[characterCursor + 0] = '0';
        s_vgaText[characterCursor + 1] = colorMode;
        characterCursor += 2;
        return;
    }
    do
    {
        buffer[position++] = s_digitBuffer[number % base];
        number /= base;
    } while (number != 0);

    while(--position > 0)
    {
        s_vgaText[characterCursor + 0] = buffer[position];
        s_vgaText[characterCursor + 1] = colorMode;
        characterCursor += 2;
    }

    s_vgaText[characterCursor + 0] = buffer[0];
    s_vgaText[characterCursor + 1] = colorMode;
    characterCursor += 2;
}

void IO::puti(i32 number, u8 base)
{
    static u8 buffer[32];
    usize position = 0;
    bool negative = false;
    if(number == 0)
    {
        s_vgaText[characterCursor + 0] = '0';
        s_vgaText[characterCursor + 1] = colorMode;
        characterCursor += 2;
        return;
    }
    else if(number < 0)
    {
        number = -number;
        negative = true;
    }

    do
    {
        buffer[position++] = s_digitBuffer[number % base];
        number /= base;
    } while (number != 0);
    if(negative)
        buffer[position++] = '-';

    while(--position != 0)
    {
        s_vgaText[characterCursor + 0] = buffer[position];
        s_vgaText[characterCursor + 1] = colorMode;
        characterCursor += 2;
    }

    s_vgaText[characterCursor + 0] = buffer[0];
    s_vgaText[characterCursor + 1] = colorMode;
    characterCursor += 2;
}

void IO::putf(f32 number, u8 precision)
{
    static char buffer[32];
    buffer[31] = '\0';
    precision = precision < 9? precision: 9; 
    static float powers_ten[] = { 1.0f, 10.0f, 100.0f, 1000.0f, 10'000.0f, 100'000.0f, 1'000'000.0f, 10'000'000.0f };

    if(number != number) IO::puts("nan");
    else if(number < -FLT_MAX) IO::puts("-inf");
    else if(number > FLT_MAX) IO::puts("+inf");
    else if(number > 1e9 || number < -1e9) IO::putu(static_cast<i32>(number));

    bool is_negative{false};
    if(number < 0.0f)
    {
        is_negative = true;
        number = 0.0f - number;
    }

    auto integral = static_cast<u32>(number);
    auto temporary = (number - static_cast<float>(integral)) * powers_ten[precision];
    auto fraction = static_cast<u32>(temporary);
    auto difference = temporary - static_cast<float>(fraction);
    usize position{31};

    if(difference >= 0.5f)
    {
        ++fraction;
        if(fraction >= powers_ten[precision])
        {
            fraction = 0.0;
            ++integral;
        }
    }

    if(precision == 0u)
    {
        difference = number - static_cast<float>(integral);
        if(difference >= 0.5) ++integral;
    }
    else
    {
        u8 count{precision};
        do
        {
            --count;
            buffer[--position] = '0' + static_cast<char>(fraction % 10u);
        } while(fraction /= 10u);

        while(position != 0ul && count-- != 0ul)
            buffer[--position] = '0';

        buffer[--position] = '.';
    }

    do
    {
        buffer[--position] = '0' + static_cast<char>(integral % 10u);
    } while(integral /= 10u);
    if(is_negative) buffer[--position] = '-';

    IO::puts(buffer + position);
}

void IO::putuln(u32 number, u8 base)
{
    putu(number, base);
    putln();
}

void IO::putiln(i32 number, u8 base)
{
    puti(number, base);
    putln();
}

void IO::putfln(f32 number, u8 precision)
{
    putf(number, precision);
    putln();
}

void IO::putsln(const char* message)
{
    puts(message);
    putln();
}

void IO::putsln(const String& message)
{
    puts(message);
    putln();
}

bool IO::isprint(char character)
{
    return character >= 32 && character <= 126;
}

u8 IO::readc()
{
    u8 scancode, normalized;
    do {
        while (!(port::in(0x64) & 0x1)) {}
        scancode = port::in(0x60);

        switch(scancode)
        {
            case 0x2a: case 0x36: isShift = true; break;
            case 0xaa: case 0xb6: isShift = false; break;
            case 0xba: isCapsLock = !isCapsLock; break;
        }
    } while(scancode & 0x80 || (normalized = normalizeKey(scancode)) == '\0');

    if(isprint(normalized))
        putc(normalized);
    return normalized;
}

void IO::reads(char* buffer, usize buffer_count)
{
    usize bytes_read = 0;
    u8 input;

    do
    {
        input = readc();
        if(input == '\b') // Backspace
        {
            if(bytes_read != 0)
            {
                --bytes_read;
                characterCursor -= 2;
                putc(' ');
                characterCursor -= 2;
            }
            continue;
        }
        buffer[bytes_read++] = input;
    } while(bytes_read < buffer_count - 1 && input != '\n');

    putln();
    buffer[bytes_read - 1] = '\0'; // Null termination
}

u8 IO::normalizeKey(u8 scancode)
{
    static const char* const key_map_regular = 
        "\0001234567890-=\b"
        "\tqwertyuiop[]\n"
        "\0asdfghjkl;'`"
        "\0\\zxcvbnm,./";
    
    static const char* const key_map_shifted = 
        "\0!@#$%^&*()_+\b"
        "\tQWERTYUIOP{}\n"
        "\0ASDFGHJKL:\"~"
        "\0|ZXCVBNM<>?";

        if(scancode <= 0x35)
        {
            auto result = isShift? key_map_shifted[scancode - 1]: key_map_regular[scancode - 1];
            if(!isCapsLock) [[likely]]
                return result;

            if(result >= 'a' && result <= 'z') return result - 'a' + 'A';
            else if(result >= 'A' && result <= 'Z') return result - 'A' + 'a';
            else return result;
        }
        else if(scancode == 0x39)
            return ' ';
        else return '\0';
}

void IO::wait()
{
    while(true)
    {
        u8 status = port::in(IO_PORT_STATUS);
        if((status & 0x1) == 0)
            break;
    }
}