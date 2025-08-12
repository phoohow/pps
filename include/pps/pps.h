#pragma once

#include <unordered_map>
#include <string>
#include <set>

#if _WIN32
#    ifdef PPS_EXPORT_DLL
#        define PPS_API __declspec(dllexport)
#    else
#        define PPS_API __declspec(dllimport)
#    endif
#else
#    define PPS_API
#endif

namespace sbin
{
class Loader;
}

namespace pps
{

struct Context
{
    // Defined variables
    std::unordered_map<std::string, bool>        bools;
    std::unordered_map<std::string, int>         ints;
    std::unordered_map<std::string, std::string> strings;

    // Branch instances
    std::unordered_map<std::string, std::string> instances;

    // Include prefixes
    std::set<std::string> prefixes;

    // Is static mode
    bool isStatic = true;
};

class Task;
class PPS_API PPS
{
    Task* m_task;

public:
    PPS();

    ~PPS();

    std::string process(const std::string& source, Context* context);

    std::string process(const std::string& source, Context* context, sbin::Loader* moduleLoader, const std::string& decryptionKey);

private:
    std::string process(const std::string& source);
};

} // namespace pps
