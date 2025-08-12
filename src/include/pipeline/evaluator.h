#pragma once

#include <frontend/parser.h>

#include <variant>
#include <unordered_map>

namespace pps
{
enum class ValueType
{
    tBool,
    tInt,
    tString,
};

struct Value
{
    ValueType                            type;
    std::variant<bool, int, std::string> value;

    Value(ValueType _type, std::variant<bool, int, std::string> _value) :
        type(_type), value(_value) {}
    virtual ~Value()           = default;
    virtual void print() const = 0;
};

struct BoolValue : Value
{
    BoolValue(bool _value) :
        Value(ValueType::tBool, _value) {}
    void print() const override;

    BoolValue operator&&(const BoolValue& other) const;
    BoolValue operator||(const BoolValue& other) const;
    BoolValue operator!(void) const;
    BoolValue operator==(const BoolValue& other) const;
    BoolValue operator!=(const BoolValue& other) const;
};

struct IntValue : Value
{
    IntValue(int _value) :
        Value(ValueType::tInt, _value) {}
    void print() const override;

    IntValue operator+(const IntValue& other) const;
    IntValue operator-(const IntValue& other) const;
    IntValue operator*(const IntValue& other) const;
    IntValue operator/(const IntValue& other) const;
    IntValue operator%(const IntValue& other) const;
    IntValue operator<<(const IntValue& other) const;
    IntValue operator>>(const IntValue& other) const;
    IntValue operator&(const IntValue& other) const;
    IntValue operator|(const IntValue& other) const;
    IntValue operator^(const IntValue& other) const;
    IntValue operator~() const;

    BoolValue operator>(const IntValue& other) const;
    BoolValue operator<(const IntValue& other) const;
    BoolValue operator>=(const IntValue& other) const;
    BoolValue operator<=(const IntValue& other) const;
    BoolValue operator==(const IntValue& other) const;
    BoolValue operator!=(const IntValue& other) const;
};

struct StringValue : Value
{
    StringValue(const std::string& _value) :
        Value(ValueType::tString, _value) {}
    StringValue(const char _char) :
        Value(ValueType::tString, std::string(1, _char)) {}
    void print() const override;

    StringValue operator+(const StringValue& other) const;
    StringValue operator-(const StringValue& other) const;
    StringValue operator+(const IntValue& other) const;
    StringValue operator>>(const IntValue& other) const;
    StringValue operator<<(const IntValue& other) const;
    StringValue operator*(const IntValue& other) const;
    StringValue operator[](const IntValue& other) const;

    BoolValue operator==(const StringValue& other) const;
    BoolValue operator!=(const StringValue& other) const;
};

struct DefineCTX;
class Evaluator
{
    std::unordered_map<std::string, bool>*        m_iBools;
    std::unordered_map<std::string, int>*         m_iInts;
    std::unordered_map<std::string, std::string>* m_iStrings;

    std::unordered_map<std::string, bool>        m_boolVars;
    std::unordered_map<std::string, int>         m_intVars;
    std::unordered_map<std::string, std::string> m_stringVars;

public:
    explicit Evaluator(std::unordered_map<std::string, bool>*, std::unordered_map<std::string, int>*, std::unordered_map<std::string, std::string>*);

    std::unique_ptr<Value> evaluate(const Node* node);

private:
    std::unique_ptr<Value> visit(const Node* node);
    std::unique_ptr<Value> visitBinaryOp(const BinaryOpNode* node);
    std::unique_ptr<Value> visitUnaryOp(const UnaryOpNode* node);
    std::unique_ptr<Value> visitVariable(const VariableNode* node);
    std::unique_ptr<Value> visitLitInt(const LitIntNode* node);
    std::unique_ptr<Value> visitLitBool(const LitBoolNode* node);
    std::unique_ptr<Value> visitLitString(const LitStringNode* node);
    std::unique_ptr<Value> visitStmtDeclaration(const StmtDeclarationNode* node);
    std::unique_ptr<Value> visitStmtAssignment(const StmtAssignmentNode* node);
    std::unique_ptr<Value> visitStmtCondition(const StmtConditionNode* node);
    std::unique_ptr<Value> visitStmtCompound(const StmtCompoundNode* node);
};

} // namespace pps
