#include <frontend/parser.h>

#include <aclg/aclg.h>

#include <iostream>
#include <stdexcept>

namespace pps
{

Parser::Parser(const std::vector<Token>& tokens) :
    m_tokens(tokens), m_pos(0) {}
std::unique_ptr<Node> Parser::parse()
{
    std::vector<std::unique_ptr<Node>> statements;

    while (!_match(TokenType::tEOF))
    {
        auto statement = _parse_statement();
        if (!statement)
            break;
        statements.push_back(std::move(statement));
    }

    if (statements.size() == 1)
        return std::move(statements[0]);

    return std::make_unique<StmtCompoundNode>(std::move(statements));
}

std::unique_ptr<Node> Parser::_parse_statement()
{
    while (_match(TokenType::tEnter))
        _consume();

    if (_matchOneOf({TokenType::tType_int, TokenType::tType_bool, TokenType::tType_string}))
    {
        return _stmt_declaration();
    }
    if (_match(TokenType::tCondition_if))
    {
        return _stmt_condition();
    }
    else if (_match(TokenType::tVariable) && _peek(1).type == TokenType::tOp_assign)
    {
        return _stmt_assignment();
    }
    else if (_matchOneOf({TokenType::tOp_bitNot, TokenType::tOp_not, TokenType::tLit_bool,
                          TokenType::tLit_int, TokenType::tLit_string, TokenType::tVariable,
                          TokenType::tOp_Lparen}))
    {
        return _stmt_expression();
    }
    else if (_match(TokenType::tEOF))
    {
        ACLG_ERROR("Empty statement");
        return nullptr;
    }

    ACLG_ERROR("Unknown statement type");
    return nullptr;
}

std::unique_ptr<Node> Parser::_op_logic_or()
{
    auto left = _op_logic_and();
    while (_match(TokenType::tOp_or))
    {
        Token op    = _consume();
        auto  right = _op_logic_and();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_logic_and()
{
    auto left = _op_bit_or();
    while (_match(TokenType::tOp_and))
    {
        Token op    = _consume();
        auto  right = _op_bit_or();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_bit_or()
{
    auto left = _op_bit_xor();
    while (_match(TokenType::tOp_bitOr))
    {
        Token op    = _consume();
        auto  right = _op_bit_xor();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_bit_xor()
{
    auto left = _op_bit_and();
    while (_match(TokenType::tOp_bitXor))
    {
        Token op    = _consume();
        auto  right = _op_bit_and();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_bit_and()
{
    auto left = _op_equality();
    while (_match(TokenType::tOp_bitAnd))
    {
        Token op    = _consume();
        auto  right = _op_equality();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_equality()
{
    auto left = _op_relation();
    while (_matchOneOf({TokenType::tOp_equal, TokenType::tOp_unequal}))
    {
        Token op    = _consume();
        auto  right = _op_relation();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_relation()
{
    auto left = _op_shift();
    while (_matchOneOf({TokenType::tOp_less, TokenType::tOp_greater}))
    {
        Token op    = _consume();
        auto  right = _op_shift();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_shift()
{
    auto left = _op_additive();
    while (_matchOneOf({TokenType::tOp_bitLMove, TokenType::tOp_bitRMove}))
    {
        Token op    = _consume();
        auto  right = _op_additive();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_additive()
{
    auto left = _op_multiplicative();
    while (_matchOneOf({TokenType::tOp_add, TokenType::tOp_sub}))
    {
        Token op    = _consume();
        auto  right = _op_multiplicative();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_multiplicative()
{
    auto left = _op_unary();
    while (_matchOneOf({TokenType::tOp_mul, TokenType::tOp_div, TokenType::tOp_mod}))
    {
        Token op    = _consume();
        auto  right = _op_unary();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::_op_unary()
{
    if (_matchOneOf({TokenType::tOp_not, TokenType::tOp_bitNot}))
    {
        Token op    = _consume();
        auto  child = _op_unary();
        return std::make_unique<UnaryOpNode>(op, std::move(child));
    }

    return _primary();
}

std::unique_ptr<Node> Parser::_primary()
{
    if (_match(TokenType::tVariable))
    {
        Token token = _consume();
        return std::make_unique<VariableNode>(token.value);
    }
    else if (_match(TokenType::tLit_int))
    {
        Token token = _consume();
        return std::make_unique<LitIntNode>(std::stoi(token.value));
    }
    else if (_match(TokenType::tLit_bool))
    {
        Token token = _consume();
        return std::make_unique<LitBoolNode>(token.value == "true");
    }
    else if (_match(TokenType::tLit_string))
    {
        Token token = _consume();
        return std::make_unique<LitStringNode>(token.value);
    }
    else if (_match(TokenType::tOp_Lparen))
    {
        _consume();
        auto node = _op_logic_or();
        if (!_match(TokenType::tOp_Rparen))
        {
            ACLG_ERROR("Expected ')'");
        }
        _consume();
        return node;
    }
    else
    {
        ACLG_ERROR("Unexpected token in primary");
        return nullptr;
    }
}

std::unique_ptr<Node> Parser::_stmt_declaration()
{
    Token typeToken = _consume();
    Token nameToken = _consume();
    _consume();
    auto value = _op_logic_or();
    return std::make_unique<StmtDeclarationNode>(typeToken, nameToken.value, std::move(value));
}

std::unique_ptr<Node> Parser::_stmt_assignment()
{
    Token nameToken = _consume();
    if (nameToken.type != TokenType::tVariable)
        throw std::runtime_error("Expected identifier in assignment");

    if (!_match(TokenType::tOp_assign))
        throw std::runtime_error("Expected '=' in assignment");
    _consume();

    auto value = _op_logic_or();
    return std::make_unique<StmtAssignmentNode>(nameToken.value, std::move(value));
}

std::unique_ptr<Node> Parser::_stmt_expression()
{
    return _op_logic_or();
}

std::unique_ptr<Node> Parser::_stmt_condition()
{
    std::vector<StmtConditionNode::ConditionBlock> branches;

    do
    {
        _consume();
        auto condition = _op_logic_or();
        auto block     = _parse_statement();
        branches.push_back({std::move(condition), std::move(block)});
    } while (_match(TokenType::tCondition_elif));

    while (_match(TokenType::tEnter))
        _consume();

    std::unique_ptr<Node> else_block = nullptr;
    if (_match(TokenType::tCondition_else))
    {
        _consume();
        else_block = _parse_statement();
    }

    while (_match(TokenType::tEnter))
        _consume();

    if (!_match(TokenType::tCondition_endif))
        ACLG_ERROR("Expected 'endif'");
    _consume();

    return std::make_unique<StmtConditionNode>(std::move(branches), std::move(else_block));
}

Token Parser::_peek(int n) const
{
    if (m_pos + n < m_tokens.size())
    {
        return m_tokens[m_pos + n];
    }
    return Token(TokenType::tEOF, "");
}

Token Parser::_consume()
{
    if (m_pos < m_tokens.size())
    {
        return m_tokens[m_pos++];
    }
    return Token(TokenType::tEOF, "");
}

bool Parser::_match(TokenType type)
{
    return _peek().type == type;
}

bool Parser::_matchOneOf(std::initializer_list<TokenType> types)
{
    for (auto t : types)
    {
        if (_peek().type == t)
        {
            return true;
        }
    }
    return false;
}

} // namespace pps
