#pragma once

#include <frontend/lexer.h>
#include <frontend/parser.h>
#include <pipeline/evaluator.h>

#include <pps/pps.h>

#include <stack>

namespace pps
{

enum class BranchTag : uint8_t
{
    tIf,
    tElif,
    tElse,
    tEndif,
};

struct StaticBranch
{
    BranchTag type       = BranchTag::tIf;
    bool      choosed_if = false;
    bool      current    = false;
};

struct DynamicBranch
{
    BranchTag   type        = BranchTag::tIf;
    bool        enable_else = false;
    bool        current     = false;
    std::string condition_expr;
};

class Task
{
    Context* m_context;

    sbin::Loader* m_loader      = nullptr;
    std::string   m_decrypt_key = "";

public:
    enum class Type
    {
        tOrigin,
        tMacro,
        tInstance,
        tInclude,
        tOverride,
        tEmbed,
        tProg,
    };

    enum class State
    {
        sKeep,
        sSkip,
    };

    Task();

    void set_ctx(Context* context);
    void set_ctx(Context* context, sbin::Loader* module_loader, const std::string& decrypt_key);

    State process(std::string& line);

private:
    State m_state = State::sKeep;
    Type  m_type  = Type::tOrigin;

    // Branch
private:
    std::stack<std::variant<StaticBranch, DynamicBranch>> m_branch_stack;

    // Prog
private:
    enum class ProgType
    {
        tStatement,
        tEnd,
    };
    std::string m_progSource;

private:
    Type          _extract_task(std::string& line);
    void          _process_state();
    bool          _is_skip();
    bool          _in_miss_branch();
    StaticBranch  _pop_static();
    DynamicBranch _pop_dynamic();

    // Origin
    void _process_origin(std::string& line);

    // Branch
    BranchTag     _extract_branch_tag(std::string& line);
    void          _eval_static_branch(std::string& line);
    DynamicBranch _eval_dynamic_banch(std::string& line);
    void          _process_static_branch(std::string& line);
    std::string   _process_dynamic_branch(std::string& line);
    bool          _has_branch_true(const std::vector<Token>& tokens);
    bool          _is_valid_condition_expr(const Node* node);
    bool          _eval_condition_expr(const std::string& line);
    std::string   _gen_condition_expr(const Node* node);

    // Include
    void        _process_include(std::string& line);
    std::string _extract_include_from_ctx(const std::string& path);
    std::string _extract_include_from_loader(const std::string& path);

    // Override
    void _process_override(std::string& origin, std::string& line);
    void _extract_override_task(std::string& line);

    // Embed
    void _process_embed(std::string& line);
    void _extract_embed_task(std::string& line);

    // Prog
    void     _process_prog(std::string& line);
    void     _eval_prog(std::string& line);
    ProgType _extract_prog_task(std::string& line);
};

} // namespace pps
