#include <pipeline/simplifier.h>

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
    std::cout << "Original Parser:" << std::endl;
    expr->print();

    pps::ExprSimplifier simplifier(variables);
    auto                simplifiedExpr = simplifier.simplify(expr.get());

    std::cout << "\nSimplified Parser:" << std::endl;
    if (simplifiedExpr)
        simplifiedExpr->print();
    else
        std::cout << "false" << std::endl;

    return 0;
}
