from lex import *
from parse import *


if __name__ == "__main__":
    text = readFile("source/example.txt")
    
    tokens = lex(text)
    print("--- TOKENS ---")
    for i, token in enumerate(tokens):
        print(f"{i:<3} {token}")

    tree, error = parse(tokens)
    print("\n--- SYNTAX TREE ---")
    if error:
        print(f"Error: {error.message}")
    if tree:
        tree.prettyPrint()