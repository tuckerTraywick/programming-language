from dataclasses import dataclass
from lex import *


# Represents a node in a parse tree. Can have zero or more children.
@dataclass
class Node:
    type: str # The type of the node.
    children: list # The children of the node. May be empty.

    # The characters parsed by the node (excluding whitespace and newlines).
    @property
    def text(self):
        result = ""
        for child in self.children:
            if isinstance(child, Node):
                result += child.text
            elif isinstance(child, str):
                result += child
        return result
    
    def __str__(self):
        children = ", ".join(map(str, self.children))
        return f"{self.type}({children})"

    # Prints a multi-line representation of the node.
    def prettyPrint(self, indentation=0):
        tab = "|  "
        print(tab*indentation + self.type)
        indentation += 1
        for child in self.children:
            if isinstance(child, Node):
                child.prettyPrint(indentation)
            else:
                print(tab*indentation + str(child))


# Represents a non-recoverable error encountered during parsing.
@dataclass
class ParsingError:
    index: int # The token the error occurred at.
    message: str # An explanation of the error.

    def __str__(self):
        return f"Error (token {self.index}): {self.message}"


# Represents a parsing rule yet to be defined. Used to make indirectly recursive rules.
class ForwardDeclaration:
    def __init__(self):
        self.parser = None

    def __call__(self, *args):
        return self.parser(*args)
    
    # Defines the rule.
    def define(self, parser):
        self.parser = parser


# Parses a token of the given type. Fails if it is not present.
def token(type):
    def parse(tokens, index):
        if index >= len(tokens):
            error = ParsingError(index, f"Expected {repr(type)}, but ran out of tokens.")
            return (index, error, error)

        if tokens[index].type != type:
            error = ParsingError(index, f"Expected {repr(type)}, but got {tokens[index]}.")
            return (index, error, error)
        return (index + 1, tokens[index] if tokens[index].type != "\n" else [], None)
    return parse


# Parses a sequence of parsers. Fails if any of the given parsers fail.
def sequence(*parsers):
    assert parsers
    def parse(tokens, index):
        oldIndex = index
        children = []
        for parser in parsers:
            if isinstance(parser, str):
                parser = token(parser)
            index, child, error = parser(tokens, index)


            if isinstance(child, list):
                children += child
            elif child:
                children.append(child)

            if error:
                return (oldIndex, children, error)
        return (index, children, error)
    return parse


# Parses any of the given choices. Returns the error of the last choice if all choices fail.
def choice(*parsers):
    assert parsers
    def parse(tokens, index):
        for parser in parsers:
            if isinstance(parser, str):
                parser = token(parser)
            newIndex, result, error = parser(tokens, index)
            
            if not error:
                return (newIndex, result, error)
        return (index, result, error)
    return parse


# Parses a sequence and puts it in a node with the given type. Fails if any of the given parsers 
# fail.
def node(type, *parsers):
    assert parsers
    def parse(tokens, index):
        index, children, error = sequence(*parsers)(tokens, index)
        return (index, Node(type, children), error)
    return parse


# Tries the given parsers and just returns None if one fails.
def maybe(*parsers):
    assert parsers
    def parse(tokens, index):
        index, result, error = sequence(*parsers)(tokens, index)
        if error:
            return (index, None, None)
        return (index, result, error)
    return parse


# Adds the given error message to the parse tree and recovers.
def error(message):
    def parse(tokens, index):
        return (index, ParsingError(index, message), None)
    return parse


# Skips tokens until `\n`.
def lineEnd(tokens, index):
    if index >= len(tokens):
        error = ParsingError(index, "Expected new line, but ran out of tokens.")
        return (index, error, error)
    
    if tokens[index].type == "\n":
        return (index + 1, [], None)

    error = ParsingError(index, "Expected end of statement.")
    while index < len(tokens):
        if tokens[index].type == "\n":
            return (index + 1, error, None)
        index += 1
    return (index, error, error)


# Skips until the given token is encountered.
def recover(*types):
    def parse(tokens, index):
        while index <= len(tokens) and tokens[index].type not in types:
            index += 1
        return (index, None, None)
    return parse


# Parses zero or more of the given sequence.
def zeroOrMore(*parsers):
    assert parsers
    def parse(tokens, index):
        children = []
        while True:
            index, child, error = sequence(*parsers)(tokens, index)
            if error:
                break

            if isinstance(child, list):
                children += child
            else:
                children.append(child)
        return (index, children, None)
    return parse


