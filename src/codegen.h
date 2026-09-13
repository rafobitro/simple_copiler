#pragma once
#include "text_buffer.h"

extern TextBuffer *declaration_buffer;//= init_text_buffer();
extern TextBuffer *main_buffer;// = init_text_buffer();

void declare_variable_type(char* word);
void declar_variable(char* name);
void declar_array(char* name, char* size);
void asign_variable(char* name, char* value );
