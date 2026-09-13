#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"



bool type_chack(char* type, TokenType t_type){
  if(t_type == NUMBER && strcmp(type,"$number")==0)
    return true;
  if(t_type == STRING && strcmp(type,"$string")==0)
    return true;
  return false;
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
