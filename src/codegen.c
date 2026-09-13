#include <stdio.h>
#include "text_buffer.h"
#include "codegen.h"

void declar_variable(char* name){
  append_text_buffer(declaration_buffer,name);
  append_text_buffer(declaration_buffer,": : .word");
  append_text_buffer(declaration_buffer,"\n");
}

void declar_array(char* name, char* size){
  append_text_buffer(declaration_buffer,name);
  append_text_buffer(declaration_buffer,": .word ");
  append_text_buffer(declaration_buffer,size);
  append_text_buffer(declaration_buffer,"\n");
}

void asign_variable(char* name, char* value ){
  //move value to register
  append_text_buffer(main_buffer,"la $t1, ");
  append_text_buffer(main_buffer,value);
  append_text_buffer(main_buffer,"\n");

  // move adres to register
  append_text_buffer(main_buffer,"la $t0, ");
  append_text_buffer(main_buffer,name);
  append_text_buffer(main_buffer,"\n");
  //overwrite value in buffer
  append_text_buffer(main_buffer,"sw $t1, 0($t0)\n");
}


