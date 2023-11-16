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


# Parses a sequence and puts it in a node with the given type. Fails if any of the given parsers fail.
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


# # Tries the given parsers and recovers if one fails. Just adds the error message to the parse tree and continues parsing.
# def recover(*parsers):
#     def parse(tokens, index):
#         index, result, _ = sequence(*parsers)(tokens, index)
#         return (index, result, None)
#     return parse


# Tries the given parsers and recovers if one fails. Just adds the error message to the parse tree and continues parsing.
# On failure, consumes tokens until the current token matches the type given.
def recoverUntil(type, message, *parsers):
    def parse(tokens, index):
        index, result, error = sequence(*parsers)(tokens, index)
        if error:
            while index < len(tokens) and tokens[index].type != type:
                index += 1

            if message:
                error.message = message.replace("$i", str(index)).replace("$t", str(tokens[index]))
        return (index, result, None)
    return parse


# Parses zero or more of the given sequence.
def zeroOrMore(*parsers):
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


# Parses one or more of the given sequence.
def oneOrMore(*parsers):
    return sequence(*parsers, zeroOrMore(*parsers))


# The grammar.
lineEnd = ";"
packageStatement = node("packageStatement", maybe("pub"), "package", recoverUntil(lineEnd, "Expected a package name.", "identifier"), lineEnd)
program = node("program", maybe(packageStatement))


# Parses the given tokens into a syntax tree.
def parse(tokens):
    return program(tokens, 0)[1:]
