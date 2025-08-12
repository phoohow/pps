#pragma once

#include <frontend/lexer.h>
#include <frontend/parser.h>
#include <pipeline/evaluator.h>

#include <pps/pps.h>

#include <stack>

namespace pps
{
enum class BranchType : uint8_t
{
    tIf,
    tElif,
    tElse,
    tEndif,
};

struct StaticState
{
    BranchType type      = BranchType::tIf;
    bool       choosedIf = false;
    bool       current   = false;
};

struct DynamicState
{
    BranchType  type       = BranchType::tIf;
    bool        enableElse = false;
    bool        current    = false;
    std::string conditionExpr;
};

class Task
{
    Context* m_context;

    sbin::Loader* m_loader        = nullptr;
    std::string   m_decryptionKey = "";

public:
    enum class Type
    {
        tOrigin,
        tBranch,
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

    void setContext(Context* context);
    void setContext(Context* context, sbin::Loader* moduleLoader, const std::string& decryptionKey);

    State process(std::string& line);

private:
    std::stack<Type> m_stack;
    State            m_state    = State::sKeep;
    bool             m_isStatic = true;

    // Branch
private:
    std::stack<StaticState>  m_staticStack;
    std::stack<DynamicState> m_dynamicStack;

    // Prog
private:
    enum class ProgType
    {
        tStatement,
        tEnd,
    };
    std::string m_progSource;

private:
    Type extractTask(std::string& line);
    void processState();
    bool isSkip();

    // Origin
    void processOrigin(std::string& line);

    // Branch
    BranchType   extractBranchType(std::string& line);
    void         evaluateStaticBranch(std::string& line);
    DynamicState evaluateDynamicBranch(std::string& line);
    std::string  processBranch(std::string& line);
    std::string  processStaticBranch(std::string& line);
    std::string  processDynamicBranch(std::string& line);
    bool         hasBranchTrue(const std::vector<Token>& tokens);
    bool         evaluateConditionExpr(const std::string& expr);
    std::string  generateConditionExpr(const Node* node);

    // Include
    void        processInclude(std::string& line);
    std::string extractIncludeFromCTX(const std::string& path);
    std::string extractIncludeFromLoader(const std::string& path);

    // Override
    void processOverride(std::string& origin, std::string& line);
    void extractOverrideTask(std::string& line);

    // Embed
    void processEmbed(std::string& line);
    void extractEmbedTask(std::string& line);

    // Prog
    void     processProg(std::string& line);
    void     evaluateProg(std::string& line);
    ProgType extractProgTask(std::string& line);
};

} // namespace pps
