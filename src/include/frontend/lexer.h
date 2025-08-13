#pragma once

#include <string>
#include <vector>

namespace pps
{

enum class TokenType
{
    tType_int,
    tType_bool,
    tType_string,

    tOp_assign,

    // logic
    tOp_or,
    tOp_and,

    // bit
    tOp_bitOr,
    tOp_bitXor,
    tOp_bitAnd,

    // equality
    tOp_equal,
    tOp_unequal,

    // relation
    tOp_greater,
    tOp_less,
    tOp_greaterEqual,
    tOp_lessEqual,

    // shift
    tOp_bitLMove,
    tOp_bitRMove,

    // additive
    tOp_add,
    tOp_sub,

    // multiplicative
    tOp_mul,
    tOp_div,
    tOp_mod,

    // unary
    tOp_not,
    tOp_bitNot,

    // parenthesis
    tOp_Lparen,
    tOp_Rparen,

    tVariable,

    tLit_int,
    tLit_bool,
    tLit_string,

    tCondition_if,
    tCondition_elif,
    tCondition_else,
    tCondition_endif,

    tEnter,
    tEOF,
    tError,
};

class Token
{
public:
    TokenType type;

    std::string value;

    Token();

    Token(TokenType type, const std::string& value);

    void print() const;
};

class Lexer
{
    const std::string& m_source;

    size_t m_pos;
    char   m_currentChar;

public:
    explicit Lexer(const std::string& input);

    std::vector<Token> tokenize();

private:
    Token next();
    void  skipSpace();
    char  peek(int n = 1) const;
    void  advance(int n = 1);
    bool  match(const std::string& pattern);

    Token literal_int();
    Token literal_string();
    Token variable();
};

} // namespace pps
