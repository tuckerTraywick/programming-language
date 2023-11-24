import lexer
from combinators import *


# Token types
keywords = {
    "u", "u8", "u16", "u32", "u64",
    "i8", "i16", "i32", "i64",
    "uf", "uf32", "uf64",
    "f", "f32", "f64",
    "halt",
    "pushu8",
    "push",
    "print8",
}

operators = {
    ":",
    ".",
    "+",
    "-",
}

lineComment = "#"


# Errors
missingColon = error("Expected a colon.")


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
)

label = node("label",
    maybe("."), "identifier", choice(":", missingColon), lineEnd
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
        "print8,"
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
    lineEnd
)

programStatement = choice(
    instruction,
    label
)

program = node("program",
    zeroOrMore(programStatement)
)


# Lexes the given text into tokens.
def lex(text):
    return lexer.lex(text, keywords, operators, lineComment)


# Parses the given tokens into a syntax tree.
def parse(tokens):
    return program(tokens, 0)[1:3]
