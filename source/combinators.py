from dataclasses import dataclass
from lex import *


# Represents a node in a parse tree. Can have zero or more children.
@dataclass
class Node:
    type: str # The type of the node.
    children: list # The children of the node. May be empty.

    # The characters parsed by the node (excluding whitespace and newlines).
    @property
    def text(self) -> str:
        result = ""
        for child in self.children:
            if isinstance(child, Node):
                result += child.text
            elif isinstance(child, str):
                result += child
        return result
    
    def __str__(self) -> str:
        children = ", ".join(map(str, self.children))
        return f"{self.type}({children})"

    # Prints a multi-line representation of the node.
    def prettyPrint(self, indentation: int=0) -> str:
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
    message: str # An explanation of the error.


# Represents a recoverable error encountered during parsing.
@dataclass
class RecoverableParsingError:
    message: str # An explanation of the error.


# Parses a token of the given type. Fails if it is not present.
def token(type: str):
    def parse(tokens: list[Token], index: int) -> (int, Token):
        if index >= len(tokens):
            return (index, ParsingError(f"Expected {type} at {index}, but ran out of tokens."))
        if tokens[index].type != type:
            return (index, ParsingError(f"Expected {type} at {index}, but got {tokens[index].type}."))
        return (index + 1, tokens[index])
    return parse


# Parses a sequence of parsers. Fails if any of the given parsers fail.
def sequence(*parsers):
    def parse(tokens: list[Token], index: int) -> (int, list[Node | Token]):
        children = []
        for parser in parsers:
            if isinstance(parser, str):
                index, child = token(parser)(tokens, index)
            else:
                index, child = parser(tokens, index)

            if isinstance(child, ParsingError):
                return (index, child)
            
            if isinstance(child, list):
                children += child
            elif child is not None:
                children.append(child)
        return (index, children)
    return parse


# Parses any of the given choices. Returns the error of the last choice if all choices fail.
def choice(*parsers):
    def parse(tokens: list[Token], index: int) -> (int, Node | Token):
        newIndex, result = None, None
        for parser in parsers:
            if isinstance(parser, str):
                newIndex, result = token(parser)(tokens, index)
            else:
                newIndex, result = parser(tokens, index)
            
            if not isinstance(result, ParsingError):
                return (newIndex, result)
        return (newIndex, result)
    return parse


# Parses a sequence and puts it in a node with the given type. Fails if any of the given parsers fail.
def node(type: str, *parsers):
    def parse(tokens: list[Token], index: int) -> (int, Node):
        index, children = sequence(*parsers)(tokens, index)
        if isinstance(children, ParsingError):
            return (index, children)
        return Node(type, children)
    return parse


# Tries the given parsers and just returns None if one fails.
def maybe(*parsers):
    def parse(tokens: list[Token], index: int) -> (int, list[Node | Token]):
        index, result = sequence(*parsers)(tokens, index)
        return (index, None if isinstance(result, ParsingError) else result)
    return parse


# Tries the given parsers and recovers if one fails. Just adds the given error message to the parse tree and continues parsing.
def recoverMessage(message: str, *parsers):
    def parse(tokens: list[Token], index: int) -> (int, list[Node | Token]):
        index, result = sequence(*parsers)(tokens, index)
        if isinstance(result, ParsingError):
            if message is None:
                return (index, RecoverableParsingError(result.message))
            newMessage = message.replace("$i", str(index)).replace("$t", str(tokens[index]))
            return (index, RecoverableParsingError(newMessage))
        return (index, result)
    return parse


# Tries the given parsers and recovers if one fails. Just adds the generated error message to the parse tree and continues parsing.
def recover(*parsers):
    return recoverMessage(None, *parsers)


# Parses zero or more of the given sequence.
def zeroOrMore(*parsers):
    def parse(tokens: list[Token], index: int) -> (int, list[Node | Token]):
        children = []
        while True:
            index, child = sequence(*parsers)(tokens, index)
            if isinstance(child, ParsingError):
                break

            if isinstance(child, list):
                children += child
            else:
                children.append(child)
        return (index, children)
    return parse


# Parses one or more of the given sequence.
def oneOrMore(*parsers):
    return sequence(*parsers, zeroOrMore(*parsers))


def main():
    parser = sequence("identifier", recover("number"))
    text = "a b 1"
    tokens = lex(text)
    result = parser(tokens, 0)
    print(f"Text: '{text}'")
    print(f"Tokens: {tokens}")
    print(f"Result: {result}")


if __name__ == "__main__":
    main()
