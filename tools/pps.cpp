#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <pps/pps.h>

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
              << "Tasks:"
              << "  --codegen    Code generation (default)"
              << "  --evaluate   Evaluation"
              << "  --help       Show this help message"
              << "Modes:"
              << "  --static     Static mode (default)"
              << "  --dynamic    Dynamic mode"
              << "Options:"
              << "  --db <xx=xxx>     Define boolean variable (stored in defineCtx)"
              << "  --di <xx=xxx>     Define integer variable (stored in defineCtx)"
              << "  --ds <xx=xxx>     Define string variable (stored in defineCtx)"
              << "  --r <xx=xxx>      Define replaceCtx text"
              << "  --i <path>        Add include path"
              << "  --input <path>    Specify input HLSL file"
              << "  --output <path>   Specify output file" << std::endl;
}

int main(int argc, char* argv[])
{
    Mode mode     = Mode::Codegen;
    bool isStatic = true;

    // Contexts for PPS processing
    pps::DefineCTX  defineCtx;
    pps::ReplaceCTX replaceCtx;
    pps::IncludeCTX includeCtx;

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
            isStatic = (arg == "--static");
        }
        // Database option
        else if (arg == "--db" && i + 1 < argc)
        {
            std::string dbOption = argv[++i];
            // Parse dbOption as key=value and add to defineCtx
            size_t pos = dbOption.find('=');
            if (pos != std::string::npos)
            {
                std::string key   = dbOption.substr(0, pos);
                std::string value = dbOption.substr(pos + 1);
                // Parse as boolean
                if (value == "true" || value == "false")
                {
                    defineCtx.bools[key] = (value == "true");
                }
                else
                {
                    // If not explicitly true/false, treat as true if value is non-empty
                    defineCtx.bools[key] = !value.empty();
                }
            }
        }
        // Diagnostic option
        else if (arg == "--di" && i + 1 < argc)
        {
            std::string diOption = argv[++i];
            // Parse diOption as key=value and add to defineCtx
            size_t pos = diOption.find('=');
            if (pos != std::string::npos)
            {
                std::string key   = diOption.substr(0, pos);
                std::string value = diOption.substr(pos + 1);
                // Try to parse as integer
                try
                {
                    int intValue        = std::stoi(value);
                    defineCtx.ints[key] = intValue;
                }
                catch (...)
                {
                    std::cerr << "Error: Invalid integer value for --di option." << std::endl;
                    return 1;
                }
            }
        }
        // Data source option
        else if (arg == "--ds" && i + 1 < argc)
        {
            std::string dsOption = argv[++i];
            // Parse dsOption as key=value and add to defineCtx
            size_t pos = dsOption.find('=');
            if (pos != std::string::npos)
            {
                std::string key        = dsOption.substr(0, pos);
                std::string value      = dsOption.substr(pos + 1);
                defineCtx.strings[key] = value;
            }
        }
        // Rule option
        else if (arg == "--r" && i + 1 < argc)
        {
            std::string rOption = argv[++i];
            // Parse rOption as key=value and add to defineCtx
            size_t pos = rOption.find('=');
            if (pos != std::string::npos)
            {
                std::string key       = rOption.substr(0, pos);
                std::string value     = rOption.substr(pos + 1);
                replaceCtx.texts[key] = value;
            }
        }
        // Include path option
        else if (arg == "--i" && i + 1 < argc)
        {
            std::string includePath = argv[++i];
            includeCtx.prefixes.insert(includePath);
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

    // Check if we have an input source
    if (inputSource.empty())
    {
        std::cerr << "Error: No input source specified" << std::endl;
        showUsage();
        return 1;
    }

    // Read input source file
    std::ifstream file(inputSource);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << inputSource << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();
    file.close();

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

            if (mode == Mode::Codegen)
            {
                // Code generation task
                result = processor.process(sourceCode, defineCtx, replaceCtx, includeCtx, isStatic);
            }
            else if (mode == Mode::Evaluate)
            {
                // Evaluation task
                // For now, we'll just process the source code as with codegen
                // but this could be extended to do specific evaluation tasks
                result = processor.process(sourceCode, defineCtx, replaceCtx, includeCtx, isStatic);
            }

            // Output result to file or stdout
            if (!outputPath.empty())
            {
                std::ofstream outFile(outputPath);
                if (!outFile.is_open())
                {
                    std::cerr << "Error: Could not open output file " << outputPath << std::endl;
                    return 1;
                }
                outFile << result;
                outFile.close();
            }
            else
            {
                // Output result to stdout
                std::cout << result << std::endl;
            }

            return 0;
        }
    }
}