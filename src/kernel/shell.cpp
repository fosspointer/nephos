#include "shell.hpp"
#include "configuration.hpp"
#include "io.hpp"

Shell::Shell()
    :m_commands{
        Command{
            .name = "version",
            .description = "Displays the current nephos version in use",
            .procedure = [](const char* arguments, Vector<Variable>& variables)
            {
                IO::putsln(Configuration::version);
                return 0;
            }
        },
        Command{
            .name = "colortest",
            .description = "Command to test IO output, scrolling and colors",
            .procedure = [](const char* arguments, Vector<Variable>& variables)
            {
                for(u32 i = 0; i < 0xff; ++i)
                {
                    IO::colorMode = i;
                    IO::puti(i, 16);
                    IO::puts(", ");
                }
                IO::putln();
                return 0;
            }
        },
        Command{
            .name = "echo",
            .description = "Print specified text to the screen",
            .procedure = [](const char* arguments, Vector<Variable>& variables)
            {
                IO::putsln(arguments);
                return 0;
            }
        },
        Command{
            .name = "meow",
            .description = "Meow",
            .procedure = [](const char* arguments, Vector<Variable>& variables)
            {
                IO::putsln("meow :3");
                return 0;
            }
        },
        Command{
            .name = "true",
            .description = "Command that exits successfully",
            .procedure = [](const char* arguments, Vector<Variable>& variables)
            {
                return 0;
            }
        },
        Command{
            .name = "false",
            .description = "Command that exits unsuccessfully",
            .procedure = [](const char* arguments, Vector<Variable>& variables)
            {
                return 1;
            }
        },
        Command{
            .name = "clear",
            .description = "Clears the screen",
            .procedure = [](const char* arguments, Vector<Variable>& variables)
            {
                IO::initScreen();
                return 0;
            }
        },
        Command{
            .name = "crash",
            .description = "Throws an invalid opcode exception to crash your system.",
            .procedure = [](const char* arguments, Vector<Variable>& variables) -> int
            {
                __asm__("ud2");
                __builtin_unreachable();
            }
        },
        Command{
            .name = "printenv",
            .description = "Prints all of the environment variables.",
            .procedure = [](const char* arguments, Vector<Variable>& variables)
            {
                for(const auto& variable: variables)
                {
                    IO::puts(variable.name);
                    IO::putc('=');
                    IO::putsln(variable.value);
                }
                return 0;
            }
        },
        // Command{
        //     .name = "repeat",
        //     .description = "Executes a given command a specified number of times.",
        //     .procedure = [](const char* arguments, Vector<Variable>& variables)
        //     {
        //         // executeCommand();
        //         return 0;
        //     }
        // }
    },
    m_variables{Variable{.name = "status", .value = "0"}}
{}

i32 Shell::executeCommand(const char* raw_command)
{
    char command_buffer[16] = {'\0'};
    u32 i = 0;

    while(true)
    {
        if(i >= 14) break;
        else if(!(command_buffer[i] = *(raw_command++))) break;
        else if(command_buffer[i] == ' ')
        {
            command_buffer[i] = '\0';
            break;
        }
        ++i;
    }
    for(usize i = 0; i < stringLength(command_buffer); ++i)
        command_buffer[i] = charToLower(command_buffer[i]);
    
    String evaluated_command;
    evaluated_command.reserve(stringLength(raw_command));
    auto command_pointer = raw_command;
    while(auto character = *(command_pointer++))
    {
        if(character != '$')
        {
            evaluated_command.push(character);
            continue;
        }

        if(*command_pointer == ':')
        {
            ++command_pointer;
            String name_buffer, value_buffer;
            while(*command_pointer)
            {
                if(*command_pointer == '=') break;

                name_buffer.push(*command_pointer);
                ++command_pointer;
            }
            ++command_pointer;

            while(*command_pointer)
            {
                if(*command_pointer == ' ') break;
                value_buffer.push(*command_pointer);
                ++command_pointer;
            }

            [&](){
                for(auto& variable: m_variables)
                    if(variable.name == name_buffer)
                    {
                        variable.value = value_buffer;
                        evaluated_command.append(variable.value);
                        return;
                    }

                m_variables.push(Variable{.name = name_buffer, .value = value_buffer});
                evaluated_command.append(m_variables.getBack().value);
            }();

            continue;
        }

        [&](){
            for(const auto& variable: m_variables)
                if(variable.name.startsWith(command_pointer))
                {
                    command_pointer += variable.name.getCount();
                    evaluated_command.append(variable.value);
                    return;
                }
            
            while(auto character = *(command_pointer++))
                if(character == ' ')
                {
                    --command_pointer;
                    break;
                }
            evaluated_command.append("null");
        }();
    }

    evaluated_command.push('\0');
    for(const auto& command: m_commands)
        if(stringEquals(command_buffer, command.name))
        {
            auto exit_code = command.procedure(evaluated_command.getData(), m_variables);
            m_variables[0].value = String::fromI32(exit_code);
            return exit_code;
        }
    if(stringEquals(command_buffer, "help"))
    {
        IO::putsln("List of available commands:\n - help: Displays this page");
        for(const auto& command: m_commands)
        {
            IO::puts(" - ");
            IO::puts(command.name);
            IO::puts(": ");
            IO::putsln(command.description);
        }
        m_variables[0].value = "0";
        return 0;
    }

    m_variables[0].value = "-1";
    IO::putsln("Unknown command. Type help for help.");
    return 1;
}