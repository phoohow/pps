#pragma once

#include <frontend/parser.h>

#include <unordered_map>
#include <memory>

namespace pps
{
class ExprSimplifier
{
    const std::unordered_map<std::string, std::string>& m_instances;

public:
    explicit ExprSimplifier(const std::unordered_map<std::string, std::string>& instances);

    std::unique_ptr<Node> simplify(const Node* node);

private:
    std::unique_ptr<Node> _simplify_node(const Node* node);

    std::unique_ptr<Node> _simplify_variable_node(const VariableNode* node);
    std::unique_ptr<Node> _simplify_binary_op_node(const BinaryOpNode* node);
    std::unique_ptr<Node> _simplify_unary_op_node(const UnaryOpNode* node);
};

} // namespace pps
