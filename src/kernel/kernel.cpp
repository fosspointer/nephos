#include "configuration.hpp"
#include "io.hpp"
#include "idt.hpp"
#include "memory.hpp"
#include "assert.hpp"
#include "vector.hpp"
#include "string.hpp"
#include "printf.hpp"
#include "shell.hpp"

void init()
{
    IO::initScreen();
    IDT::init();
    IO::initKeyboard();
    
    IO::colorMode = 0xd;
    IO::puts("NephOS ");
    IO::putsln(Configuration::version);
    IO::putln();
    IO::colorMode = 0xf;
}

extern "C" [[nodiscard]] int kernelMain()
{
    init();
    Shell shell;
    char input[81];
    int exit_code = 0;

    for(;;)
    {
        for(u32 i{0}; i < 81; ++i)
            input[i] = '\0';

        IO::colorMode = exit_code? 0x4: 0xe;
        IO::puts("nephos> ");
        IO::colorMode = 0xf;
        IO::reads(input, 81);
        exit_code = shell.executeCommand(input);
    }
}