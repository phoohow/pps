#include <pps/pps.h>
#include <task.h>

#include <sstream>

namespace pps
{

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

        while (std::getline(iss, line))
        {
            auto state = m_task->process(line, isStatic);

            switch (state)
            {
            case Task::State::sKeep:
                if (output.back() == '\n')
                    output += line;
                else
                    output += "\n" + line;
            case Task::State::sSkip:
                break;
            }
        }

        return output;
    }

} // namespace pps
