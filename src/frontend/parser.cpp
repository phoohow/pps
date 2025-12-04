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

    while (!match(TokenType::tEOF))
    {
        auto statement = parse_statement();
        if (!statement)
            break;
        statements.push_back(std::move(statement));
    }

    if (statements.size() == 1)
        return std::move(statements[0]);

    return std::make_unique<StmtCompoundNode>(std::move(statements));
}

std::unique_ptr<Node> Parser::parse_statement()
{
    while (match(TokenType::tEnter))
        consume();

    if (matchOneOf({TokenType::tType_int, TokenType::tType_bool, TokenType::tType_string}))
    {
        return stmt_declaration();
    }
    if (match(TokenType::tCondition_if))
    {
        return stmt_condition();
    }
    else if (match(TokenType::tVariable) && peek(1).type == TokenType::tOp_assign)
    {
        return stmt_assignment();
    }
    else if (matchOneOf({TokenType::tOp_bitNot, TokenType::tOp_not, TokenType::tLit_bool,
                         TokenType::tLit_int, TokenType::tLit_string, TokenType::tVariable,
                         TokenType::tOp_Lparen}))
    {
        return stmt_expression();
    }
    else if (match(TokenType::tEOF))
    {
        ACLG_ERROR("Empty statement");
        return nullptr;
    }

    ACLG_ERROR("Unknown statement type");
    return nullptr;
}

std::unique_ptr<Node> Parser::op_logicalOr()
{
    auto left = op_logicalAnd();
    while (match(TokenType::tOp_or))
    {
        Token op    = consume();
        auto  right = op_logicalAnd();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_logicalAnd()
{
    auto left = op_bitOr();
    while (match(TokenType::tOp_and))
    {
        Token op    = consume();
        auto  right = op_bitOr();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_bitOr()
{
    auto left = op_bitXor();
    while (match(TokenType::tOp_bitOr))
    {
        Token op    = consume();
        auto  right = op_bitXor();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_bitXor()
{
    auto left = op_bitAnd();
    while (match(TokenType::tOp_bitXor))
    {
        Token op    = consume();
        auto  right = op_bitAnd();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_bitAnd()
{
    auto left = op_equality();
    while (match(TokenType::tOp_bitAnd))
    {
        Token op    = consume();
        auto  right = op_equality();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_equality()
{
    auto left = op_relation();
    while (matchOneOf({TokenType::tOp_equal, TokenType::tOp_unequal}))
    {
        Token op    = consume();
        auto  right = op_relation();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_relation()
{
    auto left = op_shift();
    while (matchOneOf({TokenType::tOp_less, TokenType::tOp_greater}))
    {
        Token op    = consume();
        auto  right = op_shift();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_shift()
{
    auto left = op_additive();
    while (matchOneOf({TokenType::tOp_bitLMove, TokenType::tOp_bitRMove}))
    {
        Token op    = consume();
        auto  right = op_additive();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_additive()
{
    auto left = op_multiplicative();
    while (matchOneOf({TokenType::tOp_add, TokenType::tOp_sub}))
    {
        Token op    = consume();
        auto  right = op_multiplicative();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_multiplicative()
{
    auto left = op_unary();
    while (matchOneOf({TokenType::tOp_mul, TokenType::tOp_div, TokenType::tOp_mod}))
    {
        Token op    = consume();
        auto  right = op_unary();
        left        = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::op_unary()
{
    if (matchOneOf({TokenType::tOp_not, TokenType::tOp_bitNot}))
    {
        Token op    = consume();
        auto  child = op_unary();
        return std::make_unique<UnaryOpNode>(op, std::move(child));
    }

    return primary();
}

std::unique_ptr<Node> Parser::primary()
{
    if (match(TokenType::tVariable))
    {
        Token token = consume();
        return std::make_unique<VariableNode>(token.value);
    }
    else if (match(TokenType::tLit_int))
    {
        Token token = consume();
        return std::make_unique<LitIntNode>(std::stoi(token.value));
    }
    else if (match(TokenType::tLit_bool))
    {
        Token token = consume();
        return std::make_unique<LitBoolNode>(token.value == "true");
    }
    else if (match(TokenType::tLit_string))
    {
        Token token = consume();
        return std::make_unique<LitStringNode>(token.value);
    }
    else if (match(TokenType::tOp_Lparen))
    {
        consume();
        auto node = op_logicalOr();
        if (!match(TokenType::tOp_Rparen))
        {
            ACLG_ERROR("Expected ')'");
        }
        consume();
        return node;
    }
    else
    {
        ACLG_ERROR("Unexpected token in primary");
        return nullptr;
    }
}

std::unique_ptr<Node> Parser::stmt_declaration()
{
    Token typeToken = consume();
    Token nameToken = consume();
    consume();
    auto value = op_logicalOr();
    return std::make_unique<StmtDeclarationNode>(typeToken, nameToken.value, std::move(value));
}

std::unique_ptr<Node> Parser::stmt_assignment()
{
    Token nameToken = consume();
    if (nameToken.type != TokenType::tVariable)
        throw std::runtime_error("Expected identifier in assignment");

    if (!match(TokenType::tOp_assign))
        throw std::runtime_error("Expected '=' in assignment");
    consume();

    auto value = op_logicalOr();
    return std::make_unique<StmtAssignmentNode>(nameToken.value, std::move(value));
}

std::unique_ptr<Node> Parser::stmt_expression()
{
    return op_logicalOr();
}

std::unique_ptr<Node> Parser::stmt_condition()
{
    std::vector<StmtConditionNode::ConditionBlock> branches;

    do
    {
        consume();
        auto condition = op_logicalOr();
        auto block     = parse_statement();
        branches.push_back({std::move(condition), std::move(block)});
    } while (match(TokenType::tCondition_elif));

    while (match(TokenType::tEnter))
        consume();

    std::unique_ptr<Node> elseBlock = nullptr;
    if (match(TokenType::tCondition_else))
    {
        consume();
        elseBlock = parse_statement();
    }

    while (match(TokenType::tEnter))
        consume();

    if (!match(TokenType::tCondition_endif))
        ACLG_ERROR("Expected 'endif'");
    consume();

    return std::make_unique<StmtConditionNode>(std::move(branches), std::move(elseBlock));
}

Token Parser::peek(int n) const
{
    if (m_pos + n < m_tokens.size())
    {
        return m_tokens[m_pos + n];
    }
    return Token(TokenType::tEOF, "");
}

Token Parser::consume()
{
    if (m_pos < m_tokens.size())
    {
        return m_tokens[m_pos++];
    }
    return Token(TokenType::tEOF, "");
}

bool Parser::match(TokenType type)
{
    return peek().type == type;
}

bool Parser::matchOneOf(std::initializer_list<TokenType> types)
{
    for (auto t : types)
    {
        if (peek().type == t)
        {
            return true;
        }
    }
    return false;
}

} // namespace pps
