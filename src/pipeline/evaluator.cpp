#include <pipeline/evaluator.h>

#include <pps/pps.h>

#include <magic_enum/magic_enum.hpp>

#include <aclg/aclg.h>

#include <stdexcept>
#include <iostream>

namespace pps
{

void BoolValue::print() const
{
    std::cout << (std::get<bool>(value) ? "true" : "false");
}
BoolValue BoolValue::operator&&(const BoolValue& other) const
{
    return BoolValue(std::get<bool>(value) && std::get<bool>(other.value));
}
BoolValue BoolValue::operator||(const BoolValue& other) const
{
    return BoolValue(std::get<bool>(value) || std::get<bool>(other.value));
}
BoolValue BoolValue::operator!(void) const
{
    return BoolValue(!std::get<bool>(value));
}

void IntValue::print() const
{
    std::cout << std::get<int>(value);
}
IntValue IntValue::operator+(const IntValue& other) const
{
    return IntValue(std::get<int>(value) + std::get<int>(other.value));
}
IntValue IntValue::operator-(const IntValue& other) const
{
    return IntValue(std::get<int>(value) - std::get<int>(other.value));
}
IntValue IntValue::operator*(const IntValue& other) const
{
    return IntValue(std::get<int>(value) * std::get<int>(other.value));
}
IntValue IntValue::operator/(const IntValue& other) const
{
    return IntValue(std::get<int>(value) / std::get<int>(other.value));
}
IntValue IntValue::operator%(const IntValue& other) const
{
    return IntValue(std::get<int>(value) % std::get<int>(other.value));
}
IntValue IntValue::operator<<(const IntValue& other) const
{
    return IntValue(std::get<int>(value) << std::get<int>(other.value));
}
IntValue IntValue::operator>>(const IntValue& other) const
{
    return IntValue(std::get<int>(value) >> std::get<int>(other.value));
}
IntValue IntValue::operator&(const IntValue& other) const
{
    return IntValue(std::get<int>(value) & std::get<int>(other.value));
}
IntValue IntValue::operator|(const IntValue& other) const
{
    return IntValue(std::get<int>(value) | std::get<int>(other.value));
}
IntValue IntValue::operator^(const IntValue& other) const
{
    return IntValue(std::get<int>(value) ^ std::get<int>(other.value));
}
IntValue IntValue::operator~() const
{
    return IntValue(~std::get<int>(value));
}
BoolValue IntValue::operator>(const IntValue& other) const
{
    return BoolValue(std::get<int>(value) > std::get<int>(other.value));
}
BoolValue IntValue::operator<(const IntValue& other) const
{
    return BoolValue(std::get<int>(value) < std::get<int>(other.value));
}
BoolValue IntValue::operator>=(const IntValue& other) const
{
    return BoolValue(std::get<int>(value) >= std::get<int>(other.value));
}
BoolValue IntValue::operator<=(const IntValue& other) const
{
    return BoolValue(std::get<int>(value) <= std::get<int>(other.value));
}
BoolValue IntValue::operator==(const IntValue& other) const
{
    return BoolValue(std::get<int>(value) == std::get<int>(other.value));
}
BoolValue IntValue::operator!=(const IntValue& other) const
{
    return BoolValue(std::get<int>(value) != std::get<int>(other.value));
}

void StringValue::print() const
{
    std::cout << std::get<std::string>(value);
}
StringValue StringValue::operator+(const StringValue& other) const
{
    return StringValue(std::get<std::string>(value) + std::get<std::string>(other.value));
}
StringValue StringValue::operator-(const StringValue& other) const
{
    auto pos = std::get<std::string>(value).find(std::get<std::string>(other.value));
    if (pos != std::string::npos)
    {
        return StringValue(std::get<std::string>(value).substr(pos));
    }
    return *this;
}
StringValue StringValue::operator+(const IntValue& other) const
{
    return StringValue(std::get<std::string>(value) + std::to_string(std::get<int>(other.value)));
}
StringValue StringValue::operator>>(const IntValue& other) const
{
    return StringValue(std::get<std::string>(value).substr(std::get<int>(other.value)));
}
StringValue StringValue::operator<<(const IntValue& other) const
{
    return StringValue(std::get<std::string>(value).substr(0, std::get<int>(other.value)));
}
StringValue StringValue::operator*(const IntValue& other) const
{
    auto        repeat = std::get<int>(other.value);
    std::string result;
    for (int i = 0; i < repeat; ++i)
    {
        result += std::get<std::string>(value);
    }
    return StringValue(result);
}
StringValue StringValue::operator[](const IntValue& other) const
{
    return StringValue(std::get<std::string>(value)[std::get<int>(other.value)]);
}
BoolValue StringValue::operator==(const StringValue& other) const
{
    return BoolValue(value == other.value);
}
BoolValue StringValue::operator!=(const StringValue& other) const
{
    return BoolValue(value != other.value);
}

Evaluator::Evaluator(std::unordered_map<std::string, bool>* b, std::unordered_map<std::string, int>* i, std::unordered_map<std::string, std::string>* s) :
    m_in_bools(b), m_in_ints(i), m_in_strs(s) {}

std::unique_ptr<Value> Evaluator::evaluate(const Node* node)
{
    return _visit(node);
}

std::unique_ptr<Value> Evaluator::_visit(const Node* node)
{
    if (!node) return std::make_unique<NullValue>();

    switch (node->type())
    {
        case NodeType::tLit_int:
            return _visit_lit_int(static_cast<const LitIntNode*>(node));
        case NodeType::tLit_bool:
            return _visit_lit_bool(static_cast<const LitBoolNode*>(node));
        case NodeType::tLit_string:
            return _visit_lit_str(static_cast<const LitStringNode*>(node));
        case NodeType::tVariable:
            return _visit_variable(static_cast<const VariableNode*>(node));
        case NodeType::tOp_binary:
            return _visit_binary_op(static_cast<const BinaryOpNode*>(node));
        case NodeType::tOp_unary:
            return _visit_unary_op(static_cast<const UnaryOpNode*>(node));
        case NodeType::tStmt_declaration:
            return _visit_stmt_declaration(static_cast<const StmtDeclarationNode*>(node));
        case NodeType::tStmt_assignment:
            return _visit_stmt_assignment(static_cast<const StmtAssignmentNode*>(node));
        case NodeType::tStmt_condition:
            return _visit_stmt_condition(static_cast<const StmtConditionNode*>(node));
        case NodeType::tStmt_compound:
            return _visit_stmt_compound(static_cast<const StmtCompoundNode*>(node));
        default:
            ACLG_ERROR("Unknown node type");
            return nullptr;
    }
}

std::unique_ptr<Value> Evaluator::_visit_binary_op(const BinaryOpNode* node)
{
    auto left  = _visit(node->left.get());
    auto right = _visit(node->right.get());

    if (left->type == ValueType::tInt)
    {
        auto leftInt  = static_cast<const IntValue&>(*left);
        auto rightInt = static_cast<const IntValue&>(*right);

        switch (node->op.type)
        {
            case TokenType::tOp_add:
                return std::make_unique<IntValue>(leftInt + rightInt);
            case TokenType::tOp_sub:
                return std::make_unique<IntValue>(leftInt - rightInt);
            case TokenType::tOp_mul:
                return std::make_unique<IntValue>(leftInt * rightInt);
            case TokenType::tOp_div:
                return std::make_unique<IntValue>(leftInt / rightInt);
            case TokenType::tOp_mod:
                return std::make_unique<IntValue>(leftInt % rightInt);
            case TokenType::tOp_bitLMove:
                return std::make_unique<IntValue>(leftInt << rightInt);
            case TokenType::tOp_bitRMove:
                return std::make_unique<IntValue>(leftInt >> rightInt);
            case TokenType::tOp_bitAnd:
                return std::make_unique<IntValue>(leftInt & rightInt);
            case TokenType::tOp_bitOr:
                return std::make_unique<IntValue>(leftInt | rightInt);
            case TokenType::tOp_bitXor:
                return std::make_unique<IntValue>(leftInt ^ rightInt);
            case TokenType::tOp_bitNot:
                return std::make_unique<IntValue>(~leftInt);
            case TokenType::tOp_greater:
                return std::make_unique<BoolValue>(leftInt > rightInt);
            case TokenType::tOp_less:
                return std::make_unique<BoolValue>(leftInt < rightInt);
            case TokenType::tOp_greaterEqual:
                return std::make_unique<BoolValue>(leftInt >= rightInt);
            case TokenType::tOp_lessEqual:
                return std::make_unique<BoolValue>(leftInt <= rightInt);
            case TokenType::tOp_equal:
                return std::make_unique<BoolValue>(leftInt == rightInt);
            case TokenType::tOp_unequal:
                return std::make_unique<BoolValue>(leftInt != rightInt);
        }
    }
    else if (left->type == ValueType::tBool)
    {
        const BoolValue& left_bool  = static_cast<const BoolValue&>(*left);
        const BoolValue& right_bool = static_cast<const BoolValue&>(*right);

        switch (node->op.type)
        {
            case TokenType::tOp_and:
                return std::make_unique<BoolValue>(left_bool && right_bool);
            case TokenType::tOp_or:
                return std::make_unique<BoolValue>(left_bool || right_bool);
        }
    }
    else if (left->type == ValueType::tString)
    {
        const StringValue& left_str = static_cast<const StringValue&>(*left);

        switch (node->op.type)
        {
            case TokenType::tOp_add:
                if (right->type == ValueType::tString)
                    return std::make_unique<StringValue>(left_str + static_cast<const StringValue&>(*right));
                else if (right->type == ValueType::tInt)
                    return std::make_unique<StringValue>(left_str + static_cast<const IntValue&>(*right));
            case TokenType::tOp_sub:
                if (right->type == ValueType::tString)
                    return std::make_unique<StringValue>(left_str - static_cast<const StringValue&>(*right));
                else if (right->type == ValueType::tString)
                    return std::make_unique<StringValue>(left_str - static_cast<const StringValue&>(*right));
            case TokenType::tOp_mul:
                if (right->type == ValueType::tInt)
                    return std::make_unique<StringValue>(left_str * static_cast<const IntValue&>(*right));
            case TokenType::tOp_bitLMove:
                if (right->type == ValueType::tInt)
                    return std::make_unique<StringValue>(left_str << static_cast<const IntValue&>(*right));
            case TokenType::tOp_bitRMove:
                if (right->type == ValueType::tInt)
                    return std::make_unique<StringValue>(left_str >> static_cast<const IntValue&>(*right));
        }
    }

    ACLG_ERROR("Unknown binary operator: {}", node->op.value);
    return nullptr;
}

std::unique_ptr<Value> Evaluator::_visit_unary_op(const UnaryOpNode* node)
{
    auto val = _visit(node->child.get());
    if (node->op.type == TokenType::tOp_not)
        return std::make_unique<BoolValue>(!val);

    ACLG_ERROR("Unknown unary operator: {}", node->op.value);
    return nullptr;
}

std::unique_ptr<Value> Evaluator::_visit_variable(const VariableNode* node)
{
    if (m_var_ints.find(node->name) != m_var_ints.end())
        return std::make_unique<IntValue>(m_var_ints[node->name]);
    else if (m_var_bools.find(node->name) != m_var_bools.end())
        return std::make_unique<BoolValue>(m_var_bools[node->name]);
    else if (m_var_strs.find(node->name) != m_var_strs.end())
        return std::make_unique<StringValue>(m_var_strs[node->name]);

    if (m_in_bools)
    {
        auto iter = m_in_bools->find(node->name);
        if (iter != m_in_bools->end())
            return std::make_unique<BoolValue>(iter->second);
    }
    if (m_in_ints)
    {
        auto iter = m_in_ints->find(node->name);
        if (iter != m_in_ints->end())
            return std::make_unique<IntValue>(iter->second);
    }
    if (m_in_strs)
    {
        auto iter = m_in_strs->find(node->name);
        if (iter != m_in_strs->end())
            return std::make_unique<StringValue>(iter->second);
    }

    ACLG_ERROR("Undefined variable: {}, type: {}", node->name, magic_enum::enum_name(node->type()));
    return std::make_unique<IntValue>(0);
}

std::unique_ptr<Value> Evaluator::_visit_lit_int(const LitIntNode* node)
{
    return std::make_unique<IntValue>(node->value);
}

std::unique_ptr<Value> Evaluator::_visit_lit_bool(const LitBoolNode* node)
{
    return std::make_unique<BoolValue>(node->value);
}

std::unique_ptr<Value> Evaluator::_visit_lit_str(const LitStringNode* node)
{
    return std::make_unique<StringValue>(node->value);
}

std::unique_ptr<Value> Evaluator::_visit_stmt_declaration(const StmtDeclarationNode* node)
{
    auto value = evaluate(node->value.get());
    switch (node->var_type.type)
    {
        case TokenType::tType_int:
            m_var_ints[node->name] = std::get<int>(value->value);
            break;
        case TokenType::tType_bool:
            m_var_bools[node->name] = std::get<bool>(value->value);
            break;
        case TokenType::tType_string:
            m_var_strs[node->name] = std::get<std::string>(value->value);
            break;
        default:
            ACLG_ERROR("Invalid variable type: {}", node->var_type.value);
            break;
    }

    return value;
}

std::unique_ptr<Value> Evaluator::_visit_stmt_assignment(const StmtAssignmentNode* node)
{
    std::unique_ptr<Value> value = evaluate(node->value.get());

    if (m_var_ints.find(node->name) != m_var_ints.end())
    {
        m_var_ints[node->name] = std::get<int>(value->value);
    }
    else if (m_var_bools.find(node->name) != m_var_bools.end())
    {
        m_var_bools[node->name] = std::get<bool>(value->value);
    }
    else if (m_var_strs.find(node->name) != m_var_strs.end())
    {
        m_var_strs[node->name] = std::get<std::string>(value->value);
    }
    else
    {
        ACLG_ERROR("Undefined variable: {}, type: {}", node->name, magic_enum::enum_name(node->type()));
    }

    return value;
}

std::unique_ptr<Value> Evaluator::_visit_stmt_condition(const StmtConditionNode* node)
{
    for (const auto& branch : node->branches)
    {
        if (evaluate(branch.condition.get()))
            return evaluate(branch.block.get());
    }
    if (node->else_block)
        return evaluate(node->else_block.get());

    return std::make_unique<IntValue>(0);
}

std::unique_ptr<Value> Evaluator::_visit_stmt_compound(const StmtCompoundNode* node)
{
    std::vector<std::unique_ptr<Value>> results;
    for (auto& stmt : node->statements)
    {
        results.push_back(evaluate(stmt.get()));
    }

    return node->statements.empty() ? std::make_unique<BoolValue>(false) : std::move(results.back());
}

} // namespace pps
