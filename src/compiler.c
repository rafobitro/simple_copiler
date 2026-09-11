#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "text_buffer.h"
#include "lexer.h"



int file_to_buffer(char* filename);
TextBuffer *declaration_buffer;//= init_text_buffer();
TextBuffer *main_buffer;// = init_text_buffer();

int parser();


int main(int args,char *argv[]){

  if(args<2){
    printf("no sourcefile");
    return 1;
  }

  char *src_file = argv[1];
  //char output_name;
  int dot_index=-1;

  for(int i=0;src_file[i] != '\0' ;i++){
    if(src_file[i] == '.')
      dot_index=i;
  }

  if(dot_index==-1 || 
    src_file[dot_index+1]!='y' ||
    src_file[dot_index+2]!='u' ||
    src_file[dot_index+3]!='\0' ){
    printf("invalid exstension use .yu file extension ");
    return 1;
  }
  
  char input_file[dot_index+3];
  for(int i=0;i<=dot_index ;i++){
    input_file[i]=src_file[i];
  }
  input_file[dot_index+1]='s';
  input_file[dot_index+2]='\0';
  
  declaration_buffer=init_text_buffer();
  append_text_buffer(declaration_buffer,".data\n");
   
  main_buffer=init_text_buffer();
  append_text_buffer(main_buffer,".text\n");
  append_text_buffer(main_buffer,".globl main\n");
  append_text_buffer(main_buffer,"main:\n");
  append_text_buffer(main_buffer,"li $v0, 10\n");
  append_text_buffer(main_buffer,"syscall\n");
  
  if(file_to_buffer(src_file))return 1;
  if(lexer())return 1;
  if(parser());//return 1;
  
  FILE  *outputptr;
  outputptr = fopen(input_file,"w");
  text_buffer_to_file(declaration_buffer,outputptr);
  text_buffer_to_file(main_buffer,outputptr);
  fclose(outputptr);
  
  lexer_debug();
    for(int i=0;i<lexed_count;i++)
    free(lexed_buffer[i].word);
    free(source_buffer);
    free(lexed_buffer);
    free_text_buffer(declaration_buffer);
    free_text_buffer(main_buffer);
  return 0;
}

int file_to_buffer(char* filename){
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    printf("faild to open source file");
    return 1;
  }
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);
  source_buffer=malloc(file_size+1);
  fread(source_buffer,1,file_size,file);
  source_buffer[file_size] = '\0';
  //source_buffer[file_size+1] = '\0';
  fclose(file);
  
  return 0;
}



bool is_variable_type_exsist(char* word){
    if(strcmp(word,"$number") ==0)
      return true;
    if(strcmp(word,"$string") ==0)
      return true;
    else{
      printf("ther is not such variabkle type \n");
      return false;
    }
}
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

bool type_chack(char* type, TokenType t_type){
  if(t_type == NUMBER && strcmp(type,"$number")==0)
    return true;
  if(t_type == STRING && strcmp(type,"$string")==0)
    return true;
  return false;
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

int parser(){
  int i = 0;
  int current_line;
  while(i<lexed_count){
    current_line=lexed_buffer[i].line;
    if(lexed_buffer[i].type==VARIABLE_TYPE && is_variable_type_exsist(lexed_buffer[i].word)){
      char* type=lexed_buffer[i].word;
      i++;
      if(i>=lexed_count){
        printf("forgot to write variable name \n could not parse");
        return 1;
      }
      if(lexed_buffer[i].type!=VARIABLE ){
         printf("none valid structure afther variable type you sould write variable name \n could not parse");
        return 1;
      }
      char* name=lexed_buffer[i].word;
      i++;
      if(i>=lexed_count || lexed_buffer[i].type != ASIGNER && lexed_buffer[i].type != ARRAY_NUMBER){
        declar_variable( name);
      }
      else if( lexed_buffer[i].type == ASIGNER ){
        declar_variable( name);
        i++;
        if(i>=lexed_count){
          printf("forgot to asign value \n could not parse");
          return 1;
        }
        if(type_chack(type,lexed_buffer[i].type)){
          asign_variable(name,lexed_buffer[i].word);
          i++;
        }
        else{
          printf("you should asign  ");
          printf("%s",type);
          printf("to ");
          printf("%s",name);
          printf("\n");
          return 1;
        }
      }
      else{
        declar_array( name , lexed_buffer[i].word);

        i++;
      }
    }
    else{
      printf("could not parse\n");
      printf("%d",i);
      return 1;
    }
  }
  return 0; 
}
