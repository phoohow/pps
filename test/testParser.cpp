#include <frontend/lexer.h>
#include <frontend/parser.h>

#include <vector>
#include <iostream>
#include <cassert>

// Structure to hold test cases
struct TestCase
{
    std::string name;
    std::string input;
};

// Test cases covering various expression types and edge cases
const std::vector<TestCase> testCases = {
    {"Basic logic", "(@isRaster && @useShadow) || (!@hasSun && @isCloudy)"},
    {"Op precedence", "@a + @b * @c > 10 && @d == 5 || !@e"},
    {"Nested logic", "@a || (@b && @c) || (@d && !@e)"},
    {"Simple comp", "@value > 100"},
    {"Negation", "!@flag"},
    {"Empty expr", ""},
    {"Invalid op", "@a + @b + @c"},
};

int main()
{
    int passed = 0;
    for (const auto& test : testCases)
    {
        pps::Lexer  lexer(test.input);
        auto        tokens = lexer.tokenize();
        pps::Parser parser(tokens);
        auto        ast = parser.parse();

        if (ast)
        {
            passed++;
            std::cout << "[PASS] " << test.name << std::endl;
        }
        else
        {
            std::cout << "[FAIL] " << test.name << std::endl;
        }
    }

    std::cout << "Passed: " << passed << "/" << testCases.size() << std::endl;
    return passed == testCases.size() ? 0 : 1;
}