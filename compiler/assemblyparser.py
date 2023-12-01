import lexer
from combinators import *


# Token types
pushInstructions = {
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
}

instructions = {
    "loada8",
    "loada16",
    "loada32",
    "loada64",
    "loadap",

    "loads8",
    "loads16",
    "loads32",
    "loads64",
    "loadsp",

    "storea8",
    "storea16",
    "storea32",
    "storea64",
    "storea",

    "stores8",
    "stores16",
    "stores32",
    "stores64",
    "stores",

    "storep8",
    "storep16",
    "storep32",
    "storep64",
    "storep",

    "copy8",
    "copy16",
    "copy32",
    "copy64",
    "copy",

    "malloc",
    "calloc",
    "realloc",
    "free",

    "noop",
    "halt",
    "call",
    
    "return0",
    "return8",
    "return16",
    "return32",
    "return64",
    "return",

    "jump",
    "jumpt",
    "jumpf",

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

keywords = {
    "u", "u8", "u16", "u32", "u64",
    "i8", "i16", "i32", "i64",
    "uf", "uf32", "uf64",
    "f", "f32", "f64",

    "data",
    "entry",
}.union(pushInstructions, instructions)

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

dataValue = node("dataValue", choice(
    sequence("number", "*", numberSuffix),
    sequence(numberSuffix, zeroOrMore(numberLiteral)),
    "character",
    "string"
))

pushInstruction = choice(*pushInstructions)

instruction = node("instruction",
    choice(
        sequence(pushInstruction, literal),
        *instructions,
    ),
)

statement = choice(
    instruction,
    label
)

dataStatement = choice(
    label,
    oneOrMore(dataValue),
)

entryPoint = node("entrypoint",
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
