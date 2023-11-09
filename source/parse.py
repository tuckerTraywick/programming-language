from typing import Optional
from dataclasses import dataclass
from enum import Enum, auto
from lex import *


# Represents the type of a node.
class NodeType(Enum):
    PROGRAM = auto()
    PACKAGE_STATEMENT = auto()
    IMPORT_STATEMENT = auto()


# Represents a node in a parse tree. Can have zero or more children.
@dataclass
class Node:
    type: NodeType # The type of the node.
    parent: Optional[type["Node"]] # The parent of the node.
    children: list[type["Leaf"]] # The node's children. Can be empty.

    def prettyPrint(self, indentation: int=0) -> str:
        tab = "|  "
        print(tab*indentation + self.type.name)
        indentation += 1
        for child in self.children:
            if isinstance(child, Node):
                child.prettyPrint(indentation)
            else:
                print(tab*indentation + str(child))


# Represents an error encountered during parsing. Gets added to the parse tree.
@dataclass
class ParsingError:
    message: str
    tokenIndex: int

    def __str__(self) -> str:
        return f"{self.tokenIndex}: {self.message}"


# Represents the child of a `Node`.
Leaf = Token | Node | ParsingError


# The result of parsing a list of tokens. Represents the abstract syntax tree of a program.
@dataclass
class Tree:
    topNode: Node
    errors: list[ParsingError]


# Holds state for the parser. Helper for `parse()`.
class Parser:
    def __init__(self):
        self.tokens: list[Token]
        self.tokenIndex: int
        self.tree: Tree
        self.bottomNode: Node
        self.keepParsing: bool

    # The current token being parsed.
    @property
    def currentToken(self) -> Optional[Token]:
        if self.tokenIndex < len(self.tokens):
            return self.tokens[self.tokenIndex]
        return None
    
    # Advances to the next token.
    def getNextToken(self):
        assert self.tokenIndex < len(self.tokens), "Must have tokens left to call `getNextToken().`"
        self.tokenIndex += 1

    # Starts a new node in the parse tree and moves the bottom node down one level.
    def beginNode(self, type: NodeType):
        if self.keepParsing:
            newNode = Node(type, self.bottomNode, [])
            self.bottomNode.children.append(newNode)
            self.bottomNode = newNode

    # Ends a node in the parse tree and move the bottom node up one level.
    def endNode(self):
        if self.keepParsing and self.bottomNode is not self.tree.topNode:
            self.bottomNode = self.bottomNode.parent

    # Accepts a token of any of the given types then appends it to the bottom node if it is present, but does nothing if it isn't.
    def accept(self, *types: TokenType):
        if self.keepParsing and self.currentToken and self.currentToken.type in types:
            self.bottomNode.children.append(self.currentToken)
            self.getNextToken()

    # Expects a token of any of the given types then appends it to the bottom node if it is present, and adds
    # the given error to the parse tree if it isn't.
    def expectError(self, error: str, *types: TokenType):
        if self.keepParsing:
            if self.currentToken and self.currentToken.type in types:
                self.bottomNode.children.append(self.currentToken)
                self.getNextToken()
            else:
                errorLeaf = ParsingError(error, self.tokenIndex)
                self.bottomNode.children.append(errorLeaf)
                self.tree.errors.append(errorLeaf)
                self.keepParsing = False
    
    # Expects a token of any of the given types then appends it to the bottom node if it is present, and adds
    # a generic error to the parse tree if it isn't.
    def expect(self, *types: TokenType):
        typeNames = ", ".join(type.name for type in types[:-1]) + (" or " if len(types) > 1 else "") + types[-1].name
        error = f"Expected {typeNames}, but got {self.currentToken.type.name if self.currentToken else 'nothing'}."
        self.expectError(error, *types)

    # Accepts the given parser and backtracks if it fails. Returns true if it succeeds.
    def maybe(self, f) -> bool:
        if self.keepParsing:
            # Save the current state.
            previousTokenIndex = self.tokenIndex
            previousBottomNode = self.bottomNode
            previousChildrenLength = len(self.bottomNode.children)
            previousErrorsLength = len(self.tree.errors)
            f()
            if not self.keepParsing:
                # Backtrack to before `f()` was called.
                self.tokenIndex = previousTokenIndex
                self.bottomNode = previousBottomNode
                self.bottomNode.children = self.bottomNode.children[:previousChildrenLength]
                self.tree.errors = self.tree.errors[:previousErrorsLength]
                self.keepParsing = True
                return False
            return True
        return False

    # Accepts zero or more of the given parser and backtracks if the parser fails.
    def zeroOrMore(self, f):
        while self.keepParsing:
            if not self.maybe(f):
                return

    # Accepts one or more of the given parser and adds an error to the parse tree if there is not at least one occurrence.
    def oneOrMore(self, f):
        f()
        self.zeroOrMore(f)

    # Accepts any of the given parsers. If none of the parsers succeed, the error of the last parser is the one that gets
    # added to the parse tree.
    def any(self, *fs):
        if self.keepParsing:
            for f in fs[:-1]:
                if self.maybe(f):
                    return
            fs[-1]()

    # Parses the given tokens.
    def parse(self, tokens: list[Token]) -> Tree:
        self.tokens = tokens
        self.tokenIndex = 0
        self.tree = Tree(Node(NodeType.PROGRAM, None, []), [])
        self.bottomNode = self.tree.topNode
        self.keepParsing = True
        self.parseProgram()
        return self.tree

    # Parses a program. Helper for `self.parse()`.
    def parseProgram(self):
        self.zeroOrMore(self.parseStatement)

    # Parses a statement. Helper for `self.parse()`.
    def parseStatement(self):
        self.any(
            self.parsePackageStatement,
            self.parseImportStatement,
        )

    # Parses a package statement. Helper for `self.parse()`
    def parsePackageStatement(self):
        self.beginNode(NodeType.PACKAGE_STATEMENT)
        self.accept(TokenType.PUB)
        self.expect(TokenType.PACKAGE)
        self.expectError("Expected a package name.", TokenType.IDENTIFIER)
        self.endNode()

    # Parses an import statement. Helper for `self.parse()`
    def parseImportStatement(self):
        self.beginNode(NodeType.IMPORT_STATEMENT)
        self.expect(TokenType.IMPORT)
        self.expectError("Expected a name.", TokenType.IDENTIFIER)
        self.endNode()


