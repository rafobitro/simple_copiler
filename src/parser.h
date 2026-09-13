#pragma once
#include <stdbool.h>
#include "lexer.h"


bool type_chack(char* type, TokenType t_type);
bool is_variable_type_exsist(char* word);
int parser(void);
