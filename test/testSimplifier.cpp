#include <pipeline/simplifier.h>

#include <iostream>

int main()
{
    std::string input = "(@isRaster && @useShadow) || (!@hasSun && @isDay)";

    std::unordered_map<std::string, std::string> instances = {
        {"@isRaster", "Render.isRaster"},
        {"@useShadow", "scene.useShow"},
        {"@hasSun", "scene.hasSun"},
    };

    pps::Lexer lexer(input);
    auto       tokens = lexer.tokenize();

    pps::Parser parser(tokens);
    auto        expr = parser.parse();
    std::cout << "Original Parser:" << std::endl;
    expr->print();

    pps::ExprSimplifier simplifier(instances);
    auto                simplifiedExpr = simplifier.simplify(expr.get());

    std::cout << "\nSimplified Parser:" << std::endl;
    if (simplifiedExpr)
        simplifiedExpr->print();
    else
        std::cout << "false" << std::endl;

    return 0;
}
