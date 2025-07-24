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
2. Include: Directly corresponds to HLSL's include functionality, maintaining identical behavior.
3. Override: A new concept for dynamically modifying source code.
4. Embed: A new concept enabling dynamic code embedding to extend source code logic.
5. Prog: An independent language that enables complex logic execution and deeper interaction with source code.

## Grammar
### Data Types
1. bool: (true|false), [0|1]
2. int: [0-9]+
3. string: "[a-zA-Z]+"
4. register: (r|t|s|b)@[a-zA-Z]+

### Variable Types
1. bool: @_(is|has|use|enable|disable)[a-zA-Z]+
2. int: @_[a-zA-Z]+
3. string: @_[a-zA-Z]+
   
   
### Input Types
1. bool: @(is|has|use|enable|disable)[a-zA-Z]+
2. int: @[a-zA-Z]+
3. string: @[a-zA-Z]+

### Operations
1. Logical: &&, ||, !
2. Comparison: ==, !=, >, <
3. Arithmetic: +, -, *, /, %
4. Bitwise: &, |, >>, <<
5. Assignment: =
6. Indexing: []
   
   
### Expressions
1. boolExpr:
   bool
   bool [logical|comparison bool]+
2. intExpr:
   int
   int [arithmetic|bit int]+
   int [arithmetic|bit boolExpr]+
   boolExpr
   boolExpr [arithmetic|bit int]+
   boolExpr [arithmetic|bit boolExpr]+
3. stringExpr:
   string
   string[] + (string)
   
   
### Task Definitions
1. **branch**: $branch, if, elif, else, endif, boolExpr
2. **include**: $include
3. **override**: $override
4. [?] **embed**: $embed
5. [?] **prog**: $prog, export, start, end, if, elif, else, endif, boolExpr, intExpr
   
   
### Branch Task
1. if endif
2. if else endif
3. if elif ... endif
4. if elif ... else endif
> See [branch](samples/1branch.hlsl)
   
   
### Include Task
1. include
> See [include](samples/2include.hlsl)
   
   
### Override Task
1. override
> See [override](samples/3override.hlsl)
   
   
### Embed Task
1. embed
> See [embed](samples/4embed.hlsl)
   
   
### Prog Task
1. prog
> See [prog](samples/5prog.hlsl)
   
   
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