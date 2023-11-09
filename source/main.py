from lex import *
from parse import *


if __name__ == "__main__":
    path = "source/example.txt"
    text = readFile(path)
    
    tokens = lex(text)
    print("--- TOKENS ---")
    for i, token in enumerate(tokens):
        print(f"{i:<3} {token}")
    
    tree = parse(tokens)
    print("\n--- SYNTAX TREE ---")
    print(f"Errors: {tree.errors}")
    tree.topNode.prettyPrint()
