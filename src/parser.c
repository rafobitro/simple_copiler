#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "text_buffer.h"

#define MAX_PARAMS 4

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
    if(*i>=lexed_count || (lexed_buffer[*i].type != ASIGNER && lexed_buffer[*i].type != ARRAY_NUMBER)){
      declar_variable( name);
    }
    else if( lexed_buffer[*i].type == ASIGNER ){
      declar_variable( name);
      (*i)++;
      if(*i>=lexed_count){
        parser_error("forgot to assign value", lexed_buffer[*i - 1]);
        return 1;
      }
      asign_variable(name,lexed_buffer[*i].word);
      (*i)++;
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
  else if(lexed_buffer[*i].type==ARRAY_NUMBER){
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


int parse_fundef(int *i){
  char* name = lexed_buffer[*i].word;
  // if(!is_function_defind()) return 1;
  (*i)++;
  if(*i>=lexed_count || lexed_buffer[*i].type != LPAREN){
    parser_error("expected '(' after function name", lexed_buffer[*i-1]);
    return 1;
  }
  (*i)++;

  char *param_names[MAX_PARAMS];
  int param_count = 0;

   while(*i<lexed_count && lexed_buffer[*i].type != RPAREN){
    if(lexed_buffer[*i].type != VARIABLE){
      parser_error("expected parameter name", lexed_buffer[*i]);
      return 1;
    }
    if(param_count >= MAX_PARAMS){
      parser_error("too many parameters, max 4 allowed", lexed_buffer[*i]);
      return 1;
    }
    param_names[param_count++] = lexed_buffer[*i].word;
    (*i)++;

    if(*i<lexed_count && lexed_buffer[*i].type == COMMA){
      (*i)++;
      if(*i>=lexed_count || lexed_buffer[*i].type == RPAREN){
        parser_error("expected parameter after ',' ", lexed_buffer[*i]);
        return 1;
      }
    }
  }

  if(*i>=lexed_count){
    parser_error("parameter list never closed with ')'", lexed_buffer[*i-1]);
    return 1;
  }
  (*i)++;

  if(*i>=lexed_count || lexed_buffer[*i].type != START){
    parser_error("expected '{' to start function body", lexed_buffer[*i-1]);
    return 1;
  }
  (*i)++;


  codegen_function_start(name);
  for(int p=0; p<param_count; p++){ 
    declar_variable(param_names[p]);
  }
  for(int p=0; p<param_count; p++){
    codegen_function_param_store(param_names[p], p);
  }

  if(parse_statements(i, true)) return 1;

  codegen_function_end(name);
  return 0;
}


int parse_funcall(int *i){
  // i am not chacking if  function exsist or not i am not chacking ig i use rigth amout of parameters or not yet .
  // meybe i will do second run parser cheacker to do it . 
  char *name = lexed_buffer[*i].word + 1;
  (*i)++;

  if(*i>=lexed_count || lexed_buffer[*i].type != LPAREN){
    parser_error("expected '(' after function call", lexed_buffer[*i-1]);
    return 1;
  }
  (*i)++;

  int arg_count = 0;
  while(*i<lexed_count && lexed_buffer[*i].type != RPAREN){
    if(lexed_buffer[*i].type != NUMBER && lexed_buffer[*i].type != VARIABLE){
      parser_error("expected number or variable as argument", lexed_buffer[*i]);
      return 1;
    }
    if(arg_count >= MAX_PARAMS){
      parser_error("too many arguments, max 4 allowed", lexed_buffer[*i]);
      return 1;
    }
    codegen_load_arg(lexed_buffer[*i].word, lexed_buffer[*i].type, arg_count);
    arg_count++;
    (*i)++;

    if(*i<lexed_count && lexed_buffer[*i].type == COMMA){
      (*i)++;
      if(*i>=lexed_count || lexed_buffer[*i].type == RPAREN){
        parser_error("expected argument after ','", lexed_buffer[*i]);
        return 1;
      }
    }
  }
  if(*i>=lexed_count){
    parser_error("function call never closed with ')'", lexed_buffer[*i-1]);
    return 1;
  }
  (*i)++;

  codegen_function_call(name);
  return 0;
}

int parse_statements(int *i, bool function){
  while((*i)<lexed_count){
    if(lexed_buffer[*i].type == END){
      if(!function){
         parser_error("you closed a block with '}' that was never opened", lexed_buffer[*i]);
         return 1;
      }
      (*i)++;
      return 0;
    }
    else if(lexed_buffer[*i].type==VARIABLE_TYPE && is_variable_type_exsist(lexed_buffer[*i].word)){
      if (parse_declaration(i)) return 1;
    }
    else if(lexed_buffer[*i].type == ASEMBLY){
      append_text_buffer(main_buffer, lexed_buffer[*i].word);
      (*i)++;
    }
    else if(lexed_buffer[*i].type == VARIABLE){
      if(parse_asigner(i)) return 1;
    }
    else if(lexed_buffer[*i].type == FUN_DEF){
      if(parse_fundef(i)) return 1;
    }
    else if(lexed_buffer[*i].type == FUNCTION){
      if(parse_funcall(i)) return 1;
    }
    else{
      parser_error("could not parse", lexed_buffer[*i]);
      return 1;
    }
  }
  if(function){
    parser_error("function never closed with '}'", lexed_buffer[*i - 1]);
    return 1;
  }
  return 0;
}

int parser(){
  int i = 0;
  return parse_statements(&i, false);
}
