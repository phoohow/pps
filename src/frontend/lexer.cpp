#include <frontend/lexer.h>

#include <magic_enum/magic_enum.hpp>

#include <aclg/aclg.h>

#include <iostream>
#include <cctype>

namespace pps
{

Token::Token() :
    type(TokenType::tEnter), value("") {}

Token::Token(TokenType _type, const std::string& _value) :
    type(_type), value(_value) {}

void Token::print() const
{
    std::cout << "Token Type: " << magic_enum::enum_name(type)
              << ", Value: " << value << "\n";
}

Lexer::Lexer(const std::string& input) :
    m_source(input), m_pos(0), m_cur_char(input[m_pos]) {}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    while (m_cur_char != '\0')
    {
        tokens.push_back(_next());
    }

    return std::move(tokens);
}

Token Lexer::_next()
{
    while (m_cur_char != '\0')
    {
        switch (m_cur_char)
        {
            case ' ':
            case '\t':
            {
                _skip_space();
                continue;
            }
            case '\n':
                _advance();
                return Token(TokenType::tEnter, "enter");
            case '(':
            {
                _advance();
                return Token(TokenType::tOp_Lparen, "(");
            }
            case ')':
            {
                _advance();
                return Token(TokenType::tOp_Rparen, ")");
            }
            case '+':
            {
                _advance();
                return Token(TokenType::tOp_add, "+");
            }
            case '-':
            {
                _advance();
                return Token(TokenType::tOp_sub, "-");
            }
            case '*':
            {
                _advance();
                return Token(TokenType::tOp_mul, "*");
            }
            case '/':
            {
                _advance();
                return Token(TokenType::tOp_div, "/");
            }
            case '%':
            {
                _advance();
                return Token(TokenType::tOp_mod, "%");
            }
            case '^':
            {
                _advance();
                return Token(TokenType::tOp_bitXor, "^");
            }
            case '~':
            {
                _advance();
                return Token(TokenType::tOp_bitNot, "~");
            }
            case '>':
            {
                if (_match("> "))
                {
                    _advance(2);
                    return Token(TokenType::tOp_greater, ">");
                }
                else if (_match(">="))
                {
                    _advance(2);
                    return Token(TokenType::tOp_greaterEqual, ">=");
                }
                else if (_match(">>"))
                {
                    _advance(2);
                    return Token(TokenType::tOp_bitRMove, ">>");
                }
                break;
            }
            case '<':
            {
                if (_match("< "))
                {
                    _advance(2);
                    return Token(TokenType::tOp_less, "<");
                }
                else if (_match("<="))
                {
                    _advance(2);
                    return Token(TokenType::tOp_lessEqual, "<=");
                }
                else if (_match("<<"))
                {
                    _advance(2);
                    return Token(TokenType::tOp_bitLMove, "<<");
                }
                break;
            }
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                return _literal_int();
            }
            case '@':
            {
                return _variable();
            }
            case '"':
            {
                return _literal_string();
            }
            case '&':
            {
                if (_match("& "))
                {
                    _advance(2);
                    return Token(TokenType::tOp_bitAnd, "&");
                }
                else if (_match("&&"))
                {
                    _advance(2);
                    return Token(TokenType::tOp_and, "&&");
                }
                break;
            }
            case '|':
            {
                if (_match("| "))
                {
                    _advance(2);
                    return Token(TokenType::tOp_bitOr, "|");
                }
                else if (_match("||"))
                {
                    _advance(2);
                    return Token(TokenType::tOp_or, "||");
                }
                break;
            }
            case '!':
            {
                if (_match("!@"))
                {
                    _advance();
                    return Token(TokenType::tOp_not, "!");
                }
                else if (_match("!= "))
                {
                    _advance(3);
                    return Token(TokenType::tOp_unequal, "!=");
                }
                break;
            }
            case '=':
            {
                if (_match("= "))
                {
                    _advance(2);
                    return Token(TokenType::tOp_assign, "=");
                }
                else if (_match("== "))
                {
                    _advance(3);
                    return Token(TokenType::tOp_equal, "==");
                }
                break;
            }
            case 'b':
                if (_match("bool "))
                {
                    _advance(5);
                    return Token(TokenType::tType_bool, "bool");
                }
                break;
            case 'e':
            {
                if (_match("elif "))
                {
                    _advance(5);
                    return Token(TokenType::tCondition_elif, "elif");
                }
                else if (_match("else"))
                {
                    _advance(4);
                    return Token(TokenType::tCondition_else, "else");
                }
                else if (_match("endif"))
                {
                    _advance(5);
                    return Token(TokenType::tCondition_endif, "endif");
                }
                break;
            }
            case 'i':
                if (_match("if "))
                {
                    _advance(3);
                    return Token(TokenType::tCondition_if, "if");
                }
                else if (_match("int "))
                {
                    _advance(4);
                    return Token(TokenType::tType_int, "int");
                }
                break;
            case 's':
                if (_match("string "))
                {
                    _advance(7);
                    return Token(TokenType::tType_string, "string");
                }
                break;
            case 't':
                if (_match("true"))
                {
                    _advance(4);
                    return Token(TokenType::tLit_bool, "true");
                }
                break;
            case 'f':
                if (_match("false"))
                {
                    _advance(5);
                    return Token(TokenType::tLit_bool, "false");
                }
                break;
            default:
            {
                ACLG_ERROR("Undefined token '{}'(pos {}) in: '{}'", m_cur_char, m_pos, m_source);
            }
        }

        ACLG_ERROR("Undefined token '{}'(pos {}) in: '{}'", m_cur_char, m_pos, m_source);
        return Token(TokenType::tError, std::string(1, m_cur_char));
    }

    return Token(TokenType::tEOF, "");
}


