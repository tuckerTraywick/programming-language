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


# Parses a token of the given type. Fails if it is not present.
def token(type):
    def parse(tokens, index):
        if index >= len(tokens):
            error = ParsingError(index, f"Expected {type} at {index}, but ran out of tokens.")
            return (index, error, error)
        if tokens[index].type != type:
            error = ParsingError(index, f"Expected {type} at {index}, but got {tokens[index]}.")
            return (index, error, error)
        return (index + 1, tokens[index], None)
    return parse


# Parses a sequence of parsers. Fails if any of the given parsers fail.
def sequence(*parsers):
    def parse(tokens, index):
        children, error = [], None
        for parser in parsers:
            if isinstance(parser, str):
                index, child, error = token(parser)(tokens, index)
            else:
                index, child, error = parser(tokens, index)

            if isinstance(child, list):
                children += child
            elif child is not None:
                children.append(child)

            if error:
                break
        return (index, children, error)
    return parse


# Parses any of the given choices. Returns the error of the last choice if all choices fail.
def choice(*parsers):
    def parse(tokens, index):
        newIndex, result, error = True, None, None
        for parser in parsers:
            if isinstance(parser, str):
                newIndex, result, error = token(parser)(tokens, index)
            else:
                newIndex, result, error = parser(tokens, index)
            
            if not error:
                return (newIndex, result, error)
        return (newIndex, result, error)
    return parse


# Parses a sequence and puts it in a node with the given type. Fails if any of the given parsers 
# fail.
def node(type, *parsers):
    def parse(tokens, index):
        index, children, error = sequence(*parsers)(tokens, index)
        return (index, Node(type, children), error)
    return parse


# Tries the given parsers and just returns None if one fails.
def maybe(*parsers):
    def parse(tokens, index):
        newIndex, result, error = sequence(*parsers)(tokens, index)
        if error:
            return (index, None, None)
        return (newIndex, result, None)
    return parse


# Tries the given parsers and recovers if one fails. Just adds the error message to the parse tree 
# and continues parsing. On failure, consumes tokens until the current token matches the type given.
def recoverUntil(type, stop, message, *parsers):
    def parse(tokens, index):
        index, result, error = sequence(*parsers)(tokens, index)
        oldIndex = index
        if error:
            found = False
            while index < len(tokens):
                if isinstance(type, list) and tokens[index].type in type \
                or tokens[index].type == type:
                    found = True
                    break
                if tokens[index].type == stop:
                    break
                index += 1

            if not found:
                index = oldIndex

            if message:
                token = str(tokens[index]) if index < len(tokens) else ""
                error.message = message.replace("$i", str(index)).replace("$t", token)

            if isinstance(result[-1], Node) and result[-1].children and isinstance(result[-1].children[0], ParsingError):
                result[-1] = result[-1].children[0]
        return (index, result, None)
    return parse


# Same as `recoverUntil()` but doesn't stop at a given token.
def recover(type, message, *parsers):
    return recoverUntil(type, type, message, *parsers)


# Parses zero or more of the given sequence.
def zeroOrMore(*parsers):
    def parse(tokens, index):
        children, newIndex = [], index
        while True:
            newIndex, child, error = sequence(*parsers)(tokens, index)
            if error:
                break
            index = newIndex

            if isinstance(child, list):
                children += child
            else:
                children.append(child)
        return (index, children, None)
    return parse


# Parses one or more of the given sequence.
def oneOrMore(*parsers):
    return sequence(*parsers, zeroOrMore(*parsers))


# The grammar
lineEnd = sequence(
    recover(";", "Expected end of statement.", ";"), zeroOrMore(";")
)

packageName = node("packageName",
    "identifier", zeroOrMore(".", "identifier"),
    maybe(".", recover(";", "Expected identifier or `*`.", "*")),
)

recoverPackageName = recover(";", "Expected package name.", packageName)

importStatement = node("importStatement", choice(
    sequence(
        "from", recoverUntil("import", ";", "Expected package name.", packageName),
        recover(";", "Expected import statement.", "import", packageName), lineEnd,
    ),
    sequence("import", recoverPackageName, lineEnd),
))

programStatement = choice(
    importStatement,
)

packageStatement = node("packageStatement",
    maybe("pub"), "package", recoverPackageName, lineEnd,
)

program = node("program",
    maybe(packageStatement), zeroOrMore(programStatement),
)


# Parses the given tokens into a syntax tree.
def parse(tokens):
    return program(tokens, 0)[1:]
