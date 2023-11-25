import lexer
from combinators import *


# Token types
keywords = {
    "u", "u8", "u16", "u32", "u64",
    "i8", "i16", "i32", "i64",
    "uf", "uf32", "uf64",
    "f", "f32", "f64",
    "data",
    "entry",
    "pushu8",
    "pushu16",
    "pushu32",
    "pushu64",
    "pushi8",
    "pushi16",
    "pushi32",
    "pushi64",
    "pushuf32",
    "pushuf64",
    "pushf32",
    "pushf64",
    "load8",
    "load16",
    "load32",
    "load64",
    "loada",
    "loads",
    "load",
    "jump",
    "jumpt",
    "jumpf",
    "call",
    "return",
    "halt",
    "print8",
    "pos",
    "neg",
    "add",
    "sub",
    "mul",
    "div",
    "mod",
    "lshift",
    "rshift",
    "not",
    "and",
    "or",
    "xor",
    "bnot",
    "band",
    "bor",
    "bxor",
    "eq",
    "ne",
    "gt",
    "gte",
    "lt",
    "lte",
}

operators = {
    ":",
    ".",
    "+",
    "-",
    "*",
}

lineComment = "#"


# Grammar
numberSuffix = choice(
    "u", "u8", "u16", "u32", "u64",
    "i8", "i16", "i32", "i64",
    "uf", "uf32", "uf64",
    "f", "f32", "f64",
)

numberLiteral = node("numberLiteral",
    maybe(choice("+", "-")), "number", maybe(".", "number"), maybe(numberSuffix)
)

literal = choice(
    numberLiteral,
    "string",
    "character",
    "identifier",
)

label = node("label",
    "identifier", ":"
)

opcodeWithOneArgument = choice(
    "pushu8",
    "pushu16",
    "pushu32",
    "pushu64",
    "pushi8",
    "pushi16",
    "pushi32",
    "pushi64",
    "pushuf32",
    "pushuf64",
    "pushf32",
    "pushf64",
    "load8",
    "load16",
    "load32",
    "load64",
    "loada",
    "loads",
    "load",
    "jump",
    "jumpt",
    "jumpf",
    "call",
    "return",
)

instruction = node("instruction",
    choice(
        sequence(opcodeWithOneArgument, maybe(literal)),
        sequence("push", oneOrMore(literal)),
        "halt",
        "print8",
        "pos",
        "neg",
        "add",
        "sub",
        "mul",
        "div",
        "mod",
        "lshift",
        "rshift",
        "not",
        "and",
        "or",
        "xor",
        "bnot",
        "band",
        "bor",
        "bxor",
        "eq",
        "ne",
        "gt",
        "gte",
        "lt",
        "lte",
    ),
)

statement = choice(
    instruction,
    label
)

dataValue = node("dataValue", choice(
    sequence("number", "*", numberSuffix),
    sequence(numberSuffix, zeroOrMore(numberLiteral)),
    "character",
    "string"
))

dataStatement = choice(
    label,
    oneOrMore(dataValue),
)

entryPoint = node("entryPoint",
    ".", "entry", ":"
)

dataSection = node("dataSection",
    ".", "data", ":",
    zeroOrMore(dataStatement)
)

program = node("program",
    maybe(dataSection),
    entryPoint,
    zeroOrMore(statement)
)


# Lexes the given text into tokens.
def lex(text):
    return lexer.lex(text, keywords, operators, lineComment, True)


# Parses the given tokens into a syntax tree.
def parse(tokens):
    return program(tokens, 0)[1:3]
