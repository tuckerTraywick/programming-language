#include "parser.h"
#include "lexer.h"

const char *const node_type_names[NODE_TYPE_COUNT];

const char *const parsing_error_messages[PARSING_ERROR_TYPE_COUNT];

bool parse(char *text, struct token *tokens, struct node **nodes, struct parsing_error **errors);

