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
        return (index + 1, tokens[index], None)
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
        return (index + 1, tokens[index], None)

    error = ParsingError(index, "Expected end of statement.")
    while index < len(tokens):
        if tokens[index].type == "\n":
            return (index + 1, [error, tokens[index]], None)
        index += 1
    return (index, error, error)


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


# Error messages
missingPackageName = error("Expected package name.")
missingImportStatement = error("Expected import statement.")
missingPackageNameList = error("Expected a list of package names.")
missingExpression = error("Expected expression.")
missingCloseParenthesis = error("Expected closing `)`.")
missingCloseBrace = error("Expected closing `}`.")
missingVariableName = error("Expected variable name.")
missingFunctionName = error("Expected function name.")
missingFunctionParameters = error("Expected function parameters.")
missingFunctionBody = error("Expected function body.")
missingStructName = error("Expected struct name.")
missingCaseName = error("Expected case name.")
missingType = error("Expected type.")
invalidPackageName = error("Invalid package name. Expected identifier or `*`.")
syntaxError = error("Syntax error.")
# lineEnd = "\n"


# Grammar
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
                    choice(packageNameList, missingPackageNameList)
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

program = node("program",
    packageStatement,
    importStatement,
)


# Parses the given tokens into a syntax tree.
def parse(tokens):
    return program(tokens, 0)[1:3]
