# PPS Language Specification

## Lexical Structure

### Tokens
PPS source code consists of tokens separated by whitespace. Tokens include:
- Identifiers
- Literals
- Operators
- Punctuation

### Identifiers
Identifiers follow standard C naming conventions:
- Must start with a letter or underscore
- Can contain letters, digits, and underscores
- Case-sensitive

### Literals

#### Boolean Literals
- `true`, `false`
- `1`, `0`

#### Integer Literals
- Decimal: `[0-9]+`

#### String Literals
- Quoted strings: `"[a-zA-Z]+"`

#### Register Literals
- Format: `(r|t|s|b)@[a-zA-Z]+`

## Variables

### Variable Naming

#### Context Variables
- Boolean: `@(is|has|use|enable|disable)[a-zA-Z]+`
- Integer: `@[a-zA-Z]+`
- String: `@[a-zA-Z]+`

#### Local Variables
- Boolean: `@_(is|has|use|enable|disable)[a-zA-Z]+`
- Integer: `@_[a-zA-Z]+`
- String: `@_[a-zA-Z]+`

## Expressions

Expressions in PPS are used to compute values that can be assigned to variables or used in statements. PPS supports three types of expressions: boolean, integer, and string.

### Expression Types

Expressions in PPS are classified based on their structure and the operators they use:

1. **Primary Expressions**
   - Identifiers (variable names)
   - Literals (constants like `true`, `false`, `1`, `"string"`)
   - Parenthesized expressions `(expression)`

2. **Postfix Expressions**
   - Primary expressions
   - Array indexing operations `expression[expression]`

3. **Unary Expressions**
   - Postfix expressions
   - Unary operators applied to expressions (`!`, `-`, `~`)

4. **Binary Expressions**
   - Expressions combined with binary operators (`+`, `-`, `*`, `/`, `%`, `&`, `|`, `^`, `<<`, `>>`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `&&`, `||`)

5. **Assignment Expressions**
   - Expressions combined with assignment operator `=`

6. **Conditional Expressions**
   - Ternary conditional operator `expression ? expression : expression` (not yet supported)

7. **Compound Expressions**
   - Multiple expressions combined with operators
   - Complex expressions with nested subexpressions

### Expression Syntax

```
expression:
    assignmentExpr

assignmentExpr:
    conditionalExpr
    conditionalExpr '=' assignmentExpr

conditionalExpr:
    logicalOrExpr
    logicalOrExpr '?' expression ':' conditionalExpr

logicalOrExpr:
    logicalAndExpr
    logicalOrExpr '||' logicalAndExpr

logicalAndExpr:
    bitwiseOrExpr
    logicalAndExpr '&&' bitwiseOrExpr

bitwiseOrExpr:
    bitwiseXorExpr
    bitwiseOrExpr '|' bitwiseXorExpr

bitwiseXorExpr:
    bitwiseAndExpr
    bitwiseXorExpr '^' bitwiseAndExpr

bitwiseAndExpr:
    equalityExpr
    bitwiseAndExpr '&' equalityExpr

equalityExpr:
    relationalExpr
    equalityExpr '==' relationalExpr
    equalityExpr '!=' relationalExpr

relationalExpr:
    shiftExpr
    relationalExpr '<' shiftExpr
    relationalExpr '>' shiftExpr
    relationalExpr '<=' shiftExpr
    relationalExpr '>=' shiftExpr

shiftExpr:
    additiveExpr
    shiftExpr '<<' additiveExpr
    shiftExpr '>>' additiveExpr

additiveExpr:
    multiplicativeExpr
    additiveExpr '+' multiplicativeExpr
    additiveExpr '-' multiplicativeExpr

multiplicativeExpr:
    unaryExpr
    multiplicativeExpr '*' unaryExpr
    multiplicativeExpr '/' unaryExpr
    multiplicativeExpr '%' unaryExpr

unaryExpr:
    postfixExpr
    '!' unaryExpr
    '-' unaryExpr
    '~' unaryExpr

postfixExpr:
    primaryExpr
    postfixExpr '[' expression ']'

primaryExpr:
    identifier
    literal
    '(' expression ')'

literal:
    boolLiteral
    intLiteral
    stringLiteral

boolLiteral:
    'true'
    'false'
    '1'
    '0'

intLiteral:
    decimalNumber

stringLiteral:
    quotedString

identifier:
    variableName
```

## Operators

Operators in PPS are used to perform computations on expressions. The operators are grouped by precedence, with higher precedence operators being evaluated first.

### Operator Precedence

1. **Unary Operators** (Highest precedence)
   - `!` (Logical NOT)
   - `-` (Arithmetic negation)
   - `~` (Bitwise NOT)

