#pragma once
#include "text_buffer.h"
#include "lexer.h"


extern TextBuffer *declaration_buffer;//= init_text_buffer();
extern TextBuffer *main_buffer;// = init_text_buffer();

void declar_variable(char* name);
void declar_array(char* name, char* size);
void asign_variable(char* name, char* value );
void asign_variable_array(char* name, char* value ,char* array_index);
void codegen_function_start(char* name);
void codegen_function_end(char* name);
void codegen_function_param_store(char* name, int index);
void codegen_load_arg(char* value, TokenType type, int index);
void codegen_load_arg_address(char* name, int index);
void codegen_function_call(char* name);
