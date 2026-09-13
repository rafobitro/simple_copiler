#include <stdio.h>
#include <string.h>
#include "text_buffer.h"
#include "codegen.h"


//most likly dont need anymore becouse desison is every variable type will be register size 32 bit meaning word
void declare_variable_type(char* word){
  if(strcmp(word,"$number")==0 )
     append_text_buffer(declaration_buffer,".asciiz");
  else if(strcmp(word,"$string")==0)
     append_text_buffer(declaration_buffer,".asciiz");
}

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


