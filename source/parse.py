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
    def parse(tokens, index, recovered):
        if index >= len(tokens):
            error = ParsingError(index, f"Expected {type} at {index}, but ran out of tokens.")
            return (index, error, error, False)
        
        if recovered:
            # Fastforward to the expected token or the end of the line.
            # If neither is encountered, rewind to the previous index.
            oldIndex = index
            while index < len(tokens):
                if tokens[index].type == type:
                    break

                if tokens[index].type == ";":
                    break
                index += 1
            
            if index == len(tokens):
                index = oldIndex

        if tokens[index].type != type:
            error = ParsingError(index, f"Expected {type} at {index}, but got {tokens[index]}.")
            return (index, error, error, False)
        return (index + 1, tokens[index], None, False)
    return parse


# Parses a sequence of parsers. Fails if any of the given parsers fail.
def sequence(*parsers):
    assert parsers
    def parse(tokens, index, recovered):
        children = []
        for parser in parsers:
            if isinstance(parser, str):
                index, child, error, recovered = token(parser)(tokens, index, recovered)
            else:
                index, child, error, recovered = parser(tokens, index, recovered)

            if isinstance(child, list):
                children += child
            elif child is not None:
                children.append(child)

            if error:
                break
        return (index, children, error, recovered)
    return parse


# Parses any of the given choices. Returns the error of the last choice if all choices fail.
def choice(*parsers):
    assert parsers
    def parse(tokens, index, recovered):
        for parser in parsers:
            if isinstance(parser, str):
                newIndex, result, error, newRecovered = token(parser)(tokens, index, recovered)
            else:
                newIndex, result, error, newRecovered = parser(tokens, index, recovered)
            
            if not error:
                return (newIndex, result, error, newRecovered)
        return (newIndex, result, error, newRecovered)
    return parse


# Parses a sequence and puts it in a node with the given type. Fails if any of the given parsers 
# fail.
def node(type, *parsers):
    assert parsers
    def parse(tokens, index, recovered):
        index, children, error, recovered = sequence(*parsers)(tokens, index, recovered)
        return (index, Node(type, children), error, recovered)
    return parse


# Tries the given parsers and just returns None if one fails.
def maybe(*parsers):
    assert parsers
    def parse(tokens, index, recovered):
        index, result, error, recovered = sequence(*parsers)(tokens, index, recovered)
        return (index, None if error else result, None, recovered)
    return parse


# Tries the given parsers and recovers if one fails. Just adds the error message to the parse tree.
def recover(*parsers):
    assert parsers
    def parse(tokens, index, recovered):
        index, result, error, recovered = sequence(*parsers)(tokens, index, recovered)
        # if error:
            # # TODO: Make this more legible.
            # if isinstance(result[-1], Node) and result[-1].children and isinstance(result[-1].children[0], ParsingError):
            #     result[-1] = result[-1].children[0]
        return (index, result, None, bool(error))
    return parse


# Skips tokens until `;`.
def lineEnd(tokens, index, recovered):
    if index < len(tokens) and tokens[index].type == ";":
        return (index + 1, tokens[index], None, False)

    found = False
    newIndex = index
    while newIndex < len(tokens):
        if tokens[newIndex].type != ";":
            found = True
            break
        newIndex += 1

    # TODO: Figure out what to do next.
    if found:
        index

    if not recovered:
        error = ParsingError(index, "Expected end of statement.")
    else:
        error = None

    
    return ()


# Parses zero or more of the given sequence.
def zeroOrMore(*parsers):
    assert parsers
    def parse(tokens, index, recovered):
        children = []
        while True:
            newIndex, child, error, newRecovered = sequence(*parsers)(tokens, index, recovered)
            if error:
                break
            index = newIndex
            recovered = newRecovered

            if isinstance(child, list):
                children += child
            else:
                children.append(child)
        return (index, children, None, recovered)
    return parse


# Parses one or more of the given sequence.
# def oneOrMore(*parsers):
#     return sequence(*parsers, zeroOrMore(*parsers))


# The grammar
#lineEnd = sequence(recover(";"), maybe(";"))

variableDefinition = node("variableDefinition",
    maybe("pub"), "var", recover("identifier"), maybe("identifier"), maybe("=", "number"), lineEnd,
)

packageName = node("packageName",
    "identifier", zeroOrMore(".", "identifier"), maybe(".", recover("*")),
)

packageNameList = sequence(
    packageName, zeroOrMore(",", packageName),
)

importStatement = node("importStatement", choice(
    sequence("from", recover(packageName), recover("import", packageNameList), lineEnd),
    sequence("import", recover(packageName), lineEnd)
))

programStatement = choice(
    importStatement,
    variableDefinition,
)

packageStatement = node("packageStatement",
    maybe("pub"), "package", recover(packageName), lineEnd,
)

program = node("program",
    maybe(packageStatement), zeroOrMore(programStatement),
)


# Parses the given tokens into a syntax tree.
def parse(tokens):
    return program(tokens, 0, False)[1:3]
