#include <pipeline/simplifier.h>

namespace pps
{
ExprSimplifier::ExprSimplifier(const std::unordered_map<std::string, bool>& variables) :
    m_variables(variables) {}

std::unique_ptr<Node> ExprSimplifier::simplify(const Node* node)
{
    return simplifyNode(node);
}

std::unique_ptr<Node> ExprSimplifier::simplifyNode(const Node* node)
{
    if (!node)
        return nullptr;

    switch (node->type())
    {
        case NodeType::tVariable:
            return simplifyVariableNode(static_cast<const VariableNode*>(node));

        case NodeType::tOp_binary:
            return simplifyBinaryOpNode(static_cast<const BinaryOpNode*>(node));

        case NodeType::tOp_unary:
            return simplifyUnaryOpNode(static_cast<const UnaryOpNode*>(node));

        default:
            return std::unique_ptr<Node>(const_cast<Node*>(node));
    }
}

std::unique_ptr<Node> ExprSimplifier::simplifyVariableNode(const VariableNode* node)
{
    auto it = m_variables.find(node->name);
    if (it != m_variables.end() && !it->second)
        return nullptr;
    return std::make_unique<VariableNode>(node->name);
}

std::unique_ptr<Node> ExprSimplifier::simplifyBinaryOpNode(const BinaryOpNode* node)
{
    auto left  = simplifyNode(node->left.get());
    auto right = simplifyNode(node->right.get());

    if (node->op.type == TokenType::tOp_and ||
        node->op.type == TokenType::tOp_or ||
        node->op.type == TokenType::tOp_equal ||
        node->op.type == TokenType::tOp_unequal ||
        node->op.type == TokenType::tOp_greater ||
        node->op.type == TokenType::tOp_less ||
        node->op.type == TokenType::tOp_greaterEqual ||
        node->op.type == TokenType::tOp_lessEqual)
    {
        if (!left && !right)
            return nullptr;
        if (!left)
            return std::move(right);
        if (!right)
            return std::move(left);
        return std::make_unique<BinaryOpNode>(node->op, std::move(left), std::move(right));
    }

    return std::make_unique<BinaryOpNode>(node->op, std::move(left), std::move(right));
}

std::unique_ptr<Node> ExprSimplifier::simplifyUnaryOpNode(const UnaryOpNode* node)
{
    auto child = simplifyNode(node->child.get());
    if (!child)
        return nullptr;
    return std::make_unique<UnaryOpNode>(node->op, std::move(child));
}

} // namespace pps
