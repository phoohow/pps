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
    std::unique_ptr<Node> simplifyNode(const Node* node);

    std::unique_ptr<Node> simplifyVariableNode(const VariableNode* node);
    std::unique_ptr<Node> simplifyBinaryOpNode(const BinaryOpNode* node);
    std::unique_ptr<Node> simplifyUnaryOpNode(const UnaryOpNode* node);
};

} // namespace pps
