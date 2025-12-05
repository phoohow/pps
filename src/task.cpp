#include <task.h>
#include <pipeline/simplifier.h>
#include <pipeline/generator.h>

#include <sbin/loader.h>

#include <aclg/aclg.h>

#include <regex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>

namespace pps
{

static auto g_task = std::regex(R"(\*<\$([^>]*)>\*)");

static auto g_task_static   = std::regex(R"(static (.+))");
static auto g_task_dynamic  = std::regex(R"(dynamic (.+))");
static auto g_task_include  = std::regex(R"(include (.+))");
static auto g_task_override = std::regex(R"(override (.+))");
static auto g_task_embed    = std::regex(R"(embed .+)");
static auto g_task_prog     = std::regex(R"(prog .+)");

static auto g_branch_if    = std::regex(R"(if (.+))");
static auto g_branch_elif  = std::regex(R"(elif (.+))");
static auto g_branch_else  = std::regex(R"(else)");
static auto g_branch_endif = std::regex(R"(endif)");

Task::Task()
{
}

void Task::set_ctx(Context* context)
{
    m_context = context;
}

void Task::set_ctx(Context* context, sbin::Loader* module_loader, const std::string& decrypt_key)
{
    m_context     = context;
    m_loader      = module_loader;
    m_decrypt_key = decrypt_key;
}

Task::State Task::process(std::string& line)
{
    auto origin_line = line;
    m_type           = _extract_task(line);

    switch (m_type)
    {
        case Type::tOrigin:
            _process_origin(line);
            break;
        case Type::tMacro:
            _process_static_branch(line);
            _process_state();
            break;
        case Type::tInstance:
            line = _process_dynamic_branch(line);
            _process_state();
            break;
        case Type::tInclude:
            _process_include(line);
            break;
        case Type::tOverride:
            _process_override(origin_line, line);
            break;
        case Type::tEmbed:
            _process_embed(line);
            break;
        case Type::tProg:
            _process_prog(line);
            break;
    }

    return m_state;
}

void Task::_process_origin(std::string& line)
{
    if (_is_skip())
        line = "";
}

void Task::_eval_static_branch(std::string& line)
{
    StaticBranch state;
    state.type = _extract_branch_tag(line);

    switch (state.type)
    {
        case BranchTag::tIf:
        {
            if (_in_miss_branch())
            {
                state.current = false;
                m_branch_stack.push(state);
                break;
            }

            state.current    = _eval_condition_expr(line);
            state.choosed_if = state.current;
            m_branch_stack.push(state);
            break;
        }
        case BranchTag::tElif:
        {
            auto brother = _pop_static();
            if (_in_miss_branch())
            {
                state.current = false;
                m_branch_stack.push(state);
                break;
            }

            if (brother.choosed_if)
            {
                state.choosed_if = true;
                state.current    = false;
            }
            else
            {
                state.current    = _eval_condition_expr(line);
                state.choosed_if = state.current;
            }

            m_branch_stack.push(state);
            break;
        }
        case BranchTag::tElse:
        {
            auto brother = _pop_static();
            if (_in_miss_branch())
            {
                state.current = false;
                m_branch_stack.push(state);
                break;
            }

            state.current = !brother.choosed_if;
            m_branch_stack.push(state);
            break;
        }
        case BranchTag::tEndif:
        {
            m_branch_stack.pop();
            break;
        }
    }
}

DynamicBranch Task::_eval_dynamic_banch(std::string& line)
{
    DynamicBranch state;
    state.type = _extract_branch_tag(line);

    switch (state.type)
    {
        case BranchTag::tIf:
        {
            if (_in_miss_branch())
            {
                state.current = false;
                m_branch_stack.push(state);
                break;
            }

            Lexer          lexer(line);
            auto           tokens = lexer.tokenize();
            Parser         parser(tokens);
            auto           root = parser.parse();
            ExprSimplifier simplifier(m_context->instances);
            auto           simplifiedNode = simplifier.simplify(root.get());

            state.current = _is_valid_condition_expr(simplifiedNode.get());
            if (state.current)
                state.condition_expr = _gen_condition_expr(simplifiedNode.get());

            state.enable_else = state.current;

            m_branch_stack.push(state);
            break;
        }
        case BranchTag::tElif:
        {
            auto brother = _pop_dynamic();
            if (_in_miss_branch())
            {
                state.current = false;
                m_branch_stack.push(state);
                break;
            }

            if (!brother.enable_else)
                state.type = BranchTag::tIf;

            Lexer          lexer(line);
            auto           tokens = lexer.tokenize();
            Parser         parser(tokens);
            auto           root = parser.parse();
            ExprSimplifier simplifier(m_context->instances);
            auto           simplifiedNode = simplifier.simplify(root.get());

            state.current = _is_valid_condition_expr(simplifiedNode.get());
            if (state.current)
                state.condition_expr = _gen_condition_expr(simplifiedNode.get());

            state.enable_else = brother.enable_else || state.current;

            m_branch_stack.push(state);
            break;
        }
        case BranchTag::tElse:
        {
            auto brother = _pop_dynamic();
            if (_in_miss_branch())
            {
                state.current = false;
                m_branch_stack.push(state);
                break;
            }

            state.current = brother.enable_else;
            m_branch_stack.push(state);
            break;
        }
        case BranchTag::tEndif:
        {
            m_branch_stack.pop();
            break;
        }
        default:
            break;
    }

    return state;
}

void Task::_process_static_branch(std::string& line)
{
    _eval_static_branch(line);
    line = "";
}

std::string Task::_process_dynamic_branch(std::string& line)
{
    auto state = _eval_dynamic_banch(line);
    if (!state.current)
        return "";

    std::string expr;
    switch (state.type)
    {
        case BranchTag::tIf:
        {
            expr += "if(" + state.condition_expr + ")";
            break;
        }
        case BranchTag::tElif:
        {
            expr += "elif(" + state.condition_expr + ")";
            break;
        }
        case BranchTag::tElse:
        {
            expr += "else";
            break;
        }
        case BranchTag::tEndif:
        default:
            break;
    }

    return expr;
}

void Task::_process_include(std::string& path)
{
    if (_is_skip())
        return;

    std::string content;
    content += _extract_include_from_ctx(path);
    content += _extract_include_from_loader(path);

    std::string        out;
    std::istringstream iss(content);
    std::string        line;
    uint32_t           line_number = 0;
    while (std::getline(iss, line))
    {
        line_number++;
        process(line);

        if (line.empty())
            continue;
        else if (line.back() != '\n')
            line += "\n";

        out += line;
    }

    path = out;
}

std::string Task::_extract_include_from_ctx(const std::string& path)
{
    std::string oldPath = path;
    for (const auto& prefix : m_context->prefixes)
    {
        std::string fullPath = prefix + oldPath;
        if (!std::filesystem::exists(fullPath))
            continue;

        std::ifstream includeStream(fullPath, std::ios::binary);
        if (!includeStream)
            continue;

        includeStream.seekg(0, std::ios::end);
        std::streamsize size = includeStream.tellg();
        includeStream.seekg(0, std::ios::beg);

        std::string content(size, '\0');
        if (includeStream.read(&content[0], size))
        {
            return std::move(content);
        }
    }

    return "";
}

std::string Task::_extract_include_from_loader(const std::string& path)
{
    if (m_loader == nullptr)
    {
        ACLG_WARN("Loader is not set.");
        return "";
    }

    auto data = m_loader->get_shader(path, m_decrypt_key);
    if (data == nullptr)
    {
        ACLG_ERROR("Fail to load {} from loader.", path);
        return "";
    }

    return std::string(data->data.begin(), data->data.end());
}

void Task::_process_override(std::string& origin, std::string& expr)
{
    if (_is_skip())
        return;

    auto iter = m_context->strings.find(expr);
    if (iter == m_context->strings.end())
    {
        ACLG_ERROR("Fail to find {} in context.", expr);
        expr = "";
        return;
    }

    static std::regex token_override_expr(R"((\w+)\s*(/\*<\$override[^>]*>\*/))");
    expr = std::regex_replace(origin, token_override_expr, iter->second);
}

void Task::_process_embed(std::string& expr)
{
    if (_is_skip())
        return;
}

void Task::_process_prog(std::string& expr)
{
    if (_is_skip())
        return;
}

Task::Type Task::_extract_task(std::string& line)
{
    std::smatch match_task;
    if (!std::regex_search(line, match_task, g_task))
        return Type::tOrigin;

    auto        task = match_task[1].str();
    std::smatch match_type;
    if (std::regex_search(task, match_type, g_task_static))
    {
        line = match_type[1].str();
        return Type::tMacro;
    }
    if (std::regex_search(task, match_type, g_task_dynamic))
    {
        line = match_type[1].str();
        return m_context->isStatic ? Type::tMacro : Type::tInstance;
    }
    else if (std::regex_search(task, match_type, g_task_include))
    {
        line = match_type[1].str();
        return Type::tInclude;
    }
    else if (std::regex_search(task, match_type, g_task_override))
    {
        line = match_type[1].str();
        return Type::tOverride;
    }
    else if (std::regex_search(task, match_type, g_task_embed))
    {
        line = match_type[0].str();
        return Type::tEmbed;
    }
    else if (std::regex_search(task, match_type, g_task_prog))
    {
        line = match_type[0].str();
        return Type::tProg;
    }

    return Type::tOrigin;
}

BranchTag Task::_extract_branch_tag(std::string& line)
{
    std::smatch match_tag;
    if (std::regex_search(line, match_tag, g_branch_elif))
    {
        line = match_tag[1].str();
        return BranchTag::tElif;
    }
    else if (std::regex_search(line, match_tag, g_branch_if))
    {
        line = match_tag[1].str();
        return BranchTag::tIf;
    }
    else if (std::regex_search(line, match_tag, g_branch_else))
    {
        line = match_tag[0].str();
        return BranchTag::tElse;
    }
    else if (std::regex_search(line, match_tag, g_branch_endif))
    {
        line = match_tag[0].str();
        return BranchTag::tEndif;
    }

    return BranchTag::tEndif;
}

bool Task::_has_branch_true(const std::vector<Token>& tokens)
{
    for (int i = 0; i < tokens.size(); i++)
    {
        const auto& token = tokens[i];
        if (token.type != TokenType::tVariable)
            continue;

        auto value = m_context->bools.find(token.value);
        if (value != m_context->bools.end() && value->second)
            return true;
    }

    return false;
}

bool Task::_is_valid_condition_expr(const Node* node)
{
    Evaluator evaluator(&m_context->bools, &m_context->ints, &m_context->strings);
    auto      value = evaluator.evaluate(node);

    return value->type == ValueType::tBool;
}

bool Task::_eval_condition_expr(const std::string& line)
{
    Lexer lexer(line);
    auto  tokens = lexer.tokenize();

    Parser    parser(tokens);
    auto      root = parser.parse();
    Evaluator evaluator(&m_context->bools, &m_context->ints, &m_context->strings);
    auto      value = evaluator.evaluate(root.get());

    return std::get<bool>(value->value);
}

std::string Task::_gen_condition_expr(const Node* node)
{
    ExprGenerator generator;
    auto          expr = generator.generate(node);

    for (const auto& var : m_context->instances)
    {
        expr = std::regex_replace(expr, std::regex(var.first), var.second);
    }

    return expr;
}

void Task::_process_state()
{
    if (m_type == Type::tMacro || m_type == Type::tInstance)
    {
        m_state = _in_miss_branch() ? State::sSkip : State::sKeep;
    }
}

bool Task::_is_skip()
{
    return m_state == State::sSkip;
}

bool Task::_in_miss_branch()
{
    return !m_branch_stack.empty() &&
        !std::visit([](const auto& b) { return b.current; }, m_branch_stack.top());
}

StaticBranch Task::_pop_static()
{
    auto branch = std::get<StaticBranch>(m_branch_stack.top());
    m_branch_stack.pop();
    return branch;
}

DynamicBranch Task::_pop_dynamic()
{
    auto branch = std::get<DynamicBranch>(m_branch_stack.top());
    m_branch_stack.pop();
    return branch;
}

} // namespace pps
