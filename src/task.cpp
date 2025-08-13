#include <task.h>
#include <pipeline/simplifier.h>
#include <pipeline/generator.h>

#include <sbin/loader.h>

#include <regex>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>

namespace pps
{

static auto g_task = std::regex(R"(\*<\$([^>]*)>\*)");

static auto g_task_macro    = std::regex(R"(macro (.+))");
static auto g_task_instance = std::regex(R"(instance (.+))");
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

void Task::setContext(Context* context)
{
    m_context = context;
}

void Task::setContext(Context* context, sbin::Loader* moduleLoader, const std::string& decryptionKey)
{
    m_context       = context;
    m_loader        = moduleLoader;
    m_decryptionKey = decryptionKey;
}

Task::State Task::process(std::string& line)
{
    auto originLine = line;
    m_type          = extractTask(line);

    switch (m_type)
    {
        case Type::tOrigin:
            processOrigin(line);
            break;
        case Type::tMacro:
            processMacroBranch(line);
            processState();
            break;
        case Type::tInstance:
            line = processInstanceBranch(line);
            processState();
            break;
        case Type::tInclude:
            processInclude(line);
            break;
        case Type::tOverride:
            processOverride(originLine, line);
            break;
        case Type::tEmbed:
            processEmbed(line);
            break;
        case Type::tProg:
            processProg(line);
            break;
    }

    return m_state;
}

void Task::processOrigin(std::string& line)
{
    if (isSkip())
        line = "";
}

void Task::evaluateMacroBranch(std::string& line)
{
    MacroBranch state;
    state.type = extractBranchTag(line);

    switch (state.type)
    {
        case BranchTag::tIf:
        {
            if (inMissedBranch())
            {
                state.current = false;
                m_branchStack.push(state);
                break;
            }

            state.current   = evaluateConditionExpr(line);
            state.choosedIf = state.current;
            m_branchStack.push(state);
            break;
        }
        case BranchTag::tElif:
        {
            auto brother = popMacro();
            if (inMissedBranch())
            {
                state.current = false;
                m_branchStack.push(state);
                break;
            }

            if (brother.choosedIf)
            {
                state.choosedIf = true;
                state.current   = false;
            }
            else
            {
                state.current   = evaluateConditionExpr(line);
                state.choosedIf = state.current;
            }

            m_branchStack.push(state);
            break;
        }
        case BranchTag::tElse:
        {
            auto brother = popMacro();
            if (inMissedBranch())
            {
                state.current = false;
                m_branchStack.push(state);
                break;
            }

            state.current = !brother.choosedIf;
            m_branchStack.push(state);
            break;
        }
        case BranchTag::tEndif:
        {
            m_branchStack.pop();
            break;
        }
    }
}

InstanceBranch Task::evaluateInstanceBranch(std::string& line)
{
    InstanceBranch state;
    state.type = extractBranchTag(line);

    switch (state.type)
    {
        case BranchTag::tIf:
        {
            if (inMissedBranch())
            {
                state.current = false;
                m_branchStack.push(state);
                break;
            }

            Lexer          lexer(line);
            auto           tokens = lexer.tokenize();
            Parser         parser(tokens);
            auto           root = parser.parse();
            ExprSimplifier simplifier(m_context->bools);
            auto           simplifiedNode = simplifier.simplify(root.get());

            state.current = isValidConditionExpr(simplifiedNode.get());
            if (state.current)
                state.conditionExpr = generateConditionExpr(simplifiedNode.get());

            state.enableElse = state.current;

            m_branchStack.push(state);
            break;
        }
        case BranchTag::tElif:
        {
            auto brother = popInstance();
            if (inMissedBranch())
            {
                state.current = false;
                m_branchStack.push(state);
                break;
            }

            if (!brother.enableElse)
                state.type = BranchTag::tIf;

            Lexer          lexer(line);
            auto           tokens = lexer.tokenize();
            Parser         parser(tokens);
            auto           root = parser.parse();
            ExprSimplifier simplifier(m_context->bools);
            auto           simplifiedNode = simplifier.simplify(root.get());

            state.current = isValidConditionExpr(simplifiedNode.get());
            if (state.current)
                state.conditionExpr = generateConditionExpr(simplifiedNode.get());

            state.enableElse = brother.enableElse || state.current;

            m_branchStack.push(state);
            break;
        }
        case BranchTag::tElse:
        {
            auto brother = popInstance();
            if (inMissedBranch())
            {
                state.current = false;
                m_branchStack.push(state);
                break;
            }

            state.current = brother.enableElse;
            m_branchStack.push(state);
            break;
        }
        case BranchTag::tEndif:
        {
            m_branchStack.pop();
            break;
        }
        default:
            break;
    }

    return state;
}

void Task::processMacroBranch(std::string& line)
{
    evaluateMacroBranch(line);
}

std::string Task::processInstanceBranch(std::string& line)
{
    auto state = evaluateInstanceBranch(line);
    if (!state.current)
        return "";

    std::string expr;
    switch (state.type)
    {
        case BranchTag::tIf:
        {
            expr += "if(" + state.conditionExpr + ")";
            break;
        }
        case BranchTag::tElif:
        {
            expr += "elif(" + state.conditionExpr + ")";
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

void Task::processInclude(std::string& path)
{
    if (isSkip())
        return;

    std::string content;
    content += extractIncludeFromCTX(path);
    content += extractIncludeFromLoader(path);

    std::string        out;
    std::istringstream iss(content);
    std::string        line;
    while (std::getline(iss, line))
    {
        process(line);

        if (line.empty())
            continue;
        else if (line.back() != '\n')
            line += "\n";

        out += line;
    }

    path = out;
}

std::string Task::extractIncludeFromCTX(const std::string& path)
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

std::string Task::extractIncludeFromLoader(const std::string& path)
{
    if (m_loader == nullptr)
        return "";

    auto data = m_loader->getShader(path, m_decryptionKey);
    if (data == nullptr)
        return "";

    return std::string(data->data.begin(), data->data.end());
}

void Task::processOverride(std::string& origin, std::string& expr)
{
    if (isSkip())
        return;

    auto iter = m_context->instances.find(expr);
    if (iter == m_context->instances.end())
    {
        std::cerr << "Fail to find " << expr << " in context." << std::endl;
        expr = "";
        return;
    }

    static std::regex token_override_expr(R"((\w+)\s*(/\*<\$override[^>]*>\*/))");
    expr = std::regex_replace(origin, token_override_expr, iter->second);
}

void Task::processEmbed(std::string& expr)
{
    if (isSkip())
        return;
}

void Task::processProg(std::string& expr)
{
    if (isSkip())
        return;
}

Task::Type Task::extractTask(std::string& line)
{
    std::smatch match_task;
    if (!std::regex_search(line, match_task, g_task))
        return Type::tOrigin;

    auto        task = match_task[1].str();
    std::smatch match_type;
    if (std::regex_search(task, match_type, g_task_macro))
    {
        line = match_type[1].str();
        return Type::tMacro;
    }
    if (std::regex_search(task, match_type, g_task_instance))
    {
        if (m_context->isStatic) return Type::tMacro;

        line = match_type[1].str();
        return Type::tInstance;
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

BranchTag Task::extractBranchTag(std::string& line)
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

bool Task::hasBranchTrue(const std::vector<Token>& tokens)
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

bool Task::isValidConditionExpr(const Node* node)
{
    Evaluator evaluator(&m_context->bools, &m_context->ints, &m_context->strings);
    auto      value = evaluator.evaluate(node);

    return value->type == ValueType::tBool;
}

bool Task::evaluateConditionExpr(const std::string& line)
{
    Lexer lexer(line);
    auto  tokens = lexer.tokenize();

    Parser    parser(tokens);
    auto      root = parser.parse();
    Evaluator evaluator(&m_context->bools, &m_context->ints, &m_context->strings);
    auto      value = evaluator.evaluate(root.get());

    return std::get<bool>(value->value);
}

std::string Task::generateConditionExpr(const Node* node)
{
    ExprGenerator generator;
    auto          expr = generator.generate(node);

    for (const auto& var : m_context->instances)
    {
        expr = std::regex_replace(expr, std::regex(var.first), var.second);
    }

    return expr;
}

void Task::processState()
{
    if (m_type == Type::tMacro || m_type == Type::tInstance)
    {
        m_state = inMissedBranch() ? State::sSkip : State::sKeep;
    }
}

bool Task::isSkip()
{
    return m_state == State::sSkip;
}

bool Task::inMissedBranch()
{
    return !m_branchStack.empty() &&
        !std::visit([](const auto& b) { return b.current; }, m_branchStack.top());
}

MacroBranch Task::popMacro()
{
    auto branch = std::get<MacroBranch>(m_branchStack.top());
    m_branchStack.pop();
    return branch;
}

InstanceBranch Task::popInstance()
{
    auto branch = std::get<InstanceBranch>(m_branchStack.top());
    m_branchStack.pop();
    return branch;
}

} // namespace pps