# Parses a list of tokens into a syntax tree.
def parse(tokens: list[Token]) -> Node:
    return Parser().parse(tokens)


# # Represents the state of the parser. Helper for `parse()`.
# class Parser:
#     def __init__(self):
#         self.tokens: list[Token]
#         self.i: int
#         self.tree: Optional[Node]
#         self.innermost: Optional[Node]
#         self.errorMessage: Optional[str]

#     # Accepts the given token if it is present, but doesn't throw an error if it's not.
#     def accept(self, type: TokenType=None) -> bool:
#         assert self.tokens, "Must have tokens to accept a token."
#         assert self.innermost is not None, "Must have a node to append a child."
#         if self.errorMessage is None and self.i < len(self.tokens) and self.tokens[self.i].type == type:
#             self.innermost.children.append(self.tokens[self.i])
#             self.i += 1
#             return True
#         return False

#     # Expects the given token and adds an error to the parse tree if not.
#     def expect(self, type: TokenType=None, errorMessage: str=None) -> bool:
#         assert self.tokens, "Must have tokens to expect a token."
#         assert self.innermost is not None, "Must have a node to append a child."
#         if self.errorMessage is not None:
#             return False
        
#         if self.i >= len(self.tokens):
#             self.errorMessage = (f"{self.i}: " + errorMessage) or f"Expected {type.name} at {self.i} but ran out of tokens."
#             return False
        
#         if self.tokens[self.i].type != type:
#             self.errorMessage = (f"{self.i}: " + errorMessage) or f"Expected {type.name} at {self.i} but got {self.tokens[self.i].type.name}."
#             return False

#         self.innermost.children.append(self.tokens[self.i])
#         self.i += 1
#         return True

#     # Starts a new node to the parse tree with the given type. Adds it as a child to the innermost node.
#     def startNode(self, type: NodeType):
#         if self.errorMessage is not None:
#             return
        
#         if self.tree is None:
#             self.tree = Node(type, None, [])
#             self.innermost = self.tree
#         else:
#             newInnermost = Node(type, self.innermost, [])
#             self.innermost.children.append(newInnermost)
#             self.innermost = newInnermost

#     # Ends the current node and steps up one level in the parse tree.
#     def endNode(self):
#         assert self.tree is not None, "Must have a parse tree to end a node."
#         # Only end the node if it's not the topmost one.
#         if self.innermost is not self.tree:
#             self.innermost = self.innermost.parent

#     # Runs the given parser and resets if it fails.
#     def maybe(self, parser):
#         if self.errorMessage is not None:
#             return
#         previousI = self.i
#         previousInnermost = self.innermost
#         parser()
#         if self.errorMessage:
#             self.i = previousI
#             self.innermost = previousInnermost
#             self.errorMessage = None
    
#     # Accepts zero or more of the given parser and resets it when it fails.
#     def zeroOrMore(self, parser):
#         while self.errorMessage is None:
#             previousI = self.i
#             previousInnermost = self.innermost
#             parser()
#             if self.errorMessage is not None:
#                 self.i = previousI
#                 self.innermost = previousInnermost
#                 self.errorMessage = None
#                 break

#     # Parses the given tokens.
#     def parse(self, tokens: list[Token]) -> Node:
#         assert all(token.type != TokenType.INVALID for token in tokens), "Cannot parse invalid tokens yet."
#         self.tokens = tokens
#         self.i = 0
#         self.tree = None
#         self.innermost = None
#         self.errorMessage = None
#         self.program()
#         return self.tree
    
#     # Parses a program. Helper for `self.parse()`.
#     def program(self):
#         self.startNode(NodeType.PROGRAM)
#         self.packageStatement()
#         self.endNode()

#     # Parses a `package` statement. Helper for `self.parse()`
#     def packageStatement(self):
#         self.startNode(NodeType.PACKAGE_STATEMENT)
#         self.accept(TokenType.PUB)
#         self.expect(TokenType.PACKAGE, "Expected a declaration.")
#         self.expect(TokenType.IDENTIFIER, "Expected a package name.")
#         self.endNode()