# Parses an expression using the given operator precedences with the given literal parser.
def expression(basicExpression, infix, prefix):
    def parsePrefix(tokens, index):
        operators = []
        # Parse zero or more prefix operators.
        while index < len(tokens) and tokens[index].type in prefix:
            operators.append(tokens[index])
            index += 1

        index, operand, error = basicExpression(tokens, index)
        if error:
            return (index, error, error)
        
        if operators:
            return (index, Node("prefixExpression", operators + [operand]), error)
        return (index, operand, error)
    
    def parseInfix(tokens, index, precedence, lhs):
        children = [lhs]
        while index < len(tokens):
            operator = tokens[index]
            if operator.type not in infix:
                break
                
            index += 1
            if infix[operator.type] > precedence:
                index, rhs, error = parsePrefix(tokens, index)
                if error:
                    return (index, error, error)
                
                index, result, error = parseInfix(tokens, index, infix[operator.type] + 1, rhs)
                if error:
                    return (index, error, error)
                children += [operator, result]
            elif infix[operator.type] < precedence:
                index -= 1
                break
            else:
                index, rhs, error = parsePrefix(tokens, index)
                if error:
                    return (index, error, error)
                children += [operator, rhs]
        
        if len(children) > 1:
            return (index, Node("infixExpression", children), None)
        return (index, children[0], None)

    def parse(tokens, index):
        index, lhs, error = parsePrefix(tokens, index)
        if error:
            return (index, error, error)
        return parseInfix(tokens, index, 0, lhs)
    return parse


# Error messages
missingCloseParenthesis = error("Expected a closing `)`.")
missingCloseBrace = error("Expected a closing `}`.")
missingCloseBracket = error("Expected a closing `]`.")
missingIdentifier = error("Expected an identifier.")
missingMember = error("Expected a member name.")
missingDecimalDigits = error("Expected at least one digit after `.`.")
missingPackageName = error("Expected a package name.")
missingImportStatement = error("Expected an import statement.")
missingPackageNameList = error("Expected a list of package names or a `*`.")
missingExpression = error("Expected an expression.")
missingVariableName = error("Expected a variable name.")
missingFunctionName = error("Expected a function name.")
missingFunctionParameters = error("Expected function parameters.")
missingTypeOrDefaultValue = error("Expected an argument type and/or default value.")
missingFunctionBody = error("Expected a function body.")
missingStructName = error("Expected a struct name.")
missingStructCase = error("Expected a struct case.")
missingCases = error("Expected struct cases.")
missingCaseName = error("Expected a case name.")
missingType = error("Expected a type.")
invalidPackageName = error("Invalid package name. Expected an identifier or a `*`.")
syntaxError = error("Syntax error.")


# Grammar
structDefinition = ForwardDeclaration()
type = ForwardDeclaration()
programStatement = ForwardDeclaration()
basicExpression = ForwardDeclaration()
expression = expression(basicExpression, 
    {
        "->": 210,
        "as": 200,
        "*": 190,
        "/": 180,
        "%": 170,
        "+": 160,
        "-": 150,
        "<<": 140,
        ">>": 130,
        "&": 120,
        "|": 110,
        "^": 100,
        "is": 90,
        "==": 80,
        "!=": 70,
        ">": 60,
        ">=": 50,
        "<": 40,
        "<=": 30,
        "and": 20,
        "or": 10,
        "xor": 10,
    }, 
    {
        "+",
        "-",
        "&",
        "*",
        "~",
        "not",
        "new",
    }
)

memberAccess = node("memberAccess",
    ".",
    choice("identifier", missingMember)
)

elementAccess = node("elementAccess",
    "[",
    zeroOrMore(
        expression,
        ","
    ),
    maybe(expression),
    choice("]", missingCloseParenthesis)
)

functionCall = node("functionCall",
    "(",
    zeroOrMore(
        expression,
        ","
    ),
    maybe(expression),
    choice(")", missingCloseParenthesis)
)

booleanLiteral = node("booleanLiteral",
    choice(
        "true",
        "false"
    )
)

numberLiteral = node("numberLiteral",
    maybe(
        choice(
            "+",
            "-"
        )
    ),
    choice(
        sequence(
            "number",
            maybe(
                ".",
                maybe("number")
            )
        ),
        sequence(
            ".",
            choice("number", missingDecimalDigits)
        )
    ),
)

stringLiteral = node("stringLiteral",
    "string"
)

characterLiteral = node("characterLiteral",
    "character"
)

arrayLiteral = node("arrayLiteral",
    "[",
    zeroOrMore(
        expression,
        ",",
    ),
    maybe(expression),
    choice("]", missingCloseParenthesis)
)

tupleLiteral = node("tupleLiteral",
    "(",
    zeroOrMore(
        expression,
        ",",
    ),
    maybe(expression),
    choice(")", missingCloseParenthesis)
)

literal = choice(
    tupleLiteral,
    arrayLiteral,
    characterLiteral,
    stringLiteral,
    numberLiteral,
    booleanLiteral,
    "identifier",
)

basicExpression.define(node("basicExpression",
    literal,
    zeroOrMore(
        choice(
            memberAccess,
            elementAccess,
            functionCall,
        )
    )
))

basicType = node("basicType",
    "identifier",
    zeroOrMore(
        ".",
        choice("identifier", missingIdentifier)
    )
)

tupleType = node("tupleType",
    "(",
    zeroOrMore(
        type,
        ","
    ),
    maybe(type),
    choice(")", missingCloseParenthesis)
)

