#include <pipeline/generator.h>

namespace pps
{

    std::string ExprGenerator::generate(const Node *node)
    {
        if (!node)
            return "false";

        switch (node->type())
        {
        case NodeType::tVariable:
            return generateVariableNode(static_cast<const VariableNode *>(node));

        case NodeType::tOp_binary:
            return generateBinaryOpNode(static_cast<const BinaryOpNode *>(node));

        case NodeType::tOp_unary:
            return generateUnaryOpNode(static_cast<const UnaryOpNode *>(node));

        default:
            return "";
        }
    }

    std::string ExprGenerator::generateVariableNode(const VariableNode *node)
    {
        return node->name;
    }

    std::string ExprGenerator::generateBinaryOpNode(const BinaryOpNode *node)
    {
        std::string left = generate(node->left.get());
        std::string right = generate(node->right.get());

        if (node->op.type == TokenType::tOp_and)
            return "(" + left + " && " + right + ")";
        if (node->op.type == TokenType::tOp_or)
            return "(" + left + " || " + right + ")";

        return "";
    }

    std::string ExprGenerator::generateUnaryOpNode(const UnaryOpNode *node)
    {
        std::string child = generate(node->child.get());

        if (node->op.type == TokenType::tOp_not)
            return "(!" + child + ")";

        return "";
    }

} // namespace pps
