#include "string.hpp"

bool stringEquals(const char* first, const char* second)
{
    while(*first && *second && *(first++) == *(second++));
    return *first == 0 && *second == 0;
}

bool substringEquals(const char* first, const char* second)
{
    while(*first && *second && *(first++) == *(second++));
    return *second == 0;
}

usize stringLength(const char* string)
{
    auto start = string;
    while(*(string++));
    return string - start - 1;
}

char charToLower(char character)
{
    if(character >= 'A' && character <= 'Z')
        return character - 'A' + 'a';
    else return character;
}

char charToUpper(char character)
{
    if(character >= 'a' && character <= 'z')
        return character - 'a' + 'A';
    else return character;
}