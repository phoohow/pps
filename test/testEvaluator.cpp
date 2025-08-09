#include <frontend/lexer.h>
#include <frontend/parser.h>
#include <pipeline/evaluator.h>
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
    {"Complex evaluation", R"(
        int @x = 10
        int @y = 0
        if @x > 5
            @y = 5
        else
            @y = 0
        endif
        @x = @x + @y + 1
        string @str = "s" + 2 + "t"
        @str >> 1
    )"},
    {"Arithmetic operations", R"(
        int @a = 10
        int @b = 5
        int @c = @a * @b / (@b - 3)
        @c
    )"},
    {"Logical operations", R"(
        bool @p = true
        bool @q = false
        bool @r = @p && @q || !@p
        @r
    )"},
    {"Division operation", R"(
        int @num = 10
        int @den = 1
        int @result = @num / @den
        @result
    )"},
    {"String concatenation", R"(
        string @s1 = "Hello"
        string @s2 = " World"
        string @s3 = @s1 + @s2
        @s3
    )"},
};

int main()
{
    int passed = 0;
    for (const auto& test : testCases)
    {
        pps::Lexer lexer(test.input);
        auto       tokens = lexer.tokenize();

        pps::Parser parser(tokens);
        auto        root = parser.parse();

        pps::Evaluator evaluator;
        auto           result = evaluator.evaluate(root.get());

        if (result)
        {
            passed++;
            std::cout << "[PASS] " << test.name << ": ";
            result->print();
            std::cout << std::endl;
        }
        else
        {
            std::cout << "[FAIL] " << test.name << std::endl;
        }
    }

    std::cout << "Passed: " << passed << "/" << testCases.size() << std::endl;
    return passed == testCases.size() ? 0 : 1;
}