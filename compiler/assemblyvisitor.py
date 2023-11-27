from assemblyparser import *


def assemble(tree, file):
    if tree.children[0].type == "dataSection":
        
