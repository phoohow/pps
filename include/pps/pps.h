#pragma once

#include <unordered_map>
#include <string>
#include <set>

namespace pps
{
    struct DefineCTX
    {
        std::unordered_map<std::string, bool> bools;
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, std::string> strings;
    };

    struct ReplaceCTX
    {
        std::unordered_map<std::string, std::string> texts;
    };

    struct IncludeCTX
    {
        std::set<std::string> prefixes;
    };

    class Task;
    class PPS
    {
        Task *m_task;

    public:
        PPS(const DefineCTX &define, const ReplaceCTX &replace, const IncludeCTX &include);

        ~PPS();

        std::string process(const std::string &source, bool isStatic = true);
    };

} // namespace pps

