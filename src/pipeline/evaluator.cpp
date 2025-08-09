#include <pipeline/evaluator.h>

#include <pps/pps.h>

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

Evaluator::Evaluator(DefineCTX* define) :
    m_define(define) {}

std::unique_ptr<Value> Evaluator::evaluate(const Node* node)
{
    return visit(node);
}

std::unique_ptr<Value> Evaluator::visit(const Node* node)
{
    switch (node->type())
    {
        case NodeType::tLit_int:
            return visitLitInt(static_cast<const LitIntNode*>(node));
        case NodeType::tLit_bool:
            return visitLitBool(static_cast<const LitBoolNode*>(node));
        case NodeType::tLit_string:
            return visitLitString(static_cast<const LitStringNode*>(node));
        case NodeType::tVariable:
            return visitVariable(static_cast<const VariableNode*>(node));
        case NodeType::tOp_binary:
            return visitBinaryOp(static_cast<const BinaryOpNode*>(node));
        case NodeType::tOp_unary:
            return visitUnaryOp(static_cast<const UnaryOpNode*>(node));
        case NodeType::tStmt_declaration:
            return visitStmtDeclaration(static_cast<const StmtDeclarationNode*>(node));
        case NodeType::tStmt_assignment:
            return visitStmtAssignment(static_cast<const StmtAssignmentNode*>(node));
        case NodeType::tStmt_condition:
            return visitStmtCondition(static_cast<const StmtConditionNode*>(node));
        case NodeType::tStmt_compound:
            return visitStmtCompound(static_cast<const StmtCompoundNode*>(node));
        default:
            throw std::runtime_error("Unknown node type");
    }
}

std::unique_ptr<Value> Evaluator::visitBinaryOp(const BinaryOpNode* node)
{
    auto left  = visit(node->left.get());
    auto right = visit(node->right.get());

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
        throw std::runtime_error("Unknown binary operator: " + node->op.value);
    }
    else if (left->type == ValueType::tBool)
    {
        const BoolValue& leftBool  = static_cast<const BoolValue&>(*left);
        const BoolValue& rightBool = static_cast<const BoolValue&>(*right);

        switch (node->op.type)
        {
            case TokenType::tOp_and:
                return std::make_unique<BoolValue>(leftBool && rightBool);
            case TokenType::tOp_or:
                return std::make_unique<BoolValue>(leftBool || rightBool);
        }
        throw std::runtime_error("Unknown binary operator: " + node->op.value);
    }
    else if (left->type == ValueType::tString)
    {
        const StringValue& leftString = static_cast<const StringValue&>(*left);

        switch (node->op.type)
        {
            case TokenType::tOp_add:
                if (right->type == ValueType::tString)
                    return std::make_unique<StringValue>(leftString + static_cast<const StringValue&>(*right));
                else if (right->type == ValueType::tInt)
                    return std::make_unique<StringValue>(leftString + static_cast<const IntValue&>(*right));
            case TokenType::tOp_sub:
                if (right->type == ValueType::tString)
                    return std::make_unique<StringValue>(leftString - static_cast<const StringValue&>(*right));
                else if (right->type == ValueType::tString)
                    return std::make_unique<StringValue>(leftString - static_cast<const StringValue&>(*right));
            case TokenType::tOp_mul:
                if (right->type == ValueType::tInt)
                    return std::make_unique<StringValue>(leftString * static_cast<const IntValue&>(*right));
            case TokenType::tOp_bitLMove:
                if (right->type == ValueType::tInt)
                    return std::make_unique<StringValue>(leftString << static_cast<const IntValue&>(*right));
            case TokenType::tOp_bitRMove:
                if (right->type == ValueType::tInt)
                    return std::make_unique<StringValue>(leftString >> static_cast<const IntValue&>(*right));
        }
        throw std::runtime_error("Unknown binary operator: " + node->op.value);
    }

    return nullptr;
}

