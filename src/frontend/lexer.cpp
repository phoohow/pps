#include <frontend/lexer.h>

#include <magic_enum/magic_enum.hpp>

#include <iostream>
#include <cctype>

namespace pps
{

    Token::Token()
        : type(TokenType::tEnter), value("") {}

    Token::Token(TokenType _type, const std::string &_value)
        : type(_type), value(_value) {}

    void Token::print() const
    {
        std::cout << "Token Type: " << magic_enum::enum_name(type)
                  << ", Value: " << value << "\n";
    }

    Lexer::Lexer(const std::string &input)
        : m_source(input), m_pos(0), m_currentChar(input[m_pos]) {}

    std::vector<Token> Lexer::tokenize()
    {
        std::vector<Token> tokens;
        while (m_currentChar != '\0')
        {
            tokens.push_back(next());
        }

        return std::move(tokens);
    }

    Token Lexer::next()
    {
        while (m_currentChar != '\0')
        {
            switch (m_currentChar)
            {
            case ' ':
            case '\t':
            {
                skipSpace();
                continue;
            }
            case '\n':
                advance();
                return Token(TokenType::tEnter, "enter");
            case '(':
            {
                advance();
                return Token(TokenType::tOp_Lparen, "(");
            }
            case ')':
            {
                advance();
                return Token(TokenType::tOp_Rparen, ")");
            }
            case '+':
            {
                advance();
                return Token(TokenType::tOp_add, "+");
            }
            case '-':
            {
                advance();
                return Token(TokenType::tOp_sub, "-");
            }
            case '*':
            {
                advance();
                return Token(TokenType::tOp_mul, "*");
            }
            case '/':
            {
                advance();
                return Token(TokenType::tOp_div, "/");
            }
            case '%':
            {
                advance();
                return Token(TokenType::tOp_mod, "%");
            }
            case '^':
            {
                advance();
                return Token(TokenType::tOp_bitXor, "^");
            }
            case '~':
            {
                advance();
                return Token(TokenType::tOp_bitNot, "~");
            }
            case '>':
            {
                if (match("> "))
                {
                    advance(2);
                    return Token(TokenType::tOp_greater, ">");
                }
                else if (match(">="))
                {
                    advance(2);
                    return Token(TokenType::tOp_greaterEqual, ">=");
                }
                else if (match(">>"))
                {
                    advance(2);
                    return Token(TokenType::tOp_bitRMove, ">>");
                }
            }
            case '<':
            {
                if (match("< "))
                {
                    advance(2);
                    return Token(TokenType::tOp_less, "<");
                }
                else if (match("<="))
                {
                    advance(2);
                    return Token(TokenType::tOp_lessEqual, "<=");
                }
                else if (match("<<"))
                {
                    advance(2);
                    return Token(TokenType::tOp_bitLMove, "<<");
                }
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
                return literal_int();
            }
            case '@':
            {
                return variable();
            }
            case '"':
            {
                return literal_string();
            }
            case '&':
            {
                if (match("& "))
                {
                    advance(2);
                    return Token(TokenType::tOp_bitAnd, "&");
                }
                else if (match("&&"))
                {
                    advance(2);
                    return Token(TokenType::tOp_and, "&&");
                }
            }
            case '|':
            {
                if (match("| "))
                {
                    advance(2);
                    return Token(TokenType::tOp_bitOr, "|");
                }
                else if (match("||"))
                {
                    advance(2);
                    return Token(TokenType::tOp_or, "||");
                }
            }
            case '!':
            {
                if (match("!@"))
                {
                    advance();
                    return Token(TokenType::tOp_not, "!");
                }
                else if (match("!= "))
                {
                    advance(3);
                    return Token(TokenType::tOp_unequal, "!=");
                }
            }
            case '=':
            {
                if (match("= "))
                {
                    advance(2);
                    return Token(TokenType::tOp_assign, "=");
                }
                else if (match("== "))
                {
                    advance(3);
                    return Token(TokenType::tOp_equal, "==");
                }
            }
            case 'b':
                if (match("bool "))
                {
                    advance(5);
                    return Token(TokenType::tType_bool, "bool");
                }
            case 'e':
            {
                if (match("elif "))
                {
                    advance(5);
                    return Token(TokenType::tCondition_elif, "elif");
                }
                else if (match("else"))
                {
                    advance(4);
                    return Token(TokenType::tCondition_else, "else");
                }
                else if (match("endif"))
                {
                    advance(5);
                    return Token(TokenType::tCondition_endif, "endif");
                }
            case 'i':
                if (match("if "))
                {
                    advance(3);
                    return Token(TokenType::tCondition_if, "if");
                }
                else if (match("int "))
                {
                    advance(4);
                    return Token(TokenType::tType_int, "int");
                }
            case 's':
                if (match("string "))
                {
                    advance(7);
                    return Token(TokenType::tType_string, "string");
                }
            case 't':
                if (match("true"))
                {
                    advance(4);
                    return Token(TokenType::tLit_bool, "true");
                }
            case 'f':
                if (match("false"))
                {
                    advance(5);
                    return Token(TokenType::tLit_bool, "false");
                }

            default:
            {
            }
            }
            }

            return Token(TokenType::tError, std::string(1, m_currentChar));
        }
        return Token(TokenType::tEOF, "");
    }

    void Lexer::skipSpace()
    {
        while (m_currentChar != '\0' && std::isspace(m_currentChar) && m_currentChar != '\n')
        {
            advance();
        }
    }

    Token Lexer::variable()
    {
        std::string result;
        result += m_currentChar;
        advance();
        while (m_currentChar != '\0' && (std::isalnum(m_currentChar) || m_currentChar == '_'))
        {
            result += m_currentChar;
            advance();
        }
        return Token(TokenType::tVariable, result);
    }

    Token Lexer::literal_int()
    {
        std::string result;
        bool hasDecimal = false;

        while (m_currentChar != '\0' && (std::isdigit(m_currentChar) || m_currentChar == '.'))
        {
            if (m_currentChar == '.')
            {
                if (hasDecimal)
                    throw std::runtime_error("Invalid number: multiple decimal points");
                hasDecimal = true;
            }
            result += m_currentChar;
            advance();
        }

        return Token(TokenType::tLit_int, result);
    }

    Token Lexer::literal_string()
    {
        advance();
        std::string result;
        while (m_currentChar != '\0' && m_currentChar != '"')
        {
            result += m_currentChar;
            advance();
        }
        if (m_currentChar != '"')
            throw std::runtime_error("Unterminated string literal");
        advance();
        return Token(TokenType::tLit_string, result);
    }

    char Lexer::peek(int n) const
    {
        if (m_pos + n >= m_source.length())
            return '\0';
        return m_source[m_pos + n];
    }

    void Lexer::advance(int n)
    {
        m_pos += n;
        if (m_pos < m_source.length())
            m_currentChar = m_source[m_pos];
        else
            m_currentChar = '\0';
    }

    bool Lexer::match(const std::string &pattern)
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
