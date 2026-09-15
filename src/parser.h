#pragma once
#include <stdbool.h>
#include "lexer.h"


bool type_chack(char* type, TokenType t_type);
bool is_variable_type_exsist(char* word);
bool is_variable_declared(char* word);
void parser_error(char*message, Token token); 
int parse_declaration(int *i);
int parse_asigner(int *i);
int parser(void);
