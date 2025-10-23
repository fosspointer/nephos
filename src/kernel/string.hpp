#pragma once
#include "vector.hpp"

bool stringEquals(const char* first, const char* second);
bool substringEquals(const char* first, const char* second);
usize stringLength(const char* str);
char charToLower(char character);
char charToUpper(char character);

class String : public Vector<char>
{
public:
    String()
        :Vector<char>()
    {}

    String(const char* cString) noexcept
    {
        m_capacity = m_count = stringLength(cString);
        reallocate();
        for(usize i = 0; i < m_count; ++i)
            m_data[i] = cString[i];
    }

    i32 parseI32()
    {
        i32 result = 0;
        bool negative = false;
        usize index = 0;
        if(m_data[index] == '+') ++index;
        else if(m_data[index] == '-') { ++index; negative = true; }
        for(;index < m_count; ++index)
        {
            if(m_data[index] < '0' || m_data[index] > '9') { assert(false, "Cannot parse non-numeric string as i32"); }
            result *= 10;
            result += m_data[index] - '0';
        }
        return negative? -result: result;
    }

    u32 parseU32()
    {
        u32 result = 0;
        for(usize index = 0; index < m_count; ++index)
        {
            if(m_data[index] < '0' || m_data[index] > '9') { assert(false, "Cannot parse non-numeric string as u32"); }
            result *= 10;
            result += m_data[index] - '0';
        }
        return result;
    }

    static String fromI32(i32 number)
    {
        static char buffer[12];
        usize position = 12;
        buffer[--position] = '\0';
        bool negative = false;

        if(number == 0) return String("0");
        else if(number < 0)
        {
            number = -number;
            negative = true;
        }

        do
        {
            buffer[--position] = '0' + number % 10;
            number /= 10;
        } while (number != 0);
        if(negative)
            buffer[--position] = '-';
        return buffer + position;
    }

    static String fromU32(u32 number)
    {
        static char buffer[11];
        usize position = 11;
        buffer[--position] = '\0';

        if(number == 0) return String("0");
        do
        {
            buffer[--position] = '0' + number % 10;
            number /= 10;
        } while (number != 0);
        return buffer + position;
    }

    inline bool operator==(const String& string) const { return stringEquals(m_data, string.m_data); }
    inline bool operator!=(const String& string) const { return !stringEquals(m_data, string.m_data); }; 
    inline bool startsWith(const String& string) const { return substringEquals(m_data, string.m_data); }
    String toLower() const
    {
        String result;
        result.reserve(getCount());
        for(char c: *this)
            result.push(charToLower(c));
        return result;
    }

    String toUpper() const
    {
        String result;
        result.reserve(getCount());
        for(char c: *this)
            result.push(charToUpper(c));
        return result;
    }

    void append(const String& string)
    {
        reserve(m_count + string.m_count);

        for(usize i = 0; i < string.m_count; ++i)
            m_data[m_count + i] = string.m_data[i];
        m_count += string.m_count;
    }

    inline const char* const getData() const { return m_data; }
};