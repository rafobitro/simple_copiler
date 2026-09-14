#pragma once
#include <stdbool.h>
#include "lexer.h"


bool type_chack(char* type, TokenType t_type);
bool is_variable_type_exsist(char* word);
int parse_declaration(int *i);
void parser_error(char* message, Token token); 
int parser(void);
