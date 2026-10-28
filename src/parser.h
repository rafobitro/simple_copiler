#pragma once
#include <stdbool.h>
#include "lexer.h"


bool type_chack(char* type, TokenType t_type);
bool is_variable_type_exsist(char* word);
bool is_variable_declared(char* word);
void parser_error(char*message, Token token);
int parse_declaration(int *i, char** scopes);
int parse_asigner(int *i, char** scopes);
int parse_statements(int *i, bool function, char** scopes);
int parse_fundef(int *i, char** scopes);
int parse_funcall(int *i, char** scopes);
int parse_branch(int *i,bool is_loop, char** scopes);
int parser(void);
void parser_free(void);
