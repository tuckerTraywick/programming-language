from typing import Optional
from dataclasses import dataclass
from enum import Enum, auto


# Represents the type of a token.
class TokenType(Enum):
    INVALID = auto()
    NUMBER = auto()
    CHARACTER = auto()
    STRING = auto()
    IDENTIFIER = auto()

    PACKAGE = auto()
    IMPORT = auto()
    PUB = auto()
    PRIV = auto()
    VAR = auto()
    FUNC = auto()
    STRUCT = auto()
    CASES = auto()
    EXTEND = auto()
    ABSTRACT = auto()
    PASS = auto()
    RETURN = auto()
    YIELD = auto()
    BREAK = auto()
    CONTINUE = auto()
    IF = auto()
    ELSE = auto()
    SWITCH = auto()
    CASE = auto()
    DEFAULT = auto()
    FALLTHROUGH = auto()
    FOR = auto()
    IN = auto()
    FROM = auto()
    UNTIL = auto()
    THRU = auto()
    BY = auto()
    DO = auto()
    WHILE = auto()
    AS = auto()
    IS = auto()
    AND = auto()
    OR = auto()
    XOR = auto()
    NOT = auto()
    TRUE = auto()
    FALSE = auto()
 
    INCREMENT = auto()
    PLUS_EQUALS = auto()
    PLUS = auto()
    DECREMENT = auto()
    MINUS_EQUALS = auto()
    ARROW = auto()
    MINUS = auto()
    TIMES_EQUALS = auto()
    TIMES = auto()
    DIVIDE_EQUALS = auto()
    DIVIDE = auto()
    MODULUS_EQUALS = auto()
    MODULUS = auto()
    BITWISE_AND_EQUALS = auto()
    BITWISE_AND = auto()
    BITWISE_OR_EQUALS = auto()
    BITWISE_OR = auto()
    BITWISE_XOR_EQUALS = auto()
    BITWISE_XOR = auto()
    BITWISE_NOT_EQUALS = auto()
    BITWISE_NOT = auto()
    LEFT_SHIFT_EQUALS = auto()
    LEFT_SHIFT = auto()
    LESS_EQUAL = auto()
    LESS = auto()
    RIGHT_SHIFT_EQUALS = auto()
    RIGHT_SHIFT = auto()
    GREATER_EQUAL = auto()
    GREATER = auto()
    EQUAL = auto()
    FAT_ARROW = auto()
    ASSIGN = auto()
    NOT_EQUAL = auto()
    DOT = auto()
    COMMA = auto()
    SEMICOLON = auto()
    LEFT_PARENTHESIS = auto()
    RIGHT_PARENTHESIS = auto()
    LEFT_BRACKET = auto()
    RIGHT_BRACKET = auto()
    LEFT_BRACE = auto()
    RIGHT_BRACE = auto()


# Represents a token. Maps to a segment of the input text.
@dataclass
class Token:
    type: TokenType # The type of the token.
    start: int # The index of the start of the token in the text.
    end: int # The index of the end of the token in the text.
    text: str # The text of the token. TODO: DELETE THIS AFTER I NO LONGER NEED TO PRINT TOKENS.

    def __str__(self):
        return f"{self.type.name} '{self.text}'"


# Reads the file at `path` into a string.
def readFile(path: str) -> str:
    file = open(path, "r")
    text = file.read()
    file.close()
    return text


