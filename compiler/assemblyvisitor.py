from assemblyparser import *


def assemble(tree):
    addressSize = 8
    opcodes = {
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

    code = []
    labels = {}
    entryPoint = None
    programAddress = 0
    
    if tree.children[0].type == "dataSection":
        pass
    
    for child in tree.children:
        if child.type == "entryPoint":
            entryPoint = programAddress
        elif child.type == "label":
            labelName = child.children[0].text
            labels[labelName] = programAddress
        elif child.type == "instruction":
            instructionName = child.children[0]
            code.append(opcodes[instructionName])
            if instructionName in instructionsWithOneArgument:
                pass
    return intToBytes(entryPoint) + code


def intToBytes(number):
    pass
