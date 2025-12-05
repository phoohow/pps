#pragma once

#include <frontend/parser.h>

#include <variant>
#include <unordered_map>

namespace pps
{
enum class ValueType
{
    tNull,
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

struct NullValue : Value
{
    NullValue() :
        Value(ValueType::tNull, false) {}
    void print() const override {}
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
    std::unordered_map<std::string, bool>*        m_in_bools;
    std::unordered_map<std::string, int>*         m_in_ints;
    std::unordered_map<std::string, std::string>* m_in_strs;

    std::unordered_map<std::string, bool>        m_var_bools;
    std::unordered_map<std::string, int>         m_var_ints;
    std::unordered_map<std::string, std::string> m_var_strs;

public:
    explicit Evaluator(std::unordered_map<std::string, bool>*        b = nullptr,
                       std::unordered_map<std::string, int>*         i = nullptr,
                       std::unordered_map<std::string, std::string>* s = nullptr);

    std::unique_ptr<Value> evaluate(const Node* node);

private:
    std::unique_ptr<Value> _visit(const Node* node);
    std::unique_ptr<Value> _visit_binary_op(const BinaryOpNode* node);
    std::unique_ptr<Value> _visit_unary_op(const UnaryOpNode* node);
    std::unique_ptr<Value> _visit_variable(const VariableNode* node);
    std::unique_ptr<Value> _visit_lit_int(const LitIntNode* node);
    std::unique_ptr<Value> _visit_lit_bool(const LitBoolNode* node);
    std::unique_ptr<Value> _visit_lit_str(const LitStringNode* node);
    std::unique_ptr<Value> _visit_stmt_declaration(const StmtDeclarationNode* node);
    std::unique_ptr<Value> _visit_stmt_assignment(const StmtAssignmentNode* node);
    std::unique_ptr<Value> _visit_stmt_condition(const StmtConditionNode* node);
    std::unique_ptr<Value> _visit_stmt_compound(const StmtCompoundNode* node);
};

} // namespace pps
