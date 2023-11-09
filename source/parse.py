from typing import Optional
from dataclasses import dataclass
from enum import Enum, auto
from lex import *


# Represents the type of a node.
class NodeType(Enum):
    PROGRAM = auto()
    PACKAGE_STATEMENT = auto()
    IMPORT_STATEMENT = auto()
    PACKAGE_NAME = auto()
    VARIABLE_DEFINITION = auto()


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
        return f"token {self.tokenIndex}: {self.message}"


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
        self.saves: list

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

    # Accepts a token of any of the given types then appends it to the bottom node if it is present,
    # but does nothing if it isn't.
    def accept(self, *types: TokenType):
        if self.keepParsing and self.currentToken and self.currentToken.type in types:
            self.bottomNode.children.append(self.currentToken)
            self.getNextToken()

    # Expects a token of any of the given types then appends it to the bottom node if it is present,
    # and adds the given error to the parse tree if it isn't.
    def expectError(self, error: str, *types: TokenType):
        if self.keepParsing:
            if self.currentToken and self.currentToken.type in types:
                self.bottomNode.children.append(self.currentToken)
                self.getNextToken()
            else:
                if self.currentToken:
                    error = error.replace("$", self.currentToken.type.name)
                errorLeaf = ParsingError(error, self.tokenIndex)
                self.bottomNode.children.append(errorLeaf)
                self.tree.errors.append(errorLeaf)
                self.keepParsing = False
    
    # Expects a token of any of the given types then appends it to the bottom node if it is present,
    # and adds a generic error to the parse tree if it isn't.
    def expect(self, *types: TokenType):
        typeNames = ", ".join(type.name for type in types[:-1]) + (" or " if len(types) > 1 else "") + types[-1].name
        error = f"Expected {typeNames}, but got {self.currentToken.type.name if self.currentToken else 'nothing'}."
        self.expectError(error, *types)

    # Accepts the given parser and backtracks if it fails. Returns true if it succeeds.
    def maybe(self, f) -> bool:
        if self.keepParsing:
            # Save the current state.
            previousTokenIndex = self.tokenIndex
            previousChildrenLength = len(self.bottomNode.children)
            previousErrorsLength = len(self.tree.errors)
            previousBottomNode = self.bottomNode
            f()
            if not self.keepParsing:
                # Backtrack to before `f()` was called.
                self.tokenIndex = previousTokenIndex
                self.tree.errors = self.tree.errors[:previousErrorsLength]
                self.bottomNode = previousBottomNode
                self.bottomNode.children = self.bottomNode.children[:previousChildrenLength]
                self.keepParsing = True
                return False
            return True
        return False

    # Accepts zero or more of the given parser and backtracks if the parser fails.
    def zeroOrMore(self, f):
        while self.keepParsing:
            if not self.maybe(f):
                return

    # Accepts one or more of the given parser and adds an error to the parse tree if there is not at
    # least one occurrence.
    def oneOrMore(self, f):
        f()
        self.zeroOrMore(f)

    # Accepts any of the given parsers. If none of the parsers succeed, the error of the last parser
    # is the one that gets added to the parse tree.
    def any(self, *fs):
        if self.keepParsing:
            for f in fs[:-1]:
                if self.maybe(f):
                    return
            fs[-1]()

    # Saves the state of the parser in case it needs to recover from an error.
    def save(self):
        self.saves.append(self.keepParsing)

    # Recovers to a previous save if an error was encountered.
    def recover(self):
        doRecover = self.saves.pop()
        if doRecover and not self.keepParsing:
            self.keepParsing = True

    # Recovers to the previous save and consumes tokens until a `;` is found if an error was
    # encountered.
    def recoverUntilLineEnd(self, keepLineEnd=False):
        doRecover = self.saves.pop()
        if doRecover and not self.keepParsing:
            self.keepParsing = True
            while self.currentToken and self.currentToken.type != TokenType.SEMICOLON:
                self.getNextToken()

            if keepLineEnd and self.currentToken and self.currentToken.type == TokenType.SEMICOLON:
                self.bottomNode.children.append(self.currentToken)
                self.getNextToken()

    # Parses the given tokens.
    def parse(self, tokens: list[Token]) -> Tree:
        self.tokens = tokens
        self.tokenIndex = 0
        self.tree = Tree(Node(NodeType.PROGRAM, None, []), [])
        self.bottomNode = self.tree.topNode
        self.keepParsing = True
        self.saves = []
        self.parseProgram()
        return self.tree

    # Parses a program. Helper for `self.parse()`.
    def parseProgram(self):
        # for i in range(4):
        #     self.parsePackageStatement()
        self.maybe(self.parsePackageStatement)
        self.zeroOrMore(self.parseStatement)

    # Parses a statement. Helper for `self.parse()`.
    def parseStatement(self):
        self.any(
            self.parseImportStatement,
            self.parseVariableDefinition,
        )

    # Parses a package statement. Helper for `self.parse()`
    def parsePackageStatement(self):
        self.beginNode(NodeType.PACKAGE_STATEMENT)
        self.accept(TokenType.PUB)
        self.expect(TokenType.PACKAGE)
        self.save()
        self.parsePackageName()
        self.recoverUntilLineEnd()
        self.parseLineEnd()
        self.endNode()

    # Parses an import statement. Helper for `self.parse()`
    def parseImportStatement(self):
        self.beginNode(NodeType.IMPORT_STATEMENT)
        self.expect(TokenType.IMPORT)
        self.save()
        self.parsePackageName()
        self.zeroOrMore(self.parseImportStatementName)
        self.accept(TokenType.COMMA)
        self.recoverUntilLineEnd()
        self.parseLineEnd()
        self.endNode()
    
    # Parses a package name in an import statement. Helper for `self.parseImportStatement()`.
    def parseImportStatementName(self):
        self.expect(TokenType.COMMA)
        self.parsePackageName()

    # Parses a variable declaration / definition. Helper for `self.parse()`.
    def parseVariableDefinition(self):
        self.beginNode(NodeType.VARIABLE_DEFINITION)
        self.accept(TokenType.PUB)
        self.expect(TokenType.VAR)
        self.save()
        self.expectError("Expected an identifier.", TokenType.IDENTIFIER)
        self.accept(TokenType.IDENTIFIER)
        self.recoverUntilLineEnd()
        self.parseLineEnd()

    # Parses a package name. Helper for `self.parse()`.
    def parsePackageName(self):
        self.beginNode(NodeType.PACKAGE_NAME)
        self.expectError("Expected a package name.", TokenType.IDENTIFIER)
        self.zeroOrMore(self.parsePackageNameEnd)
        self.endNode()

    # Parses the end of a package name. Helper for `self.parsePackageName()`.
    def parsePackageNameEnd(self):
        self.expect(TokenType.DOT)
        self.expectError("Invalid package name (expected identifier or `*`).", TokenType.IDENTIFIER, TokenType.TIMES)

    # Parses a `;` and maybe recovers from extra tokens before the `;`. Helper for `self.parse()`.
    def parseLineEnd(self):
        self.save()
        self.expectError("Expected a semicolon.", TokenType.SEMICOLON)
        self.recoverUntilLineEnd(True)


# Parses a list of tokens into a syntax tree.
def parse(tokens: list[Token]) -> Node:
    return Parser().parse(tokens)