# Splits a string into tokens.
def lex(text: str) -> list[Token]:
    # A map of strings to the keywords they represent.
    keywords = {
        "package": TokenType.PACKAGE,
        "import": TokenType.IMPORT,
        "pub": TokenType.PUB,
        "priv": TokenType.PRIV,
        "var": TokenType.VAR,
        "func": TokenType.FUNC,
        "struct": TokenType.STRUCT,
        "cases": TokenType.CASES,
        "extend": TokenType.EXTEND,
        "abstract": TokenType.ABSTRACT,
        "pass": TokenType.PASS,
        "return": TokenType.RETURN,
        "yield": TokenType.YIELD,
        "break": TokenType.BREAK,
        "continue": TokenType.CONTINUE,
        "if": TokenType.IF,
        "else": TokenType.ELSE,
        "switch": TokenType.SWITCH,
        "case": TokenType.CASE,
        "default": TokenType.DEFAULT,
        "fallthrough": TokenType.FALLTHROUGH,
        "for": TokenType.FOR,
        "in": TokenType.IN,
        "from": TokenType.FROM,
        "until": TokenType.UNTIL,
        "thru": TokenType.THRU,
        "by": TokenType.BY,
        "do": TokenType.DO,
        "while": TokenType.WHILE,
        "as": TokenType.AS,
        "is": TokenType.IS,
        "and": TokenType.AND,
        "or": TokenType.OR,
        "xor": TokenType.XOR,
        "not": TokenType.NOT,
        "true": TokenType.TRUE,
        "false": TokenType.FALSE,
    }
    # A map of strings to the operators they represent.
    operators = {
        "++": TokenType.INCREMENT,
        "+=": TokenType.PLUS_EQUALS,
        "+": TokenType.PLUS,
        "--": TokenType.DECREMENT,
        "-=": TokenType.MINUS_EQUALS,
        "->": TokenType.ARROW,
        "-": TokenType.MINUS,
        "*=": TokenType.TIMES_EQUALS,
        "*": TokenType.TIMES,
        "/=": TokenType.DIVIDE_EQUALS,
        "/": TokenType.DIVIDE,
        "%=": TokenType.MODULUS_EQUALS,
        "%": TokenType.MODULUS,
        "&=": TokenType.BITWISE_AND_EQUALS,
        "&": TokenType.BITWISE_AND,
        "|=": TokenType.BITWISE_OR_EQUALS,
        "|": TokenType.BITWISE_OR,
        "^=": TokenType.BITWISE_XOR_EQUALS,
        "^": TokenType.BITWISE_XOR,
        "~=": TokenType.BITWISE_NOT_EQUALS,
        "~": TokenType.BITWISE_NOT,
        "<<=": TokenType.LEFT_SHIFT_EQUALS,
        "<<": TokenType.LEFT_SHIFT,
        "<=": TokenType.LESS_EQUAL,
        "<": TokenType.LESS,
        ">>=": TokenType.RIGHT_SHIFT_EQUALS,
        ">>": TokenType.RIGHT_SHIFT,
        ">=": TokenType.GREATER_EQUAL,
        ">": TokenType.GREATER,
        "==": TokenType.EQUAL,
        "=>": TokenType.FAT_ARROW,
        "=": TokenType.ASSIGN,
        "!=": TokenType.NOT_EQUAL,
        ".": TokenType.DOT,
        ",": TokenType.COMMA,
        ";": TokenType.SEMICOLON,
        "(": TokenType.LEFT_PARENTHESIS,
        ")": TokenType.RIGHT_PARENTHESIS,
        "[": TokenType.LEFT_BRACKET,
        "]": TokenType.RIGHT_BRACKET,
        "{": TokenType.LEFT_BRACE,
        "}": TokenType.RIGHT_BRACE,
    }
    # Every non-alphanumeric character.
    symbols = "`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?"
    # The start of a line comment.
    lineComment = "#"

    tokens: list[Token] = []
    i: int = 0
    while i < len(text):
        # Skip whitespace.
        if text[i].isspace():
            i += 1
        # Skip comments.
        elif text[i] == lineComment:
            while i < len(text) and text[i] != "\n":
                i += 1
        # Try to lex a character literal.
        elif text[i] == "'":
            # TODO: Account for escapes and check length.
            start: int = i
            # Skip the first quote.
            i += 1
            # Skip the body of the string.
            while i < len(text) and text[i] != "'":
                i += 1
            # Skip the last quote.
            if i < len(text) and text[i] == "'":
                i += 1
                tokens.append(Token(TokenType.CHARACTER, start, i, text[start:i]))
            else:
                tokens.append(Token(TokenType.INVALID, start, i, text[start:i]))
                break
        # Try to lex a string literal.
        elif text[i] == '"':
            # TODO: Account for escapes and check length.
            start: int = i
            # Skip the first quote.
            i += 1
            # Skip the body of the string.
            while i < len(text) and text[i] != '"':
                i += 1
            # Skip the last quote.
            if i < len(text) and text[i] == '"':
                i += 1
                tokens.append(Token(TokenType.CHARACTER, start, i, text[start:i]))
            else:
                tokens.append(Token(TokenType.INVALID, start, i, text[start:i]))
                break
        # Try to lex a number.
        elif text[i].isdigit():
            start: int = i
            while i < len(text) and text[i].isdigit():
                i += 1
            tokens.append(Token(TokenType.NUMBER, start, i, text[start:i]))
        # Try to lex an identifier or keyword.
        elif text[i].isalpha() or text[i] == "_":
            start: int = i
            while i < len(text) and (text[i].isalnum() or text[i] == "_"):
                i += 1
            tokens.append(Token(keywords.get(text[start:i], TokenType.IDENTIFIER), start, i, text[start:i]))
        # Try to lex an operator.
        elif text[i] in symbols:
            # Try to lex each operator.
            for name, type in operators.items():
                if text.startswith(name, i):
                    tokens.append(Token(type, i, i + len(name), text[start:i]))
                    i += len(name)
                    break
            # If it's not an operator, it's an invalid token.
            else:
                # TODO: Find the length of the invalid token.
                tokens.append(Token(TokenType.INVALID, i, i, text[start:i]))
                break # Break out of the outer while loop.
        # If none of the above succeed, the token is invalid.
        else:
            tokens.append(Token(TokenType.INVALID, i, i, text[start:i]))
            break

    return tokens


