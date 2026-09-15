#pragma once
#include "text_buffer.h"

extern TextBuffer *declaration_buffer;//= init_text_buffer();
extern TextBuffer *main_buffer;// = init_text_buffer();

void declar_variable(char* name);
void declar_array(char* name, char* size);
void asign_variable(char* name, char* value );
void asign_variable_array(char* name, char* value ,char* array_index);
