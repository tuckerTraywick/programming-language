import lexer
from combinators import *


# Token types
keywords = {
    "package",
    "from",
    "import",
    "pub",
    "priv",
    "const",
    "mut",
    "owned",
    "shared",
    "weak",
    "var",
    "fun",
    "struct",
    "alias",
    "cases",
    "embed",
    "impl",
    "pass",
    "return",
    "yield",
    "break",
    "continue",
    "if",
    "else",
    "switch",
    "case",
    "default",
    "fallthrough",
    "for",
    "in",
    "until",
    "thru",
    "by",
    "do",
    "while",
    "as",
    "is",
    "isnot",
    "and",
    "or",
    "xor",
    "not",
    "true",
    "false",
}

operators = {
    "+=",
    "+",
    "-=",
    "->",
    "-",
    "*=",
    "*",
    "/=",
    "/",
    "%=",
    "%",
    "&=",
    "&",
    "|=",
    "|",
    "^=",
    "^",
    "~=",
    "~",
    "<<=",
    "<<",
    "<=",
    "<",
    ">>=",
    ">>",
    ">=",
    ">",
    "==",
    "=>",
    "=",
    "!=",
    ".",
    ",",
    "(",
    ")",
    "[",
    "]",
    "{",
    "}",
}

lineComment = "#"


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
missingWhile = error("Expected a `while` statement.")
missingIn = error("Expected `in` statement.")
invalidPackageName = error("Invalid package name. Expected an identifier or a `*`.")
syntaxError = error("Syntax error.")


# Grammar
structDefinition = ForwardDeclaration()
type = ForwardDeclaration()
programStatement = ForwardDeclaration()
basicExpression = ForwardDeclaration()
expression = expression(basicExpression, 
    {
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
        "as": 95,
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
    }
)

arrow = node("arrow",
    "->",
    choice("identifier", missingIdentifier),
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

elementAccess = node("elementAccess",
    "[",
    zeroOrMore(
        expression,
        ","
    ),
    maybe(expression),
    choice("]", missingCloseParenthesis)
)

memberAccess = node("memberAccess",
    ".",
    choice("identifier", missingMember)
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
            arrow,
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

assignment = node("assignment",
    expression,
    choice(
        "=",
        "+=",
        "-=",
        "*=",
        "/=",
        "%=",
        "~=",
        "<<=",
        ">>=",
        "&="
        "|=",
        "^=",
    ),
    choice(expression, missingExpression),
    lineEnd,
)

block = node("block",
    openBrace,
    zeroOrMore(programStatement),
    closeBrace,
)

forLoop = node("forLoop",
    "for",
    choice(expression, missingExpression),
    choice("in", missingIn),
    choice(
        sequence(
            expression,
            maybe("\n")
        ),
        missingExpression
    ),
    block,
    lineEnd
)

whileLoop = node("whileLoop",
    choice(
        sequence(
            "do",
            choice(
                "while",
                sequence(
                    missingWhile,
                    recover("{")
                )
            )
        ),
        "while"
    ),
    choice(
        sequence(expression, maybe("\n")),
        sequence(missingExpression, recover("{"))
    ),
    block,
    lineEnd
)

ifStatement = node("ifStatement",
    "if",
    choice(
        sequence(expression, maybe("\n")),
        sequence(missingExpression, recover("{"))
    ),
    block,
    zeroOrMore(
        "else",
        "if",
        choice(
            sequence(expression, maybe("\n")),
            sequence(missingExpression, recover("{"))
        ),
        block
    ),
    maybe(
        "else",
        block
    ),
    lineEnd
)

continueStatement = node("continueStatement",
    "continue",
    lineEnd
)

breakStatement = node("breakStatement",
    "break",
    lineEnd
)

returnStatement = node("returnStatement",
    "return",
    choice(expression, missingExpression),
    lineEnd
)

passStatement = node("passStatement",
    "pass",
    lineEnd
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
    ")"
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
    passStatement,
    returnStatement,
    breakStatement,
    continueStatement,
    ifStatement,
    whileLoop,
    forLoop,
    block,
    assignment,
    sequence(
        expression,
        lineEnd
    ),
))

program = node("program",
    maybe(packageStatement),
    zeroOrMore(programStatement),
)


# Lexes the given text into tokens.
def lex(text):
    return lexer.lex(text, keywords, operators, lineComment)


# Parses the given tokens into a syntax tree.
def parse(tokens):
    return program(tokens, 0)[1:3]
