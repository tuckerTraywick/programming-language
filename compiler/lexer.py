from dataclasses import dataclass


# Represents a token. Maps to a segment of the input text.
@dataclass
class Token:
    type: str # The type of the token.
    text: str # The text of the token.

    def __str__(self) -> str:
        return ("" if self.type == self.text else self.type + " ") + repr(self.text)


# Reads the file at `path` into a string.
def readFile(path: str) -> str:
    file = open(path, "r")
    text = file.read()
    file.close()
    return text


# Splits a string into tokens.
def lex(text: str, keywords, operators, lineComment, ignoreNewlines=False) -> list[Token]:
    # Every non-alphanumeric character.
    symbols = "`~!@#$%^&*()-=+[{]}\\|;:'\",<.>/?"

    tokens: list[Token] = []
    i: int = 0
    while i < len(text):
        # Lex newlines.
        if text[i] == "\n":
            if not tokens or (not ignoreNewlines and tokens[-1].type != "\n"):
                tokens.append(Token("\n", "\n"))
            i += 1
        # Skip whitespace.
        elif text[i].isspace():
            i += 1
        # Skip comments.
        elif text[i] == lineComment:
            while i < len(text) and text[i] != "\n":
                i += 1
        # Try to lex a character literal.
        elif text[i] == "'":
            # TODO: Account for escapes and check length.
            start = i
            # Skip the first quote.
            i += 1
            # Skip the body of the string.
            while i < len(text) and text[i] != "'":
                i += 1
            # Skip the last quote.
            if i < len(text) and text[i] == "'":
                i += 1
                tokens.append(Token("character", text[start:i]))
            else:
                tokens.append(Token("invalid", text[start:i]))
                break
        # Try to lex a string literal.
        elif text[i] == '"':
            # TODO: Account for escapes and check length.
            start = i
            # Skip the first quote.
            i += 1
            # Skip the body of the string.
            while i < len(text) and text[i] != '"':
                i += 1
            # Skip the last quote.
            if i < len(text) and text[i] == '"':
                i += 1
                tokens.append(Token("string", text[start:i]))
            else:
                tokens.append(Token("invalid", text[start:i]))
                break
        # Try to lex a number.
        elif text[i].isdigit():
            start = i
            while i < len(text) and text[i].isdigit():
                i += 1
            tokens.append(Token("number", text[start:i]))
        # Try to lex an identifier or keyword.
        elif text[i].isalpha() or text[i] == "_":
            start = i
            while i < len(text) and (text[i].isalnum() or text[i] == "_"):
                i += 1
            
            token = text[start:i]
            if token in keywords:
                tokens.append(Token(token, token))
            else:
                tokens.append(Token("identifier", token))
        # Try to lex an operator.
        elif text[i] in symbols:
            # 2-character operators.
            if i < len(text) - 1 and text[i:i+2] in operators:
                tokens.append(Token(text[i:i+2], text[i:i+2]))
                i += 2
            # 1-character operators.
            elif text[i] in operators:
                tokens.append(Token(text[i], text[i]))
                i += 1
            # If it's not an operator, it's an invalid token.
            else:
                # TODO: Find the length of the invalid token.
                tokens.append(Token("invalid", text[start:i]))
                break # Break out of the outer while loop.
        # If none of the above succeed, the token is invalid.
        else:
            tokens.append(Token("invalid", text[start:i]))
            break

    # Append a '\n' to the end of the token stream to make parsing slightly easier.
    if not tokens or (not ignoreNewlines and tokens[-1].type != "\n"):
        tokens.append(Token("\n", "\n"))
    return tokens
