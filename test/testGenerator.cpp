#include <pipeline/simplifier.h>
#include <pipeline/generator.h>

#include <iostream>

int main()
{
    std::string input = "(@isRaster && @useShadow) || (!@hasSun && @isDay)";

    std::unordered_map<std::string, bool> variables = {
        {"@isRaster", false},
        {"@useShadow", true},
        {"@hasSun", true},
        {"@isDay", true},
    };

    pps::Lexer lexer(input);
    auto       tokens = lexer.tokenize();

    pps::Parser parser(tokens);
    auto        expr = parser.parse();

    pps::ExprSimplifier simplifier(variables);
    auto                simplifiedExpr = simplifier.simplify(expr.get());

    pps::ExprGenerator generator;
    std::string        expressionString = generator.generate(simplifiedExpr.get());

    std::cout << "Origin: " << input << std::endl;
    std::cout << "\nGenerated:" << expressionString << std::endl;

    return 0;
}