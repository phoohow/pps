#pragma once

#include <frontend/parser.h>

#include <string>
#include <memory>

namespace pps
{

    class ExprGenerator
    {
    public:
        std::string generate(const Node *node);

    private:
        std::string generateVariableNode(const VariableNode *node);
        std::string generateBinaryOpNode(const BinaryOpNode *node);
        std::string generateUnaryOpNode(const UnaryOpNode *node);
    };

} // namespace pps