std::unique_ptr<Value> Evaluator::visitUnaryOp(const UnaryOpNode* node)
{
    auto val = visit(node->child.get());
    if (node->op.type == TokenType::tOp_not)
        return std::make_unique<BoolValue>(!val);
    throw std::runtime_error("Unknown unary operator: " + node->op.value);
}

std::unique_ptr<Value> Evaluator::visitVariable(const VariableNode* node)
{
    if (m_intVars.find(node->name) != m_intVars.end())
        return std::make_unique<IntValue>(m_intVars[node->name]);
    else if (m_boolVars.find(node->name) != m_boolVars.end())
        return std::make_unique<BoolValue>(m_boolVars[node->name]);
    else if (m_stringVars.find(node->name) != m_stringVars.end())
        return std::make_unique<StringValue>(m_stringVars[node->name]);

    if (m_define)
    {
        if (m_define->bools.find(node->name) != m_define->bools.end())
            return std::make_unique<BoolValue>(m_define->bools[node->name]);
        if (m_define->ints.find(node->name) != m_define->ints.end())
            return std::make_unique<IntValue>(m_define->ints[node->name]);
        if (m_define->strings.find(node->name) != m_define->strings.end())
            return std::make_unique<StringValue>(m_define->strings[node->name]);
    }

    std::cout << "Undefined variable: " + node->name << std::endl;
    return std::make_unique<IntValue>(0);
}

std::unique_ptr<Value> Evaluator::visitLitInt(const LitIntNode* node)
{
    return std::make_unique<IntValue>(node->value);
}

std::unique_ptr<Value> Evaluator::visitLitBool(const LitBoolNode* node)
{
    return std::make_unique<BoolValue>(node->value);
}

std::unique_ptr<Value> Evaluator::visitLitString(const LitStringNode* node)
{
    return std::make_unique<StringValue>(node->value);
}

std::unique_ptr<Value> Evaluator::visitStmtDeclaration(const StmtDeclarationNode* node)
{
    auto valueNode = node->value.get();
    auto value     = evaluate(valueNode);
    switch (node->varType.type)
    {
        case TokenType::tType_int:
            m_intVars[node->varName] = std::get<int>(value->value);
            break;
        case TokenType::tType_bool:
            m_boolVars[node->varName] = std::get<bool>(value->value);
            break;
        case TokenType::tType_string:
            m_stringVars[node->varName] = std::get<std::string>(value->value);
            break;
        default:
            throw std::runtime_error("Invalid variable type: " + node->varType.value);
    }

    return value;
}

std::unique_ptr<Value> Evaluator::visitStmtAssignment(const StmtAssignmentNode* node)
{
    std::unique_ptr<Value> value = evaluate(node->value.get());

    if (m_intVars.find(node->name) != m_intVars.end())
    {
        m_intVars[node->name] = std::get<int>(value->value);
    }
    else if (m_boolVars.find(node->name) != m_boolVars.end())
    {
        m_boolVars[node->name] = std::get<bool>(value->value);
    }
    else if (m_stringVars.find(node->name) != m_stringVars.end())
    {
        m_stringVars[node->name] = std::get<std::string>(value->value);
    }
    else
    {
        throw std::runtime_error("Undefined variable: " + node->name);
    }

    return value;
}

std::unique_ptr<Value> Evaluator::visitStmtCondition(const StmtConditionNode* node)
{
    for (const auto& branch : node->branches)
    {
        if (evaluate(branch.condition.get()))
            return evaluate(branch.block.get());
    }
    if (node->elseBlock)
        return evaluate(node->elseBlock.get());

    return std::make_unique<IntValue>(0);
}

std::unique_ptr<Value> Evaluator::visitStmtCompound(const StmtCompoundNode* node)
{
    std::vector<std::unique_ptr<Value>> results;
    for (auto& stmt : node->statements)
    {
        results.push_back(evaluate(stmt.get()));
    }

    return node->statements.empty() ? std::make_unique<BoolValue>(false) : std::move(results.back());
}

} // namespace pps
