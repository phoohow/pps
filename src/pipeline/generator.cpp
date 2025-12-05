#include <pipeline/generator.h>

#include <aclg/aclg.h>

namespace pps
{

std::string ExprGenerator::generate(const Node* node)
{
    if (!node)
        return "false";

    switch (node->type())
    {
        case NodeType::tVariable:
            return _gen_variable_node(static_cast<const VariableNode*>(node));

        case NodeType::tOp_binary:
            return _gen_binary_op_node(static_cast<const BinaryOpNode*>(node));

        case NodeType::tOp_unary:
            return _gen_unary_op_node(static_cast<const UnaryOpNode*>(node));

        default:
            ACLG_ERROR("Unknown node type");
            return "";
    }
}

std::string ExprGenerator::_gen_variable_node(const VariableNode* node)
{
    return node->name;
}

std::string ExprGenerator::_gen_binary_op_node(const BinaryOpNode* node)
{
    std::string left  = generate(node->left.get());
    std::string right = generate(node->right.get());

    if (node->op.type == TokenType::tOp_and)
        return "(" + left + " && " + right + ")";
    if (node->op.type == TokenType::tOp_or)
        return "(" + left + " || " + right + ")";

    ACLG_WARN("operator: {} is not supported", node->op.value);
    return "";
}

std::string ExprGenerator::_gen_unary_op_node(const UnaryOpNode* node)
{
    std::string child = generate(node->child.get());

    if (node->op.type == TokenType::tOp_not)
        return "(!" + child + ")";

    ACLG_WARN("operator: {} is not supported", node->op.value);
    return "";
}

} // namespace pps
