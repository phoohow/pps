#pragma once

#include <frontend/parser.h>

#include <unordered_map>
#include <memory>

namespace pps
{
    class ExprSimplifier
    {
        std::unordered_map<std::string, bool> m_variables;

    public:
        explicit ExprSimplifier(const std::unordered_map<std::string, bool> &variables);

        std::unique_ptr<Node> simplify(const Node *node);

    private:
        std::unique_ptr<Node> simplifyNode(const Node *node);
        
        std::unique_ptr<Node> simplifyVariableNode(const VariableNode *node);
        std::unique_ptr<Node> simplifyBinaryOpNode(const BinaryOpNode *node);
        std::unique_ptr<Node> simplifyUnaryOpNode(const UnaryOpNode *node);
    };

} // namespace pps
