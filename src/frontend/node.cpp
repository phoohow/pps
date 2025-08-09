#include <frontend/node.h>

#include <iostream>

namespace pps
{
VariableNode::VariableNode(const std::string& name) :
    name(name) {}
void VariableNode::print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << "Variable: " << name << "\n";
}

BinaryOpNode::BinaryOpNode(const Token& op, std::unique_ptr<Node> left, std::unique_ptr<Node> right) :
    op(op), left(std::move(left)), right(std::move(right)) {}
void BinaryOpNode::print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << "BinaryOp: " << op.value << "\n";
    left->print(depth + 1);
    right->print(depth + 1);
}

UnaryOpNode::UnaryOpNode(const Token& op, std::unique_ptr<Node> child) :
    op(op), child(std::move(child)) {}
void UnaryOpNode::print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << "UnaryOp: " << op.value << "\n";
    child->print(depth + 1);
}

LitIntNode::LitIntNode(int value) :
    value(value) {}
void LitIntNode::print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << "IntLit: " << value << "\n";
}

LitBoolNode::LitBoolNode(bool value) :
    value(value) {}
void LitBoolNode::print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << "BoolLit: " << (value ? "true" : "false") << "\n";
}

LitStringNode::LitStringNode(const std::string& value) :
    value(value) {}
void LitStringNode::print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << "StringLit: " << value << "\n";
}

StmtDeclarationNode::StmtDeclarationNode(const Token& _varType, const std::string& _varName, std::unique_ptr<Node> _value) :
    varType(_varType), varName(_varName), value(std::move(_value)) {}
void StmtDeclarationNode::print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << "Declaration: " << varType.value << " " << varName << "\n";
    if (value)
        value->print(depth + 1);
}

StmtAssignmentNode::StmtAssignmentNode(const std::string& name, std::unique_ptr<Node> value) :
    name(name), value(std::move(value)) {}
void StmtAssignmentNode::print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << "Assignment: " << name << "\n";
    value->print(depth + 1);
}

StmtExpressionNode::StmtExpressionNode(std::unique_ptr<Node> value) :
    value(std::move(value)) {}
void StmtExpressionNode::print(int depth) const
{
    std::cout << std::string(depth * 2, ' ') << "Expression:\n";
    value->print(depth + 1);
}

StmtConditionNode::StmtConditionNode(std::vector<ConditionBlock> branches, std::unique_ptr<Node> elseBlock) :
    branches(std::move(branches)), elseBlock(std::move(elseBlock)) {}
void StmtConditionNode::print(int depth) const
{
    for (const auto& branch : branches)
    {
        std::cout << std::string(depth * 2, ' ') << "If:\n";
        branch.condition->print(depth + 1);
        std::cout << std::string(depth * 2, ' ') << "Then:\n";
        branch.block->print(depth + 1);
    }
    if (elseBlock)
    {
        std::cout << std::string(depth * 2, ' ') << "Else:\n";
        elseBlock->print(depth + 1);
    }
}

StmtCompoundNode::StmtCompoundNode(std::vector<std::unique_ptr<Node>> statements) :
    statements(std::move(statements)) {}
void StmtCompoundNode::print(int depth) const
{
    for (const auto& stmt : statements)
    {
        stmt->print(depth);
        std::cout << std::endl;
    }
}

} // namespace pps
