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
    VAR = auto()
    FUNC = auto()
    STRUCT = auto()
    CASES = auto()
    ABSTRACTS = auto()
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
        "var": TokenType.VAR,
        "func": TokenType.FUNC,
        "struct": TokenType.STRUCT,
        "cases": TokenType.CASES,
        "abstracts": TokenType.ABSTRACTS,
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
                tokens.append(Token(TokenType.CHARACTER, start, i))
            else:
                tokens.append(Token(TokenType.INVALID, start, i))
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
                tokens.append(Token(TokenType.CHARACTER, start, i))
            else:
                tokens.append(Token(TokenType.INVALID, start, i))
                break
        # Try to lex a number.
        elif text[i].isdigit():
            start: int = i
            while i < len(text) and text[i].isdigit():
                i += 1
            tokens.append(Token(TokenType.NUMBER, start, i))
        # Try to lex an identifier or keyword.
        elif text[i].isalpha() or text[i] == "_":
            start: int = i
            while i < len(text) and (text[i].isalnum() or text[i] == "_"):
                i += 1
            tokens.append(Token(keywords.get(text[start:i-1], TokenType.IDENTIFIER), start, i))
        # Try to lex an operator.
        elif text[i] in symbols:
            # Try to lex each operator.
            for name, type in operators.items():
                if text.startswith(name, i):
                    tokens.append(Token(type, i, i + len(name)))
                    i += len(name)
                    break
            # If it's not an operator, it's an invalid token.
            else:
                # TODO: Find the length of the invalid token.
                tokens.append(Token(TokenType.INVALID, i, i))
                break # Break out of the outer while loop.
        # If none of the above succeed, the token is invalid.
        else:
            tokens.append(Token(TokenType.INVALID, i, i))
            break

    return tokens


# Splits the file at `path` into tokens.
def lexFile(path: str) -> list[Token]:
    text = readFile(path)
    return lex(text)


# Represents a node in a parse tree. Has a type and zero or more children.
@dataclass
class Node:
    head: Token # The token that represents the type of the node.
    children: list # The node's children. Can be empty.


# Represents a prefix expression. Separate class so I can define prettyprinting methods later.
class PrefixNode(Node):
    pass


# Represents an infix expression.
class InfixNode(Node):
    pass


# Represents the state of the parser. Helper for `parse()`.
class Parser:
    def __init__(self):
        self.tokens: list[Token]
        self.i: int

    # Looks at the next token without consuming it and checks its type.
    def peek(self, type: TokenType=None) -> Token:
        assert self.tokens
        if self.i < len(self.tokens) and (type is None or self.tokens[self.i].type == type):
            return self.tokens[self.i]
        return None
    
    # Consumes the next token and returns it.
    def consume(self) -> Optional[Token]:
        assert self.tokens
        if self.i < len(self.tokens):
            self.i += 1
            return self.tokens[self.i - 1]
        return None
    
    # Parses the given tokens.
    def parse(self, tokens: list[Token], precedence: int=0) -> Node:
        assert all(token.type != TokenType.INVALID for token in tokens)
        self.tokens = tokens
        self.i = 0
        return self.parseExpression()
    
    # Parse an expression. Helper for `self.parse()`.
    def parseExpression(self) -> Token | Node:
        prefix = {
            TokenType.MINUS: 30,
        }
        infix = {
            TokenType.TIMES: 20,
            TokenType.PLUS: 10,
        }

        result = None
        innermost = None
        while self.peek():
            next = self.consume()

            if result is None:
                result = next
                innermost = next
            
            # Try to parse a prefix expression.
            if next.type in prefix:
                result


            # Try to parse a prefix expression.
            if operator.type in prefix:
                result.append((PrefixNode(self.consume(), [self.parseLiteral()]), prefix[operator.type]))
            # Try to parse an infix expression.
            elif operator.type in infix:
                assert result # TODO: Handle infix operator lhs missing.
                lhs, precedence = result[-1]
                self.consume()
                rhs = self.parseLiteral()
                if isinstance(lhs, PrefixNode) or isinstance(lhs, InfixNode):
                    if precedence < infix[operator.type]:
                        # Steal the lhs's last child.
                        lhs.children.append(InfixNode(operator, [lhs.children.pop(), rhs]))
                    else:
                        result.append(InfixNode(operator, [lhs, rhs]))

    # Parses a literal in an expression.
    def parseLiteral(self) -> Node | Token:
        if self.peek(TokenType.NUMBER):
            return self.consume()
        else:
            # TODO: Raise exception.
            pass


# Parses a list of tokens into a syntax tree.
def parse(tokens: list[Token]) -> Node:
    return Parser().parse(tokens)


def main():
    path = "source/example.txt"
    text = readFile(path)
    
    tokens = lex(text)
    print("--- TOKENS ---")
    for i, token in enumerate(tokens):
        print(f"{i:<3} '{text[token.start:token.end]}' {token.type.name}")
    
    tree = parse(tokens)
    print("--- SYNTAX TREE ---")
    print(tree)


if __name__ == "__main__":
    main()
