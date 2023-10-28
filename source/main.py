from dataclasses import dataclass
from enum import Enum, auto
from typing import Optional


class NodeType(Enum):
    INVALID = auto()
    NUMBER = auto()
    CHARACTER = auto()
    STRING = auto()
    IDENTIFIER = auto()


@dataclass
class Node:
    type: NodeType
    textStart: int
    texdEnd: int
    parent: int
    firstChild: int
    lastChild: int


@dataclass
class ParsingResult:
    errorMessage: Optional[str]
    nodes: Optional[list[Node]]
    topNode: Optional[Node]


def main():
    pass


if __name__ == "__main__":
    main()
