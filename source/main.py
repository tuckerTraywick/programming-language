from lex import *
# from combinators import *


if __name__ == "__main__":
    path = "source/example.txt"
    text = readFile(path)
    
    tokens = lex(text)
    print("--- TOKENS ---")
    for i, token in enumerate(tokens):
        print(f"{i:<3} {token}")

    # tree = parse(tokens)
    # if tree.errors:
    #     print("\n--- PARSING ERRORS ---")
    #     for error in tree.errors:
    #         print(error)        
    # print("\n--- SYNTAX TREE ---")
    # tree.topNode.prettyPrint()
