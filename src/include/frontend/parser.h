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
    std::unique_ptr<Node> _op_logic_or();
    std::unique_ptr<Node> _op_logic_and();
    std::unique_ptr<Node> _op_bit_or();
    std::unique_ptr<Node> _op_bit_xor();
    std::unique_ptr<Node> _op_bit_and();
    std::unique_ptr<Node> _op_equality();
    std::unique_ptr<Node> _op_relation();
    std::unique_ptr<Node> _op_shift();
    std::unique_ptr<Node> _op_additive();
    std::unique_ptr<Node> _op_multiplicative();
    std::unique_ptr<Node> _op_unary();
    std::unique_ptr<Node> _primary();
    std::unique_ptr<Node> _parse_statement();
    std::unique_ptr<Node> _stmt_declaration();
    std::unique_ptr<Node> _stmt_condition();
    std::unique_ptr<Node> _stmt_assignment();
    std::unique_ptr<Node> _stmt_expression();

    Token _peek(int n = 0) const;
    Token _consume();
    bool  _match(TokenType type);
    bool  _matchOneOf(std::initializer_list<TokenType> types);
};

} // namespace pps
