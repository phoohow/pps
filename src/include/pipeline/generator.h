#pragma once

#include <frontend/parser.h>

#include <string>
#include <memory>

namespace pps
{

class ExprGenerator
{
public:
    std::string generate(const Node* node);

private:
    std::string _gen_variable_node(const VariableNode* node);
    std::string _gen_binary_op_node(const BinaryOpNode* node);
    std::string _gen_unary_op_node(const UnaryOpNode* node);
};

} // namespace pps
