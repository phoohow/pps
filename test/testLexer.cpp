#include <frontend/lexer.h>

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
    {"Empty", ""},
    {"Keywords", "if elif else endif false true"},
    {"Numerics", "42 3 01"},
    {"String", "\"hello world\""},
    {"VarNames", "@validVar @_var @var123 @VarWithCaps"},
    {"LogicOps", "(@isRaster && @useShadow) || (@hasSun && @isCloudy)"},
    {"CompArith", "@value > 100 || @count == 5 && @score + 20 <= 100"},
    {"NestedExpr", "((@a || @b) && (@c != @d)) || (@e > 100 + @f)"},
};

int main()
{
    int passed = 0;
    for (const auto &test : testCases)
    {
        pps::Lexer lexer(test.input);
        auto tokens = lexer.tokenize();

        if (!tokens.empty())
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