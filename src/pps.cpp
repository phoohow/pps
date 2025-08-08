#include <pps/pps.h>
#include <task.h>

#include <sstream>
#include <iostream>

namespace pps
{
    static int countBlank(const std::string &str)
    {
        size_t firstNonTab = str.find_first_not_of(' ');
        if (firstNonTab == std::string::npos)
        {
            return str.length();
        }
        return firstNonTab;
    }

    static bool startsWith(const std::string &str, const std::string &prefix)
    {
        return str.rfind(prefix, 0) == 0;
    }

    static bool endsWith(const std::string &str, const std::string &suffix)
    {
        return str.rfind(suffix) == (str.length() - suffix.length());
    }

    static void formatPPSIndent(std::string &line, int &indentLevel)
    {
        if (startsWith(line, "{"))
        {
            indentLevel++;
        }
        else if (startsWith(line, "}"))
        {
            indentLevel = std::max(0, indentLevel - 1);
        }
        else
        {
            auto currentIndent = countBlank(line);
            if (currentIndent < indentLevel * 4)
            {
                line = std::string(indentLevel * 4, ' ') + line;
            }
        }
    }

    static void formatPPSEnter(std::string &line)
    {
        if (endsWith(line, "}"))
        {
            line += "\n";
        }

        line += "\n";
    }

    PPS::PPS()
    {
        m_task = new Task();
    }

    PPS::~PPS()
    {
        delete m_task;
    }

    std::string PPS::process(const std::string &source, const DefineCTX &define, const ReplaceCTX &replace, const IncludeCTX &include, bool isStatic)
    {
        m_task->setContext(define, replace, include);
        return process(source, isStatic);
    }

    std::string PPS::process(const std::string &source, const DefineCTX &define, const ReplaceCTX &replace, sbin::Loader *moduleLoader, const std::string &decryptionKey, bool isStatic)
    {
        m_task->setContext(define, replace, moduleLoader, decryptionKey);
        return process(source, isStatic);
    }

    std::string PPS::process(const std::string &source, bool isStatic)
    {
        std::istringstream iss(source);
        std::string line;
        std::string output;

        int indentLevel = 0;

        while (std::getline(iss, line))
        {
            auto state = m_task->process(line, isStatic);
            if (line.empty())
                continue;

            formatPPSIndent(line, indentLevel);

            formatPPSEnter(line);

            output += line;
        }

        return output;
    }

} // namespace pps