2. **Multiplicative Operators**
   - `*` (Multiplication)
   - `/` (Division)
   - `%` (Modulo)
   - `&` (Bitwise AND)
   - `<<` (Left shift)
   - `>>` (Right shift)

3. **Additive Operators**
   - `+` (Addition)
   - `-` (Subtraction)
   - `|` (Bitwise OR)
   - `^` (Bitwise XOR)

4. **Comparison Operators**
   - `==` (Equal)
   - `!=` (Not equal)
   - `<` (Less than)
   - `<=` (Less than or equal)
   - `>` (Greater than)
   - `>=` (Greater than or equal)

5. **Logical Operators** (Lowest precedence)
   - `&&` (Logical AND)
   - `||` (Logical OR)

### Assignment Operator
- `=` (Assignment)

### Indexing Operator
- `[]` (Array indexing)

## Statements

PPS tasks consist of statements that define the behavior of the task.

### Declaration Statement

Declares a new variable with an optional initial value.

Syntax:
```
type variableName = expression;
```

Examples:
```
int @count = 5;
bool @isEnabled = true;
string @message = "hello";
```

### Expression Statement

Evaluates an expression and discards the result.

Syntax:
```
expression;
```

Examples:
```
@value + 1;
@value = @value + 10;
@isEnabled && @hasFeature;
"computed value: " + @name;
```

### Control Statement

Conditional execution based on boolean expressions.

Syntax:
```
if expression
    statement
[elif expression
    statement]
[else
    statement]
endif
```

Examples:
```
if @useLighting
    @lighting = @value;
else
    @lighting = 0;
endif

if @quality == 0
    @quality = 0;
elif @quality == 1
    @quality = 1;
else
    @quality = 2;
endif
`

### Loop Statement(not supported yet)

## Tasks

PPS programs consist of tasks that define transformations to be applied to source code. 

### Branch Task

Conditional compilation based on boolean expressions.

Syntax:
```
/*<$branch if expression>*/
    ...code...
/*<$branch elif expression>*/
    ...code...
/*<$branch else>*/
    ...code...
/*<$branch endif>*/
`

Variants:
1. `if` ... `endif`
2. `if` ... `else` ... `endif`
3. `if` ... `elif` ... `endif`
4. `if` ... `elif` ... `else` ... `endif`

Example:
```
/*<$branch if @hasBaseColorMap>*/
{
    float4 value = texture(baseColorMap, uv);
    color.rgb *= value.rgb;

    /*<$branch if @useBaseColorMapAlpha>*/
    color.a *= value.a;
    /*<$branch else>*/
    color.a *= 0.5;
    /*<$branch endif>*/
}
/*<$branch endif>*/
```

See [branch](samples/1branch.hlsl)

### Include Task

Includes external files.

Syntax:
```
/*<$include path/to/file>*/
```

Example:
```
/*<$branch if @useBasic>*/
/*<$include util/basic.hlsl>*/
/*<$branch endif>*/
```

See [include](samples/2include.hlsl)

### Override Task

Replaces content at a specific location.

Syntax:
```
... token /*<$override variable>*/ ...
```

Example:
```
SamplerState s_LinearWrap : register(s0 /*<$override @sLinearWrap>*/);
SamplerState s_LinearClamp : register(s1 /*<$override @sLinearClamp>*/);
```

See [override](samples/3override.hlsl)

### Embed Task

Defines and inserts code blocks.

Syntax:
```
// Definition
/*<$embed @variable>*/

// Usage
/*<$embed #variable>*/
```

Example:
```
/*<$branch if @hasEmbedAO>*/
/*<$embed @embedAO>*/
/*<$branch endif>*/

void main(out float4 color)
{
    float ao = 1.0f;
    /*<$branch if @hasEmbedAO>*/
    /*<$embed #embedAO>*/;
    /*<$branch endif>*/

    color *= ao;
}
```

See [embed](samples/4embed.hlsl)

### Prog Task

Executes procedural code for complex logic.

Syntax:
```
/*<$prog start>*/
/*<$prog define type variable>*/
/*<$prog if expression>*/
    /*<$prog variable = expression>*/
/*<$prog elif expression>*/
    /*<$prog variable = expression>*/
/*<$prog else>*/
    /*<$prog variable = expression>*/
/*<$prog endif>*/
/*<$prog export variable>*/
/*<$prog end>*/
```

Example:
```
/*<$prog start>*/
/*<$prog define int @_samples>*/
/*<$prog if @isNodeMode>*/
    /*<$prog @_samples = 1>*/
/*<$prog elif @isSingleMode>*/
    /*<$prog @_samples = @samples>*/
/*<$prog elif @isDoubleMode>*/
    /*<$prog @_samples = 2 * @samples>*/
/*<$prog endif>*/
/*<$prog export @_samples>*/
/*<$prog end>*/
```

See [prog](samples/5prog.hlsl)