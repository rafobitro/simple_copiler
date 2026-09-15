#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "text_buffer.h"



bool type_chack(char* type, TokenType t_type){
  if(t_type == NUMBER && strcmp(type,"$number")==0)
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

void parser_error(char* message, Token token) {
    printf("\033[1;31m[PARSER ERROR]: %s\033[0m\n", message);
    printf("\033[31m  -> Line   : %d\n", token.line);
    printf("  -> Token  : '%s'\n\033[0m", token.word);
}

int parse_declaration(int *i){
    char* type=lexed_buffer[*i].word;
    (*i)++;
    if(*i>=lexed_count){
      parser_error("forgot to write variable name, could not parse", lexed_buffer[*i-1]);
      return 1;
    }
    if(lexed_buffer[*i].type!=VARIABLE ){
       parser_error("invalid structure after variable type, you should write a variable name", lexed_buffer[*i]);
      return 1;
    }
    char* name=lexed_buffer[*i].word;
    (*i)++;
    if(*i>=lexed_count || lexed_buffer[*i].type != ASIGNER && lexed_buffer[*i].type != ARRAY_NUMBER){
      declar_variable( name);
    }
    else if( lexed_buffer[*i].type == ASIGNER ){
      declar_variable( name);
      (*i)++;
      if(*i>=lexed_count){
        parser_error("forgot to assign value", lexed_buffer[*i - 1]);
        return 1;
      }
      if(type_chack(type,lexed_buffer[*i].type)){
        asign_variable(name,lexed_buffer[*i].word);
        (*i)++;
      }
      else{
        parser_error("type mismatch ", lexed_buffer[*i]);
        return 1;
      }
    }
    else{
      declar_array( name , lexed_buffer[*i].word);
      (*i)++;
    }
  return 0;

}

int parse_asigner(int *i){
  char* name = lexed_buffer[*i].word;
  //if(!is_variable_declared) return 1;
  (*i)++;
  if(*i>=lexed_count){
    parser_error("forgot to finish expresion, could not parse", lexed_buffer[*i-1]);
    return 1;
  }
  if(lexed_buffer[*i].type==ASIGNER){
    (*i)++;
    if(lexed_buffer[*i].type==NUMBER){
      asign_variable(name,lexed_buffer[*i].word);
      (*i)++;

    }
    else{
      parser_error("you shoiuld write a value afther asigner", lexed_buffer[*i-1]);
      return 1;

    }
  }
  else if(lexed_buffer[*i].type=ARRAY_NUMBER){
    char* array_number=lexed_buffer[*i].word;
    (*i)++;
    if(lexed_buffer[*i].type==ASIGNER){
      (*i)++;
      if(lexed_buffer[*i].type==NUMBER){
        asign_variable_array(name,lexed_buffer[*i].word,array_number);
        (*i)++;

      }
      else{
        parser_error("you shoiuld write a value afther asigner", lexed_buffer[*i-1]);
        return 1;

      }
    }
  }
  else{
    parser_error("if expresion starts with variable it should or be array number or asigner", lexed_buffer[*i-1]);
    return 1;

  }
  return 0;

}

int parser(){
  int i = 0;
  while(i<lexed_count){
    if(lexed_buffer[i].type==VARIABLE_TYPE && is_variable_type_exsist(lexed_buffer[i].word)){
      if (parse_declaration(&i))  return 1;
    }
    else if(lexed_buffer[i].type == ASEMBLY ){
      append_text_buffer(main_buffer,lexed_buffer[i++].word);
    }
    else if(lexed_buffer[i].type == VARIABLE){
      if(parse_asigner(&i)) return 1;
    }
    else{
      parser_error("could not parse",lexed_buffer[i]);
      return 1;
    }
  }
  return 0; 
}
