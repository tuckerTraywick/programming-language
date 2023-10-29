from dataclasses import dataclass
from enum import Enum, auto


# Represents the type of a token.
class TokenType(Enum):
    INVALID = auto()
    NUMBER = auto()
    CHARACTER = auto()
    STRING = auto()
    IDENTIFIER = auto()
    PLUS = auto()
    MINUS = auto()


# Represents a token. Maps to a segment of the input text.
@dataclass
class Token:
    type: TokenType # The type of the token.
    start: int # The index of the start of the token in the text.
    end: int # The index of the end of the token in the text.


# Splits a string into tokens.
def lex(text: str) -> list[Token]:
    # Every non-alphanumeric character.
    symbols = "`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?"
    # A map of token strigs to their operator types.
    operators = {
        "+": TokenType.PLUS,
        "-": TokenType.MINUS,
    }

    tokens: list[Token] = []
    currentToken: Token = None
    for i, char in enumerate(text):
        # If not currently lexing a token, figure out the type of the current token.
        if currentToken is None:
            if char.isspace():
                continue
            elif char.isdigit():
                currentToken = Token(TokenType.NUMBER, i, i)
            elif char.isalpha() or char == "_":
                currentToken = Token(TokenType.IDENTIFIER, i, i)
            elif char in symbols:
                currentToken = Token(None, i, i)
            else:
                currentToken = Token(TokenType.INVALID, i, i)
        # If in the middle of lexing a token, try to keep lexing it.
        elif currentToken.type == TokenType.NUMBER and char.isdigit() \
             or currentToken.type == TokenType.IDENTIFIER and (char.isalnum() or char == '_') \
             or currentToken.type == None and (char in symbols):
                currentToken.end = i
            

# Splits the file at `path` into tokens.
def lexFile(path: str) -> list[Token]:
    file = open(path, "r")
    text = file.read()
    file.close()
    return lex(text)


def main():
    path = "source/example.txt"
    print(lexFile(path))


if __name__ == "__main__":
    main()
