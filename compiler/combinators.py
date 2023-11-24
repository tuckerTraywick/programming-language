from dataclasses import dataclass


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
        return (index + 1, tokens[index] if tokens[index].type != "\n" else [], None)
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
        return (index + 1, [], None)

    error = ParsingError(index, "Expected end of statement.")
    while index < len(tokens):
        if tokens[index].type == "\n":
            return (index + 1, error, None)
        index += 1
    return (index, error, error)


# Skips until the given token is encountered.
def recover(*types):
    def parse(tokens, index):
        while index <= len(tokens) and tokens[index].type not in types:
            index += 1
        return (index, None, None)
    return parse


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


# Parses one or more of the given sequence.
def oneOrMore(*parsers):
    return sequence(sequence(*parsers), zeroOrMore(*parsers))


# Parses an expression using the given operator precedences with the given literal parser.
def expression(basicExpression, infix, prefix):
    def parsePrefix(tokens, index):
        operators = []
        # Parse zero or more prefix operators.
        while index < len(tokens) and tokens[index].type in prefix:
            operators.append(tokens[index])
            index += 1

        index, operand, error = basicExpression(tokens, index)
        if error:
            return (index, error, error)
        
        if operators:
            return (index, Node("prefixExpression", operators + [operand]), error)
        return (index, operand, error)
    
    def parseInfix(tokens, index, precedence, lhs):
        children = [lhs]
        while index < len(tokens):
            operator = tokens[index]
            if operator.type not in infix:
                break
                
            index += 1
            if infix[operator.type] > precedence:
                index, rhs, error = parsePrefix(tokens, index)
                if error:
                    return (index, error, error)
                
                index, result, error = parseInfix(tokens, index, infix[operator.type] + 1, rhs)
                if error:
                    return (index, error, error)
                children += [operator, result]
            elif infix[operator.type] < precedence:
                index -= 1
                break
            else:
                index, rhs, error = parsePrefix(tokens, index)
                if error:
                    return (index, error, error)
                children += [operator, rhs]
        
        if len(children) > 1:
            return (index, Node("infixExpression", children), None)
        return (index, children[0], None)

    def parse(tokens, index):
        index, lhs, error = parsePrefix(tokens, index)
        if error:
            return (index, error, error)
        return parseInfix(tokens, index, 0, lhs)
    return parse
