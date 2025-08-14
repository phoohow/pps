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
    BranchTag type      = BranchTag::tIf;
    bool      choosedIf = false;
    bool      current   = false;
};

struct DynamicBranch
{
    BranchTag   type       = BranchTag::tIf;
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

    void setContext(Context* context);
    void setContext(Context* context, sbin::Loader* moduleLoader, const std::string& decryptionKey);

    State process(std::string& line);

private:
    State m_state = State::sKeep;
    Type  m_type  = Type::tOrigin;

    // Branch
private:
    std::stack<std::variant<StaticBranch, DynamicBranch>> m_branchStack;

    // Prog
private:
    enum class ProgType
    {
        tStatement,
        tEnd,
    };
    std::string m_progSource;

private:
    Type          extractTask(std::string& line);
    void          processState();
    bool          isSkip();
    bool          inMissedBranch();
    StaticBranch  popStatic();
    DynamicBranch popDynamic();

    // Origin
    void processOrigin(std::string& line);

    // Branch
    BranchTag     extractBranchTag(std::string& line);
    void          evaluateStaticBranch(std::string& line);
    DynamicBranch evaluateDynamicBranch(std::string& line);
    void          processStaticBranch(std::string& line);
    std::string   processDynamicBranch(std::string& line);
    bool          hasBranchTrue(const std::vector<Token>& tokens);
    bool          isValidConditionExpr(const Node* node);
    bool          evaluateConditionExpr(const std::string& line);
    std::string   generateConditionExpr(const Node* node);

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
