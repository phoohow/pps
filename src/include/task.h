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

        Task(const DefineCTX &define, const ReplaceCTX &replace);

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
            bool hasTrue = false;
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

        // Branch
        BranchState evaluateBranch(std::string &line);
        std::string processBranch(std::string &line);
        BranchType extractBranchTask(std::string &line);
        bool hasBranchTrue(const std::vector<Token> &tokens);
        std::string processCondition(const Node *node);

        // Include
        void processsInclude(std::string &line);
        void extractIncludeTask(std::string &line);

        // Override
        void processsOverride(std::string &line);
        void extractOverrideTask(std::string &line);

        // Embed
        void processsEmbed(std::string &line);
        void extractEmbedTask(std::string &line);

        // Prog
        void processProg(std::string &line);
        void evaluateProg(std::string &line);
        ProgType extractProgTask(std::string &line);
    };

}
