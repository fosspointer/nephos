#pragma once
#include "string.hpp"

struct Variable final
{
    String name, value;
};

class Shell
{
public:
    struct Command final
    {
        const char* name;
        const char* description;
        i32(*procedure)(const char* arguments, Shell& shell);
    };
    Shell();
    i32 executeCommand(const char* raw_command);
    inline const Vector<Variable>& getVariables() const { return m_variables; }
    inline Vector<Variable>& getVariables() { return m_variables; }
private:
    const Vector<Command> m_commands;
    Vector<Variable> m_variables;
};