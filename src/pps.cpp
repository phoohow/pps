#include <pps/pps.h>
#include <task.h>

#include <sstream>
#include <iostream>

namespace pps
{
static int countBlank(const std::string& str)
{
    size_t firstNonTab = str.find_first_not_of(' ');
    if (firstNonTab == std::string::npos)
    {
        return str.length();
    }
    return firstNonTab;
}

static bool startsWith(const std::string& str, const std::string& prefix)
{
    return str.rfind(prefix, 0) == 0;
}

static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.rfind(suffix) == (str.length() - suffix.length());
}

static void formatPPSIndent(std::string& line, int& indentLevel)
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

static void formatPPSEnter(std::string& line)
{
    if (endsWith(line, "}"))
    {
        line += "\n";
    }

    line += "\n";
}

static void limitConsecutiveNewlines(std::string& str, int maxConsecutive = 2)
{
    if (str.empty() || maxConsecutive < 0)
        return;

    auto isWindowsNewline = [&](size_t pos) {
        return pos + 1 < str.length() && str[pos] == '\r' && str[pos + 1] == '\n';
    };

    auto isAnyNewline = [&](size_t pos) {
        return str[pos] == '\n' || str[pos] == '\r';
    };

    size_t writePos     = 0;
    int    newlineCount = 0;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (isWindowsNewline(i))
        {
            newlineCount++;
            if (newlineCount <= maxConsecutive)
            {
                str[writePos++] = '\n';
            }
            ++i;
        }
        else if (isAnyNewline(i))
        {
            newlineCount++;
            if (newlineCount <= maxConsecutive)
            {
                str[writePos++] = '\n';
            }
        }
        else
        {
            str[writePos++] = str[i];
            newlineCount    = 0;
        }
    }

    str.resize(writePos);
}

PPS::PPS()
{
    m_task = new Task();
}

PPS::~PPS()
{
    delete m_task;
}

std::string PPS::process(const std::string& source, Context* context)
{
    m_task->setContext(context);
    return process(source);
}

std::string PPS::process(const std::string& source, Context* context, sbin::Loader* moduleLoader, const std::string& decryptionKey)
{
    m_task->setContext(context, moduleLoader, decryptionKey);
    return process(source);
}

std::string PPS::process(const std::string& source)
{
    std::istringstream iss(source);
    std::string        line;
    std::string        output;

    int indentLevel = 0;

    while (std::getline(iss, line))
    {
        auto state = m_task->process(line);
        if (line.empty())
            continue;

        formatPPSIndent(line, indentLevel);

        output += line;
    }

    limitConsecutiveNewlines(output);

    return output;
}

} // namespace pps
