#include <stdio.h>
#include "text_buffer.h"
#include "codegen.h"

void declar_variable(char* name){
  append_text_buffer(declaration_buffer,name);
  append_text_buffer(declaration_buffer,": .word");
  append_text_buffer(declaration_buffer,"\n");
}

void declar_array(char* name, char* size){
  append_text_buffer(declaration_buffer,name);
  append_text_buffer(declaration_buffer,": .word 0:");
  append_text_buffer(declaration_buffer,size);
  append_text_buffer(declaration_buffer,"\n");
}

void asign_variable(char* name, char* value ){
  //move value to register
  append_text_buffer(main_buffer,"li $t1, ");
  append_text_buffer(main_buffer,value);
  append_text_buffer(main_buffer,"\n");

  // move adres to register
  append_text_buffer(main_buffer,"la $t0, ");
  append_text_buffer(main_buffer,name);
  append_text_buffer(main_buffer,"\n");
  //overwrite value in buffer
  append_text_buffer(main_buffer,"sw $t1, 0($t0)\n");
}

void asign_variable_array(char* name, char* value ,char* array_index){
  //move value to register
  append_text_buffer(main_buffer,"li $t1, ");
  append_text_buffer(main_buffer,value);
  append_text_buffer(main_buffer,"\n");

  // move adres to register
  append_text_buffer(main_buffer,"la $t0, ");
  append_text_buffer(main_buffer,name);
  append_text_buffer(main_buffer,"\n");

  // move index to register
  append_text_buffer(main_buffer,"li $t2, ");
  append_text_buffer(main_buffer,array_index);
  append_text_buffer(main_buffer,"\n");

  //make index word size multiplay by 4
  append_text_buffer(main_buffer,"sll $t2, $t2, 2 \n");


  append_text_buffer(main_buffer,"add $t0, $t2 ,$t0\n");


  //overwrite value in buffer
  append_text_buffer(main_buffer,"sw $t1, 0($t0)\n");
}

