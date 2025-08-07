#include <task.h>
#include <pipeline/simplifier.h>
#include <pipeline/generator.h>

#include <sbin/loader.h>

#include <regex>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace pps
{

    static auto g_task = std::regex(R"(\*<\$([^>]*)>\*)");

    static auto g_task_branch = std::regex(R"(branch (.+))");
    static auto g_task_include = std::regex(R"(include (.+))");
    static auto g_task_override = std::regex(R"(override (.+))");
    static auto g_task_embed = std::regex(R"(embed .+)");
    static auto g_task_prog = std::regex(R"(prog .+)");

    static auto g_branch_if = std::regex(R"(if (.+))");
    static auto g_branch_elif = std::regex(R"(elif (.+))");
    static auto g_branch_else = std::regex(R"(else)");
    static auto g_branch_endif = std::regex(R"(endif)");

    Task::Task()
    {
    }

    void Task::setContext(const DefineCTX &define, const ReplaceCTX &replace, const IncludeCTX &include)
    {
        m_condition = define;
        m_replace = replace;
        m_include = include;
    }

    void Task::setContext(const DefineCTX &define, const ReplaceCTX &replace, sbin::Loader *moduleLoader, const std::string &decryptionKey)
    {
        m_condition = define;
        m_replace = replace;
        m_loader = moduleLoader;
        m_decryptionKey = decryptionKey;
    }

    Task::State Task::process(std::string &line, bool isStatic)
    {
        auto originLine = line;
        m_isStatic = isStatic;
        auto type = extractTask(line);

        switch (type)
        {
        case Type::tOrigin:
            processOrigin(line);
            break;
        case Type::tBranch:
            line = processBranch(line);
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

    void Task::processOrigin(std::string &line)
    {
        if (isSkip())
            line = "";
    }

    Task::BranchState Task::evaluateBranch(std::string &line)
    {
        BranchState state;
        state.type = extractBranchTask(line);

        switch (state.type)
        {
        case BranchType::tIf:
        {
            Lexer lexer(line);
            auto tokens = lexer.tokenize();
            state.keepCurrent = hasBranchTrue(tokens);

            Parser parser(tokens);
            auto root = parser.parse();
            Evaluator evaluator(&m_condition);
            auto value = evaluator.evaluate(root.get());

            state.value = std::get<bool>(value->value);

            if (!m_isStatic && state.keepCurrent)
            {
                state.value = true;
                state.conditionExpr = processCondition(root.get());
                state.keepElse = true;
            }
            m_branchStack.push(state);

            break;
        }
        case BranchType::tElif:
        {
            Lexer lexer(line);
            auto tokens = lexer.tokenize();
            state.keepCurrent = hasBranchTrue(tokens);

            auto oldState = m_branchStack.top();
            state.keepElse = oldState.keepElse;

            if (!oldState.keepCurrent && state.keepCurrent)
            {
                state.type = BranchType::tIf;
            }

            Parser parser(tokens);
            auto root = parser.parse();
            Evaluator evaluator(&m_condition);
            auto value = evaluator.evaluate(root.get());

            state.value = std::get<bool>(value->value);

            if (!m_isStatic && state.keepCurrent)
            {
                state.value = true;
                state.conditionExpr = processCondition(root.get());
                state.keepElse = true;
            }

            m_branchStack.top() = state;

            break;
        }
        case BranchType::tElse:
        {
            auto oldState = m_branchStack.top();
            state.keepElse = oldState.keepElse;
            state.value = !oldState.value;
            if (!m_isStatic && state.keepElse)
            {
                state.value = true;
            }

            m_branchStack.top() = state;
            break;
        }
        case BranchType::tEndif:
        {
            m_branchStack.pop();
            break;
        }
        default:
            break;
        }

        return state;
    }

    std::string Task::processBranch(std::string &line)
    {
        auto state = evaluateBranch(line);

        if (m_isStatic || m_branchStack.empty())
        {
            return "";
        }

        if (state.type == BranchType::tIf)
        {
            if (!state.keepCurrent)
            {
                return "";
            }

            std::string ifExpr = "if(";
            ifExpr += state.conditionExpr;
            ifExpr += ")";

            return ifExpr;
        }
        else if (state.type == BranchType::tElif)
        {
            if (!state.keepCurrent)
            {
                return "";
            }

            std::string elifExpr = "elif(";
            elifExpr += state.conditionExpr;
            elifExpr += ")";

            return elifExpr;
        }
        else if (state.type == BranchType::tElse)
        {
            if (!state.keepElse)
            {
                return "";
            }

            return "else";
        }
        else if (state.type == BranchType::tEndif)
        {
            return "";
        }

        return "";
    }

    void Task::processInclude(std::string &path)
    {
        if (isSkip())
            return;

        std::string content;
        content += extractIncludeFromCTX(path);
        content += extractIncludeFromLoader(path);

        path = content;
    }

    std::string Task::extractIncludeFromCTX(const std::string &path)
    {
        std::string oldPath = path;
        for (const auto &prefix : m_include.prefixes)
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

    std::string Task::extractIncludeFromLoader(const std::string &path)
    {
        if (m_loader == nullptr)
            return "";

        auto data = m_loader->getShader(path, m_decryptionKey);
        if (data == nullptr)
            return "";

        return std::string(data->data.begin(), data->data.end());
    }

    void Task::processOverride(std::string &origin, std::string &expr)
    {
        if (isSkip())
            return;

        auto iter = m_replace.texts.find(expr);
        if (iter == m_replace.texts.end())
        {
            std::cerr << "Fail to find " << expr << " in context." << std::endl;
            expr = "";
            return;
        }

        static std::regex token_override_expr(R"((\w+)\s*(/\*<\$override[^>]*>\*/))");
        expr = std::regex_replace(origin, token_override_expr, iter->second);
    }

    void Task::processEmbed(std::string &expr)
    {
        if (isSkip())
            return;
    }

    void Task::processProg(std::string &expr)
    {
        if (isSkip())
            return;
    }

    Task::Type Task::extractTask(std::string &line)
    {
        std::smatch match_task;
        if (!std::regex_search(line, match_task, g_task))
            return Type::tOrigin;

        auto task = match_task[1].str();
        std::smatch match_branch;
        if (std::regex_search(task, match_branch, g_task_branch))
        {
            line = match_branch[1].str();
            return Type::tBranch;
        }
        else if (std::regex_search(task, match_branch, g_task_include))
        {
            line = match_branch[1].str();
            return Type::tInclude;
        }
        else if (std::regex_search(task, match_branch, g_task_override))
        {
            line = match_branch[1].str();
            return Type::tOverride;
        }
        else if (std::regex_search(task, match_branch, g_task_embed))
        {
            line = match_branch[0].str();
            return Type::tEmbed;
        }
        else if (std::regex_search(task, match_branch, g_task_prog))
        {
            line = match_branch[0].str();
            return Type::tProg;
        }

        return Type::tOrigin;
    }

    Task::BranchType Task::extractBranchTask(std::string &line)
    {
        std::smatch match_branch;
        if (std::regex_search(line, match_branch, g_branch_elif))
        {
            line = match_branch[1].str();
            return BranchType::tElif;
        }
        else if (std::regex_search(line, match_branch, g_branch_if))
        {
            line = match_branch[1].str();
            return BranchType::tIf;
        }
        else if (std::regex_search(line, match_branch, g_branch_else))
        {
            line = match_branch[0].str();
            return BranchType::tElse;
        }
        else if (std::regex_search(line, match_branch, g_branch_endif))
        {
            line = match_branch[0].str();
            return BranchType::tEndif;
        }

        return BranchType::tEndif;
    }

    bool Task::hasBranchTrue(const std::vector<Token> &tokens)
    {
        for (int i = 0; i < tokens.size(); i++)
        {
            const auto &token = tokens[i];
            if (token.type != TokenType::tVariable)
                continue;

            auto value = m_condition.bools[token.value];
            if (value)
                return true;
        }

        return false;
    }

    std::string Task::processCondition(const Node *node)
    {
        ExprSimplifier simplifier(m_condition.bools);
        auto simplifiedNode = simplifier.simplify(node);

        ExprGenerator generator;
        auto expr = generator.generate(simplifiedNode.get());

        for (const auto &var : m_replace.texts)
        {
            expr = std::regex_replace(expr, std::regex(var.first), var.second);
        }

        return expr;
    }

    void Task::processState()
    {
        if (m_branchStack.empty() || m_branchStack.top().value)
        {
            m_state = State::sKeep;
        }
        else
        {
            m_state = State::sSkip;
        }
    }

    bool Task::isSkip()
    {
        return m_state == State::sSkip;
    }

} // namespace pps
