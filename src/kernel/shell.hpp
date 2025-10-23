#pragma once
#include "string.hpp"

struct Variable final
{
    String name, value;
};

struct Command final
{
    const char* name;
    const char* description;
    i32(*procedure)(const char* arguments, Vector<Variable>& variables);
};

class Shell
{
public:
    Shell();
    i32 executeCommand(const char* raw_command);
private:
    const Vector<Command> m_commands;
    Vector<Variable> m_variables;
};