arrayType = node("arrayType",
    "[",
    maybe(expression),
    choice("]", missingCloseBracket),
    choice(type, missingType)
)

pointerType = node("pointerType",
    "&",
    choice(type, missingType),
)

mutType = node("mutType",
    "mut",
    choice(type, missingType)
)

constType = node("constType",
    "const",
    choice(type, missingType)
)

type.define(node("type",
    choice(
        constType,
        mutType,
        pointerType,
        arrayType,
        tupleType,
        basicType,
    )
))

accessModifier = maybe(
    choice(
        "pub",
        "priv"
    )
)

openBrace = sequence(
    "{",
    maybe("\n")
)

closeBrace = sequence(
    maybe("\n"),
    "}",
)

variableDefinition = node("variableDefinition",
    accessModifier,
    "var",
    choice(
        sequence(
            "identifier",
            choice(
                sequence(
                    type,
                    maybe(
                        "=",
                        choice(expression, missingExpression)
                    )
                ),
                sequence(
                    "=",
                    choice(expression, missingExpression)
                )
            )
        ),
        missingVariableName
    ),
    lineEnd
)

functionBody = node("functionBody",
    openBrace,
    zeroOrMore(programStatement),
    closeBrace
)

functionParameter = node("functionParameter",
    "identifier",
    choice(
        sequence(
            type,
            maybe(
                "=",
                choice(expression, missingExpression)
            )
        ),
        sequence(
            "=",
            choice(expression, missingExpression)
        ),
        missingTypeOrDefaultValue
    )
)

functionParameters = node("functionParameters",
    "(",
    zeroOrMore(
        functionParameter,
        ","
    ),
    maybe(functionParameter),
    ")" # choice(")", missingCloseParenthesis),
)

functionSignature = node("functionSignature",
    accessModifier,
    "fun",
    choice(
        sequence(
            "identifier",
            choice(functionParameters, missingFunctionParameters),
            maybe(type),
        ),
        missingFunctionName
    ),
    lineEnd
)

functionDefinition = node("functionDefinition",
    accessModifier,
    "fun",
    choice(
        sequence(
            "identifier",
            choice(
                sequence(
                    functionParameters,
                    maybe(type),
                    choice(functionBody, missingFunctionBody)
                ),
                sequence(
                    missingFunctionParameters,
                    recover("{"),
                    choice(
                        functionBody,
                        sequence(missingFunctionBody, recover("}"))
                    )
                )
            )
            # choice(functionParameters, missingFunctionParameters),
            # maybe(type),
            # choice(functionBody, missingFunctionBody)
        ),
        missingFunctionName
    ),
    lineEnd
)

structCase = node("structCase",
    choice(
        sequence(
            "case",
            choice(
                sequence(
                    "identifier",
                    maybe(
                        "=",
                        choice(expression, missingExpression)
                    )
                ),
                missingCaseName
            ),
            lineEnd
        ),
        structDefinition
    ),
)

structCases = node("structCases",
    "cases",
    openBrace,
    maybe(
        "default",
        choice(
            structCase,
            sequence(
                missingStructCase,
                lineEnd
            )
        )
    ),
    zeroOrMore(structCase),
    choice(closeBrace, missingCloseBrace),
)

structMember = choice(
    sequence(
        "embed",
        choice(
            type,
            missingType
        ),
        lineEnd
    ),
    functionSignature,
    variableDefinition
)

structBody = node("structBody",
    openBrace,
    zeroOrMore(structMember),
    choice(closeBrace, missingCloseBrace)
)

structDefinition.define(node("structDefinition",
    accessModifier,
    "struct",
    choice("identifier", missingStructName),
    maybe(structBody),
    maybe(structCases),
    lineEnd
))

packageName = node("packageName",
    "identifier",
    zeroOrMore(".", "identifier"),
    maybe(
        ".",
        choice("*", invalidPackageName)
    )
)

packageNameList = sequence(
    packageName,
    zeroOrMore(
        ",",
        maybe("\n"),
        packageName
    ),
    maybe(",")
)

importStatement = node("importStatement",
    choice(
        sequence(
            "from",
            choice(packageName, missingPackageName),
            choice(
                sequence(
                    "import",
                    choice(
                        "*",
                        packageNameList, 
                        missingPackageNameList
                    )
                ),
                missingImportStatement,
            ),
            lineEnd
        ),
        sequence(
            "import",
            choice(packageName, missingPackageName),
            lineEnd
        )
    )
)

packageStatement = node("packageStatement",
    maybe("pub"),
    "package",
    choice(packageName, missingPackageName),
    lineEnd
)

programStatement.define(choice(
    importStatement,
    structDefinition,
    functionDefinition,
    variableDefinition,
    sequence(
        expression,
        lineEnd
    ),
))

program = node("program",
    maybe(packageStatement),
    zeroOrMore(programStatement),
)


# Parses the given tokens into a syntax tree.
def parse(tokens):
    return program(tokens, 0)[1:3]