void Lexer::_skip_space()
{
    while (m_cur_char != '\0' && std::isspace(m_cur_char) && m_cur_char != '\n')
    {
        _advance();
    }
}

Token Lexer::_variable()
{
    std::string result;
    result += m_cur_char;
    _advance();
    while (m_cur_char != '\0' && (std::isalnum(m_cur_char) || m_cur_char == '_'))
    {
        result += m_cur_char;
        _advance();
    }
    return Token(TokenType::tVariable, result);
}

Token Lexer::_literal_int()
{
    std::string result;
    bool        hasDecimal = false;

    while (m_cur_char != '\0' && (std::isdigit(m_cur_char) || m_cur_char == '.'))
    {
        if (m_cur_char == '.')
        {
            if (hasDecimal)
            {
                ACLG_ERROR("Invalid number: multiple decimal points");
                throw std::runtime_error("Invalid number: multiple decimal points");
            }
            hasDecimal = true;
        }
        result += m_cur_char;
        _advance();
    }

    return Token(TokenType::tLit_int, result);
}

Token Lexer::_literal_string()
{
    _advance();
    std::string result;
    while (m_cur_char != '\0' && m_cur_char != '"')
    {
        result += m_cur_char;
        _advance();
    }
    if (m_cur_char != '"')
    {
        ACLG_ERROR("Unterminated string literal");
        throw std::runtime_error("Unterminated string literal");
    }

    _advance();
    return Token(TokenType::tLit_string, result);
}

char Lexer::_peek(int n) const
{
    if (m_pos + n >= m_source.length())
        return '\0';
    return m_source[m_pos + n];
}

void Lexer::_advance(int n)
{
    m_pos += n;
    if (m_pos < m_source.length())
        m_cur_char = m_source[m_pos];
    else
        m_cur_char = '\0';
}

bool Lexer::_match(const std::string& pattern)
{
    if (m_pos + pattern.length() > m_source.length())
        return false;

    for (size_t i = 0; i < pattern.length(); ++i)
    {
        if (m_source[m_pos + i] != pattern[i])
            return false;
    }
    return true;
}

} // namespace pps
