#pragma once

#include <frontend/lexer.h>

#include <memory>

namespace pps
{

enum class NodeType
{
    tLit_int,
    tLit_bool,
    tLit_string,

    tVariable,

    tOp_binary,
    tOp_unary,

    tStmt_declaration,
    tStmt_assignment,
    tStmt_condition,
    tStmt_expression,
    tStmt_compound,
};

class Node
{
public:
    virtual ~Node()                             = default;
    virtual void     print(int depth = 0) const = 0;
    virtual NodeType type() const               = 0;
};

class VariableNode : public Node
{
public:
    std::string name;

    explicit VariableNode(const std::string& name);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tVariable; }
};

class BinaryOpNode : public Node
{
public:
    Token                 op;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit BinaryOpNode(const Token& op, std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tOp_binary; }
};

class UnaryOpNode : public Node
{
public:
    Token                 op;
    std::unique_ptr<Node> child;

    explicit UnaryOpNode(const Token& op, std::unique_ptr<Node> child);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tOp_unary; }
};

class LitIntNode : public Node
{
public:
    int value;

    explicit LitIntNode(int value);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tLit_int; }
};

class LitBoolNode : public Node
{
public:
    bool value;

    explicit LitBoolNode(bool value);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tLit_bool; }
};

class LitStringNode : public Node
{
public:
    std::string value;

    explicit LitStringNode(const std::string& value);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tLit_string; }
};

class StmtDeclarationNode : public Node
{
public:
    Token                 varType;
    std::string           varName;
    std::unique_ptr<Node> value;

    explicit StmtDeclarationNode(const Token& varType, const std::string& varName, std::unique_ptr<Node> value);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tStmt_declaration; }
};

class StmtAssignmentNode : public Node
{
public:
    std::string           name;
    std::unique_ptr<Node> value;

    explicit StmtAssignmentNode(const std::string& name, std::unique_ptr<Node> value);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tStmt_assignment; }
};

class StmtExpressionNode : public Node
{
public:
    std::unique_ptr<Node> value;

    explicit StmtExpressionNode(std::unique_ptr<Node> value);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tStmt_expression; }
};

class StmtConditionNode : public Node
{
public:
    struct ConditionBlock
    {
        std::unique_ptr<Node> condition;
        std::unique_ptr<Node> block;
    };
    std::vector<ConditionBlock> branches;
    std::unique_ptr<Node>       elseBlock;

    explicit StmtConditionNode(std::vector<ConditionBlock> branches, std::unique_ptr<Node> elseBlock);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tStmt_condition; }
};

class StmtCompoundNode : public Node
{
public:
    std::vector<std::unique_ptr<Node>> statements;

    explicit StmtCompoundNode(std::vector<std::unique_ptr<Node>> statements);
    void     print(int depth = 0) const override;
    NodeType type() const override { return NodeType::tStmt_compound; }
};

} // namespace pps
