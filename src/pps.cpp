#include <pps/pps.h>
#include <task.h>

#include <sstream>
#include <iostream>

namespace pps
{
static int count_blank(const std::string& str)
{
    size_t firstNonTab = str.find_first_not_of(' ');
    if (firstNonTab == std::string::npos)
    {
        return str.length();
    }
    return firstNonTab;
}

static bool start_with(const std::string& str, const std::string& prefix)
{
    return str.rfind(prefix, 0) == 0;
}

static bool end_with(const std::string& str, const std::string& suffix)
{
    return str.rfind(suffix) == (str.length() - suffix.length());
}

static void format_pps_indent(std::string& line, int& indent_level)
{
    if (start_with(line, "{"))
    {
        indent_level++;
    }
    else if (start_with(line, "}"))
    {
        indent_level = std::max(0, indent_level - 1);
    }
    else
    {
        auto currentIndent = count_blank(line);
        if (currentIndent < indent_level * 4)
        {
            line = std::string(indent_level * 4, ' ') + line;
        }
    }
}

static void format_pps_enter(std::string& line)
{
    if (end_with(line, "}"))
    {
        line += "\n";
    }

    line += "\n";
}

static void limit_conherent_enters(std::string& str, int maxConsecutive = 2)
{
    if (str.empty() || maxConsecutive < 0)
        return;

    auto is_win_enter = [&](size_t pos) {
        return pos + 1 < str.length() && str[pos] == '\r' && str[pos + 1] == '\n';
    };

    auto is_non_win_enter = [&](size_t pos) {
        return str[pos] == '\n' || str[pos] == '\r';
    };

    size_t writePos     = 0;
    int    newlineCount = 0;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (is_win_enter(i))
        {
            newlineCount++;
            if (newlineCount <= maxConsecutive)
            {
                str[writePos++] = '\n';
            }
            ++i;
        }
        else if (is_non_win_enter(i))
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
    m_task->set_ctx(context);
    return process(source);
}

std::string PPS::process(const std::string& source, Context* context, sbin::Loader* module_loader, const std::string& decrypt_key)
{
    m_task->set_ctx(context, module_loader, decrypt_key);
    return process(source);
}

std::string PPS::process(const std::string& source)
{
    std::istringstream iss(source);
    std::string        line;
    std::string        output;

    int indent_level = 0;

    while (std::getline(iss, line))
    {
        auto state = m_task->process(line);
        if (line.empty())
            continue;

        format_pps_indent(line, indent_level);

        output += line;
    }

    limit_conherent_enters(output);

    return output;
}

} // namespace pps
