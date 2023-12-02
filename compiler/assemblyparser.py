import lexer
from combinators import *


# Token types
pushInstructions = {
    "pushu8": 1,
    "pushu16": 2,
    "pushu32": 3,
    "pushu64": 4,
    "pushi8": 5,
    "pushi16": 6,
    "pushi32": 7,
    "pushi64": 8,
    "pushuf32": 9,
    "pushuf64": 10,
    "pushf32": 11,
    "pushf64": 12,
}

instructions = {
    "pop8": 13,
    "pop16": 14,
    "pop32": 15,
    "pop64": 16,
    "pop": 17,

    "loada8": 18,
    "loada16": 19,
    "loada32": 20,
    "loada64": 21,
    "loadap": 22,

    "loads8": 23,
    "loads16": 24,
    "loads32": 25,
    "loads64": 26,
    "loadsp": 27,

    "storea8": 28,
    "storea16": 29,
    "storea32": 30,
    "storea64": 34,
    "storea": 35,

    "stores8": 36,
    "stores16": 37,
    "stores32": 38,
    "stores64": 39,
    "stores": 34,

    "storep8": 35,
    "storep16": 36,
    "storep32": 37,
    "storep64": 38,
    "storep": 39,

    "copy8": 40,
    "copy16": 41,
    "copy32": 42,
    "copy64": 43,
    "copy": 44,

    "malloc": 45,
    "calloc": 46,
    "realloc": 47,
    "free": 48,

    "noop": 0,
    "halt": 49,
    "call": 50,
    
    "return0": 51,
    "return8": 52,
    "return16": 53,
    "return32": 54,
    "return64": 55,
    "return": 56,

    "jump": 57,
    "jumpt": 58,
    "jumpf": 59,

    "print8": 60,

    "posu8": 61,
    "posu16": 62,
    "posu32": 63,
    "posu64": 64,
    "posi8": 65,
    "posi16": 66,
    "posi32": 67,
    "posi64": 68,
    "posuf32": 69,
    "posuf64": 70,
    "posf32": 71,
    "posf64": 72,
    
    "negu8": 73,
    "negu16": 74,
    "negu32": 75,
    "negu64": 76,
    "negi8": 77,
    "negi16": 78,
    "negi32": 79,
    "negi64": 80,
    "neguf32": 81,
    "neguf64": 82,
    "negf32": 83,
    "negf64": 84,

    "addu8": 85,
    "addu16": 86,
    "addu32": 87,
    "addu64": 88,
    "addi8": 89,
    "addi16": 90,
    "addi32": 91,
    "addi64": 92,
    "adduf32": 93,
    "adduf64": 94,
    "addf32": 95,
    "addf64": 96,
    
    "subu8": 97,
    "subu16": 98,
    "subu32": 99,
    "subu64": 100,
    "subi8": 101,
    "subi16": 102,
    "subi32": 103,
    "subi64": 104,
    "subuf32": 105,
    "subuf64": 106,
    "subf32": 107,
    "subf64": 108,
    
    "mulu8": 109,
    "mulu16": 110,
    "mulu32": 111,
    "mulu64": 112,
    "muli8": 113,
    "muli16": 114,
    "muli32": 115,
    "muli64": 116,
    "muluf32": 117,
    "muluf64": 118,
    "mulf32": 119,
    "mulf64": 120,
    
    "divu8": 121,
    "divu16": 122,
    "divu32": 123,
    "divu64": 124,
    "divi8": 125,
    "divi16": 126,
    "divi32": 127,
    "divi64": 128,
    "divuf32": 129,
    "divuf64": 130,
    "divf32": 131,
    "divf64": 132,
    
    "modu8": 133,
    "modu16": 134,
    "modu32": 135,
    "modu64": 136,
    "modi8": 137,
    "modi16": 138,
    "modi32": 139,
    "modi64": 140,
    "moduf32": 141,
    "moduf64": 142,
    "modf32": 143,
    "modf64": 144,

    "lshift8": 157,
    "lshift16": 158,
    "lshift32": 159,
    "lshift64": 160,

    "rshift8": 161,
    "rshift16": 162,
    "rshift32": 163,
    "rshift64": 164,

    "not8": 165,
    "not16": 166,
    "not32": 167,
    "not64": 168,

    "and8": 169,
    "and16": 170,
    "and32": 171,
    "and64": 172,

    "or8": 173,
    "or16": 174,
    "or32": 175,
    "or64": 176,

    "xor8": 177,
    "xor16": 178,
    "xor32": 179,
    "xor64": 180,

    "bnot8": 181,
    "bnot16": 182,
    "bnot32": 183,
    "bnot64": 184,

    "band8": 185,
    "band16": 186,
    "band32": 187,
    "band64": 188,

    "bor8": 189,
    "bor16": 190,
    "bor32": 191,
    "bor64": 192,

    "bxor8": 193,
    "bxor16": 194,
    "bxor32": 195,
    "bxor64": 196,

    "eq8": 197,
    "eq16": 198,
    "eq32": 199,
    "eq64": 200,

    "ne8": 201,
    "ne16": 202,
    "ne32": 203,
    "ne64": 204,

    "gt8": 205,
    "gt16": 206,
    "gt32": 207,
    "gt64": 208,

    "ge8": 209,
    "ge16": 210,
    "ge32": 211,
    "ge64": 212,

    "lt8": 213,
    "lt16": 214,
    "lt32": 215,
    "lt64": 216,

    "le8": 217,
    "le16": 218,
    "le32": 219,
    "le64": 220,
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