# Splits the file at `path` into tokens.
def lexFile(path: str) -> list[Token]:
    text = readFile(path)
    return lex(text)


# Represents the type of a node.
class NodeType(Enum):
    PROGRAM = auto()
    PACKAGE_STATEMENT = auto()


# Represents a node in a parse tree. Has a type and zero or more children.
@dataclass
class Node:
    type: NodeType # The type of the node.
    parent: "Node" # The parent of the node.
    children: list # The node's children. Can be empty.

    def prettyPrint(self, indentation: int=0) -> str:
        tab = "|  "
        print(tab*indentation + self.type.name)
        indentation += 1
        for child in self.children:
            if isinstance(child, Node):
                child.prettyPrint(indentation)
            else:
                print(tab*indentation + str(child))


# Represents the state of the parser. Helper for `parse()`.
class Parser:
    def __init__(self):
        self.tokens: list[Token]
        self.i: int
        self.tree: Optional[Node]
        self.innermost: Optional[Node]
        self.errorMessage: Optional[str]

    # Accepts the given token if it is present, but doesn't throw an error if it's not.
    def accept(self, type: TokenType=None) -> bool:
        assert self.tokens, "Must have tokens to accept a token."
        assert self.innermost is not None, "Must have a node to append a child."
        if self.errorMessage is None and self.i < len(self.tokens) and self.tokens[self.i].type == type:
            self.innermost.children.append(self.tokens[self.i])
            self.i += 1
            return True
        return False

    # Expects the given token and adds an error to the parse tree if not.
    def expect(self, type: TokenType=None) -> bool:
        assert self.tokens, "Must have tokens to expect a token."
        assert self.innermost is not None, "Must have a node to append a child."
        if self.errorMessage is not None:
            return False
        
        if self.i >= len(self.tokens):
            self.errorMessage = f"Expected {type.name} at {self.i} but ran out of tokens."
            return False
        
        if self.tokens[self.i].type != type:
            self.errorMessage = f"Expected {type.name} at {self.i} but got {self.tokens[self.i].type.name}."
            return False

        self.innermost.children.append(self.tokens[self.i])
        self.i += 1
        return True

    # Starts a new node to the parse tree with the given type. Adds it as a child to the innermost node.
    def startNode(self, type: NodeType):
        if self.errorMessage is not None:
            return
        
        if self.tree is None:
            self.tree = Node(type, None, [])
            self.innermost = self.tree
        else:
            newInnermost = Node(type, self.innermost, [])
            self.innermost.children.append(newInnermost)
            self.innermost = newInnermost

    # Ends the current node and steps up one level in the parse tree.
    def endNode(self):
        assert self.tree is not None, "Must have a parse tree to end a node."
        # Only end the node if it's not the topmost one.
        if self.innermost is not self.tree:
            self.innermost = self.innermost.parent

    # Runs the given parser and resets if it fails.
    def maybe(self, parser):
        previousI = self.i
        previousInnermost = self.innermost
        parser()
        if self.errorMessage:
            self.i = previousI
            self.innermost = previousInnermost

    # Parses the given tokens.
    def parse(self, tokens: list[Token]) -> Node:
        assert all(token.type != TokenType.INVALID for token in tokens), "Cannot parse invalid tokens yet."
        self.tokens = tokens
        self.i = 0
        self.tree = None
        self.innermost = None
        self.errorMessage = None
        self.program()
        return self.tree
    
    # Parses a program. Helper for `self.parse()`.
    def program(self):
        self.startNode(NodeType.PROGRAM)
        self.packageStatement()
        self.endNode()

    # Parses a `package` statement. Helper for `self.parse()`
    def packageStatement(self):
        self.startNode(NodeType.PACKAGE_STATEMENT)
        self.accept(TokenType.PUB)
        self.expect(TokenType.PACKAGE)
        self.expect(TokenType.IDENTIFIER)
        self.endNode()
    

# Parses a list of tokens into a syntax tree.
def parse(tokens: list[Token]) -> Node:
    return Parser().parse(tokens)


def main():
    path = "source/example.txt"
    text = readFile(path)
    
    tokens = lex(text)
    print("--- TOKENS ---")
    for i, token in enumerate(tokens):
        print(f"{i:<3} {token}")
    
    parser = Parser()
    tree = parser.parse(tokens)
    print("\n--- SYNTAX TREE ---")
    if parser.errorMessage:
        print("Error message:", parser.errorMessage + "\n")
    tree.prettyPrint()


if __name__ == "__main__":
    main()
