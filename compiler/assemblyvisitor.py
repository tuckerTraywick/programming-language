from assemblyparser import *


def assemble(tree):
    addressSize = 8
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
