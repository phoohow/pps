#include <pps/pps.h>

#include <aclg/aclg.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

enum class Mode
{
    Help,
    Codegen,
    Evaluate
};

// Function to display usage information for HLSL processing
void showUsage()
{
    std::cout << "Usage: pps [task] [mode] [option] <input_file.hlsl>\n"
              << "Tasks:\n"
              << "  --codegen            Code generation (default)\n"
              << "  --evaluate           Evaluation\n"
              << "  --help               Show this help message\n"
              << "Modes:\n"
              << "  --static             Static mode (default)\n"
              << "  --dynamic            Dynamic mode\n"
              << "Options:\n"
              << "  --db <xx=xxx>        Define boolean variable\n"
              << "  --di <xx=xxx>        Define integer variable\n"
              << "  --ds <xx=xxx>        Define string variable\n"
              << "  --instance <xx=xxx>  Define instance context\n"
              << "  --include <path>     Add include path\n"
              << "  --input <path>       Specify input HLSL file\n"
              << "  --output <path>      Specify output file" << std::endl;
}

int main(int argc, char* argv[])
{
    Mode mode     = Mode::Codegen;
    bool isStatic = true;

    // Contexts for PPS processing
    pps::Context ctx;

    std::string inputSource;
    std::string outputPath;

    // Parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        // Help option
        if (arg == "--help" || arg == "-h")
        {
            mode = Mode::Help;
        }
        // Task options
        else if (arg == "--codegen")
        {
            mode = Mode::Codegen;
        }
        else if (arg == "--evaluate")
        {
            mode = Mode::Evaluate;
        }
        // Mode options
        else if (arg == "--static" || arg == "--dynamic")
        {
            ctx.isStatic = (arg == "--static");
        }
        // Database option
        else if (arg == "--db" && i + 1 < argc)
        {
            std::string dbOption = argv[++i];
            // Parse dbOption as key=value and add to Context
            size_t pos = dbOption.find('=');
            if (pos != std::string::npos)
            {
                std::string key   = dbOption.substr(0, pos);
                std::string value = dbOption.substr(pos + 1);
                // Parse as boolean
                if (value == "true" || value == "false")
                {
                    ctx.bools[key] = (value == "true");
                }
                else
                {
                    // If not explicitly true/false, treat as true if value is non-empty
                    ctx.bools[key] = !value.empty();
                }
            }
        }
        // Diagnostic option
        else if (arg == "--di" && i + 1 < argc)
        {
            std::string diOption = argv[++i];
            // Parse diOption as key=value and add to Context
            size_t pos = diOption.find('=');
            if (pos != std::string::npos)
            {
                std::string key   = diOption.substr(0, pos);
                std::string value = diOption.substr(pos + 1);
                // Try to parse as integer
                try
                {
                    int intValue  = std::stoi(value);
                    ctx.ints[key] = intValue;
                }
                catch (...)
                {
                    ACLG_ERROR("Invalid integer value for --di option.");
                    return 1;
                }
            }
        }
        // Data source option
        else if (arg == "--ds" && i + 1 < argc)
        {
            std::string dsOption = argv[++i];
            // Parse dsOption as key=value and add to Context
            size_t pos = dsOption.find('=');
            if (pos != std::string::npos)
            {
                std::string key   = dsOption.substr(0, pos);
                std::string value = dsOption.substr(pos + 1);
                ctx.strings[key]  = value;
            }
        }
        // Rule option
        else if (arg == "--r" && i + 1 < argc)
        {
            std::string rOption = argv[++i];
            // Parse rOption as key=value and add to Context
            size_t pos = rOption.find('=');
            if (pos != std::string::npos)
            {
                std::string key    = rOption.substr(0, pos);
                std::string value  = rOption.substr(pos + 1);
                ctx.instances[key] = value;
            }
        }
        // Include path option
        else if (arg == "--i" && i + 1 < argc)
        {
            std::string includePath = argv[++i];
            ctx.prefixes.insert(includePath);
        }
        // Input file option
        else if (arg == "--input" && i + 1 < argc)
        {
            inputSource = argv[++i];
        }
        // Output file option
        else if (arg == "--output" && i + 1 < argc)
        {
            outputPath = argv[++i];
        }
        // Input file (assumed to be the last argument if not prefixed)
        else if (arg.substr(0, 2) != "--")
        {
            inputSource = arg;
        }
    }

    switch (mode)
    {
        case Mode::Help:
            showUsage();
            return 0;
        // (TODO:)Evaluation task prog; need do grammar check
        case Mode::Evaluate:

        case Mode::Codegen:
        default:
        {
            // Process based on task
            pps::PPS    processor;
            std::string result;

            // Check if we have an input source
            if (inputSource.empty())
            {
                ACLG_ERROR("No input source specified.");
                showUsage();
                return 1;
            }

            // Read input source file
            std::ifstream file(inputSource);
            if (!file.is_open())
            {
                ACLG_ERROR("Could not open file {}.", inputSource);
                return 1;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string sourceCode = buffer.str();
            file.close();

            if (mode == Mode::Codegen)
            {
                // Code generation task
                result = processor.process(sourceCode, &ctx);
            }
            else if (mode == Mode::Evaluate)
            {
                // Evaluation task
                // For now, we'll just process the source code as with codegen
                // but this could be extended to do specific evaluation tasks
                result = processor.process(sourceCode, &ctx);
            }

            // Output result to file or stdout
            if (!outputPath.empty())
            {
                std::ofstream outFile(outputPath);
                if (!outFile.is_open())
                {
                    ACLG_ERROR("Could not open output file {}.", outputPath);
                    return 1;
                }
                outFile << result;
                outFile.close();
            }
            else
            {
                // Output result to stdout
                ACLG_INFO("Succeed: {}", result);
            }

            return 0;
        }
    }
}