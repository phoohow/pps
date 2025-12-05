#include <pipeline/simplifier.h>

#include <aclg/aclg.h>

namespace pps
{
ExprSimplifier::ExprSimplifier(const std::unordered_map<std::string, std::string>& instances) :
    m_instances(instances) {}

std::unique_ptr<Node> ExprSimplifier::simplify(const Node* node)
{
    return _simplify_node(node);
}

std::unique_ptr<Node> ExprSimplifier::_simplify_node(const Node* node)
{
    if (!node)
        return nullptr;

    switch (node->type())
    {
        case NodeType::tVariable:
            return _simplify_variable_node(static_cast<const VariableNode*>(node));

        case NodeType::tOp_binary:
            return _simplify_binary_op_node(static_cast<const BinaryOpNode*>(node));

        case NodeType::tOp_unary:
            return _simplify_unary_op_node(static_cast<const UnaryOpNode*>(node));

        default:
            ACLG_ERROR("Unknown node type");
            return std::unique_ptr<Node>(const_cast<Node*>(node));
    }
}

std::unique_ptr<Node> ExprSimplifier::_simplify_variable_node(const VariableNode* node)
{
    auto iter = m_instances.find(node->name);
    return iter == m_instances.end() ? nullptr :
                                       std::make_unique<VariableNode>(node->name);
}

std::unique_ptr<Node> ExprSimplifier::_simplify_binary_op_node(const BinaryOpNode* node)
{
    auto left  = _simplify_node(node->left.get());
    auto right = _simplify_node(node->right.get());

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

std::unique_ptr<Node> ExprSimplifier::_simplify_unary_op_node(const UnaryOpNode* node)
{
    auto child = _simplify_node(node->child.get());
    if (!child)
        return nullptr;
    return std::make_unique<UnaryOpNode>(node->op, std::move(child));
}

} // namespace pps
