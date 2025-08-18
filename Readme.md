## Introduction
PPS is a source code processing language designed to modify or remove code blocks based on contextual variable values through various Tasks. It addresses the limitations of HLSL preprocessing by introducing dynamic and static modes, enabling unified handling of code logic across different scenarios and reducing maintenance costs by minimizing code duplication.

## Functionality
- Input: File and context (Variable table)
- Output: .bin file (with original file in debug mode)

### Modes
We provide two modes: dynamic and static.
- In static mode, blocks are retained when the associated Variable is true; otherwise, they are removed.
- In dynamic mode, conditional statements are replaced with static statements when Variables evaluate to true; otherwise, they are removed.

### Tasks
1. Branch: Extends HLSL's conditional compilation concept, enabling conditional expression simplification for more flexible code generation in both dynamic and static modes.
   - Supports both static and dynamic modes with different syntax (`/*<$static if condition>*/` and `/*<$dynamic if condition>*/`)
2. Include: Directly corresponds to HLSL's include functionality, maintaining identical behavior.
   - Syntax: `/*<$include path>*/`
3. Override: A new concept for dynamically modifying source code.
   - Syntax: `/*<$override variable>*/`
4. Embed: A new concept enabling dynamic code embedding to extend source code logic.
   - Syntax: `/*<$embed variable>*/`
5. Prog: An independent language that enables complex logic execution and deeper interaction with source code.
   - Syntax: `/*<$prog command>*/`

## Grammar

See [grammar.md](grammar.md) for detailed grammar specifications.

## Pipeline
PPS offers two distinct workflows for processing source code:

### Evaluation Workflow
```
Tokenize → Parse → Evaluate
```
1. **Tokenize**: Split source code into meaningful tokens
2. **Parse**: Convert tokens into an Abstract Syntax Tree (AST)
3. **Evaluate**: Process AST to resolve expressions and execute conditional logic based on context variables
> See [evaluate](src/test/testEvaluator.cpp)

### Optimization Workflow
```
Tokenize → Parse → Simplify → Codegen
```
1. **Tokenize**: Split source code into meaningful tokens
2. **Parse**: Convert tokens into an Abstract Syntax Tree (AST)
3. **Simplify**: Optimize AST by reducing complex expressions and removing unnecessary nodes
4. **Codegen**: Generate optimized HLSL output from the simplified AST
> See [generate](src/test/testGenerator.cpp)

## Command Line Tool

PPS includes a command-line tool `pps` for processing HLSL source files:

```
pps [task] [mode] [options] <input_file.hlsl>
```

### Tasks
- `--codegen` - Code generation (default)
- `--evaluate` - Evaluation

### Modes
- `--static` - Static mode (default)
- `--dynamic` - Dynamic mode

### Options
- `--db <key=value>` - Define boolean variable
- `--di <key=value>` - Define integer variable
- `--ds <key=value>` - Define string variable
- `--r <key=value>` - Define instance variable (for dynamic mode)
- `--i <path>` - Add include path
- `--input <path>` - Specify input file
- `--output <path>` - Specify output file
- `--help` - Show help message

### Examples

```
# Process a file with default settings (codegen, static)
pps source.hlsl

# Process a file with dynamic mode
pps --dynamic source.hlsl

# Process a file with evaluation task and custom variables
pps --evaluate --db DEBUG=true --di VERBOSE=1 --ds DEFAULT_COLOR="float3(1, 0, 0)" source.hlsl

# Process a file with instance variables for dynamic mode
pps --dynamic --db useBaseColorMap=true --r useBaseColorMap=mat.useBaseColorMap source.hlsl

# Process a file with custom include path
pps --i ./includes source.hlsl

# Process a file with explicit input and output paths
pps --input source.hlsl --output result.hlsl

# Process a file with multiple context variables
pps --db hasNormalMap=true --db hasSpecularMap=false --di qualityLevel=2 --ds shaderModel="5_0" source.hlsl
```