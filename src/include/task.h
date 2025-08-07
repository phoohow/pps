#pragma once

#include <frontend/lexer.h>
#include <frontend/parser.h>
#include <pipeline/evaluator.h>

#include <pps/pps.h>

#include <stack>

namespace pps
{

    class Task
    {
        DefineCTX m_condition;
        ReplaceCTX m_replace;
        IncludeCTX m_include;

        sbin::Loader *m_loader = nullptr;
        std::string m_decryptionKey = "";

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

        void setContext(const DefineCTX &define, const ReplaceCTX &replace, const IncludeCTX &include);
        void setContext(const DefineCTX &define, const ReplaceCTX &replace, sbin::Loader *moduleLoader, const std::string &decryptionKey);

        State process(std::string &line, bool isStatic);

    private:
        std::stack<Type> m_stack;
        State m_state = State::sKeep;
        bool m_isStatic = true;

        // Branch
    private:
        enum class BranchType : uint8_t
        {
            tIf,
            tElif,
            tElse,
            tEndif,
        };

        struct BranchState
        {
            BranchType type = BranchType::tIf;
            bool value = false;
            bool keepCurrent = false;
            bool keepElse = false;
            std::string conditionExpr;
        };

        std::stack<BranchState> m_branchStack;

        // Prog
    private:
        enum class ProgType
        {
            tStatement,
            tEnd,
        };
        std::string m_progSource;

    private:
        Type extractTask(std::string &line);
        void processState();
        bool isSkip();

        // Origin
        void processOrigin(std::string &line);

        // Branch
        BranchState evaluateBranch(std::string &line);
        std::string processBranch(std::string &line);
        BranchType extractBranchTask(std::string &line);
        bool hasBranchTrue(const std::vector<Token> &tokens);
        std::string processCondition(const Node *node);

        // Include
        void processInclude(std::string &line);
        std::string extractIncludeFromCTX(const std::string &path);
        std::string extractIncludeFromLoader(const std::string &path);

        // Override
        void processOverride(std::string &origin, std::string &line);
        void extractOverrideTask(std::string &line);

        // Embed
        void processEmbed(std::string &line);
        void extractEmbedTask(std::string &line);

        // Prog
        void processProg(std::string &line);
        void evaluateProg(std::string &line);
        ProgType extractProgTask(std::string &line);
    };

}
