#pragma once

#include <frontend/node.h>

#include <memory>
#include <vector>

namespace pps
{
class Node;
class Parser
{
    const std::vector<Token>& m_tokens;

    size_t m_pos;

public:
    explicit Parser(const std::vector<Token>& tokens);

    std::unique_ptr<Node> parse();

private:
    std::unique_ptr<Node> op_logicalOr();
    std::unique_ptr<Node> op_logicalAnd();
    std::unique_ptr<Node> op_bitOr();
    std::unique_ptr<Node> op_bitXor();
    std::unique_ptr<Node> op_bitAnd();
    std::unique_ptr<Node> op_equality();
    std::unique_ptr<Node> op_relation();
    std::unique_ptr<Node> op_shift();
    std::unique_ptr<Node> op_additive();
    std::unique_ptr<Node> op_multiplicative();
    std::unique_ptr<Node> op_unary();
    std::unique_ptr<Node> primary();
    std::unique_ptr<Node> parse_statement();
    std::unique_ptr<Node> stmt_declaration();
    std::unique_ptr<Node> stmt_condition();
    std::unique_ptr<Node> stmt_assignment();
    std::unique_ptr<Node> stmt_expression();

    Token peek(int n = 0) const;
    Token consume();
    bool  match(TokenType type);
    bool  matchOneOf(std::initializer_list<TokenType> types);
};

} // namespace pps
