#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "text_buffer.h"

#define MAX_PARAMS 4
#define MAX_VARS 1000


static int label_counter = 0;

static char *declared[MAX_VARS];
static int declared_count = 0;

bool is_variable_type_exsist(char* word){
    if(strcmp(word,"number") ==0)
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

static char* make_label(char* scope, char* name){
  size_t scope_len = strlen(scope);
  char* label = malloc(scope_len + strlen(name) + 3);
  if(label == NULL) return NULL;
  strcpy(label, scope);
  strcat(label, "__");
  strcat(label, name);
  return label;
}

static char* find_label(char* label){
  for(int k=0; k<declared_count; k++){
    if(strcmp(declared[k], label) == 0)
      return declared[k];
  }
  return NULL;
}

static char* declare_name(char** scopes, Token t){
  char* label = make_label(scopes[0], t.word);
  if(label == NULL){
    parser_error("no space to alloc variable name", t);
    return NULL;
  }
  if(find_label(label) != NULL){
    parser_error("variable is already declared in this scope", t);
    free(label);
    return NULL;
  }
  if(declared_count >= MAX_VARS){
    parser_error("too many variables", t);
    free(label);
    return NULL;
  }
  declared[declared_count++] = label;
  return label;
}

static char* lookup_name(char** scopes, Token t){
  for(int s=0; scopes[s] != NULL; s++){
    char* label = make_label(scopes[s], t.word);
    if(label == NULL){
      parser_error("no space to alloc variable name", t);
      return NULL;
    }
    char* found = find_label(label);
    free(label);
    if(found != NULL) return found;
  }
  parser_error("variable was never declared", t);
  return NULL;
}

//// NEW: expand_asm. Copies the asm text and replaces every %name with the label of that variable
//// (found with lookup_name, so scopes work). %% gives a literal %. Returns a malloc'd string or NULL on error.
static char* expand_asm(Token asm_token, char** scopes){
  char* text = asm_token.word;
  size_t cap = strlen(text) * 2 + 64;
  size_t len = 0;
  char* out = malloc(cap);
  if(out == NULL){
    parser_error("no space to alloc asembly text", asm_token);
    return NULL;
  }

  for(size_t k=0; text[k] != '\0'; ){
    const char* piece;
    size_t piece_len;
    char one[2];

    if(text[k] == '%' && text[k+1] == '%'){
      one[0] = '%'; one[1] = '\0';
      piece = one; piece_len = 1;
      k += 2;
    }
    else if(text[k] == '%'){
      size_t start = k + 1;
      size_t end = start;
      while(is_letter(text[end]) || is_digit(text[end])) end++;
      if(end == start){
        parser_error("expected a variable name after % in asembly, use %% for a real %", asm_token);
        free(out);
        return NULL;
      }
      Token name_token = asm_token;
      name_token.word = malloc(end - start + 1);
      if(name_token.word == NULL){
        parser_error("no space to alloc variable name", asm_token);
        free(out);
        return NULL;
      }
      memcpy(name_token.word, text + start, end - start);
      name_token.word[end - start] = '\0';

      char* label = lookup_name(scopes, name_token);
      free(name_token.word);
      if(label == NULL){
        free(out);
        return NULL;
      }
      piece = label; piece_len = strlen(label);
      k = end;
    }
    else{
      piece = text + k; piece_len = 1;
      k++;
    }

    if(len + piece_len + 2 > cap){
      cap = (len + piece_len + 2) * 2;
      char* bigger = realloc(out, cap);
      if(bigger == NULL){
        parser_error("no space to alloc asembly text", asm_token);
        free(out);
        return NULL;
      }
      out = bigger;
    }
    memcpy(out + len, piece, piece_len);
    len += piece_len;
  }
  out[len] = '\0';
  return out;
}

void parser_free(void){
  for(int k=0; k<declared_count; k++)
    free(declared[k]);
  declared_count = 0;
}

int parse_declaration(int *i, char** scopes){
    (*i)++; 
    if(*i>=lexed_count){
      parser_error("forgot to write variable name, could not parse", lexed_buffer[*i-1]);
      return 1;
    }
    if(lexed_buffer[*i].type!=VARIABLE ){
       parser_error("invalid structure after variable type, you should write a variable name", lexed_buffer[*i]);
      return 1;
    }

    char* name = declare_name(scopes, lexed_buffer[*i]);
    if(name == NULL) return 1;
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

int parse_asigner(int *i, char** scopes){
  char* name = lookup_name(scopes, lexed_buffer[*i]);
  if(name == NULL) return 1;
  (*i)++;
  if(*i>=lexed_count){
    parser_error("forgot to finish expresion, could not parse", lexed_buffer[*i-1]);
    return 1;
  }
  if(lexed_buffer[*i].type==ASIGNER){
    (*i)++;
    if(*i>=lexed_count){
      parser_error("forgot to write a value afther asigner", lexed_buffer[*i-1]);
      return 1;
    }
    if(lexed_buffer[*i].type==NUMBER || lexed_buffer[*i].type==CHAR){
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
    if(*i>=lexed_count){
      parser_error("forgot to finish array assignment", lexed_buffer[*i-1]);
      return 1;
    }
    if(lexed_buffer[*i].type==ASIGNER){
      (*i)++;
      if(*i>=lexed_count){
        parser_error("forgot to write a value afther asigner", lexed_buffer[*i-1]);
        return 1;
      }
      if(lexed_buffer[*i].type==NUMBER || lexed_buffer[*i].type==CHAR ){
        asign_variable_array(name,lexed_buffer[*i].word,array_number);
        (*i)++;

      }
      else{
        parser_error("you shoiuld write a value afther asigner", lexed_buffer[*i-1]);
        return 1;

      }
    }
    else{
      parser_error("array element must be followed by '='", lexed_buffer[*i-1]);
      return 1;
    }
  }
  else{
    parser_error("if expresion starts with variable it should or be array number or asigner", lexed_buffer[*i-1]);
    return 1;

  }
  return 0;

}


int parse_fundef(int *i, char** scopes){
  if(strcmp(scopes[0], "main") != 0){
    parser_error("you can not define a function inside a function", lexed_buffer[*i]);
    return 1;
  }
  char* name = lexed_buffer[*i].word;
  if(strcmp(name, "main") == 0){
    parser_error("main is the name of the global scope, use another function name", lexed_buffer[*i]);
    return 1;
  }
 
  char* function_scopes[3] = { name, "main", NULL };

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

    char* param_label = declare_name(function_scopes, lexed_buffer[*i]);
    if(param_label == NULL) return 1;
    param_names[param_count++] = param_label;
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

  append_text_buffer(main_buffer, "addi $sp, $sp, -4\n");
  append_text_buffer(main_buffer, "sw $ra, 0($sp)\n");

  for(int p=0; p<param_count; p++){ 
    declar_variable(param_names[p]);
  }
  for(int p=0; p<param_count; p++){
    codegen_function_param_store(param_names[p], p);
  }

  if(parse_statements(i, true, function_scopes)) return 1;

  append_text_buffer(main_buffer, "lw $ra, 0($sp)\n");
  append_text_buffer(main_buffer, "addi $sp, $sp, 4\n");

  codegen_function_end(name);
  return 0;
}


int parse_funcall(int *i, char** scopes){
  // i am not chacking if  function exsist or not i am not chacking ig i use rigth amout of parameters or not yet .
  // meybe i will do second run parser cheacker to do it . 
  char *name = lexed_buffer[*i].word;//REMEMBER TO ADD LINE NUMBER
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

    char *arg_word = lexed_buffer[*i].word;
    TokenType arg_type = lexed_buffer[*i].type;
    if(arg_type == VARIABLE){
      arg_word = lookup_name(scopes, lexed_buffer[*i]);
      if(arg_word == NULL) return 1;
    }
    (*i)++;

    if(*i<lexed_count && lexed_buffer[*i].type == ARRAY_NUMBER && lexed_buffer[*i].word[0] == '\0'){
      codegen_load_arg_address(arg_word, arg_count);
      (*i)++; 
    }
    else{
      codegen_load_arg(arg_word, arg_type, arg_count);
    }

    arg_count++;

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

static bool is_comperison(TokenType type){
  if(type == EQUAL) return true;
  if(type == NOT_EQUAL) return true;
  if(type == BIGER) return true;
  if(type == BIGER_OR_EQUAL) return true;
  if(type == SMALLER) return true;
  if(type == SMALLER_OR_EQUAL) return true;
  return false;
}

int parse_branch(int *i,bool is_loop, char** scopes){
  
  int id = label_counter++;
  char* name="BRUNCH";
  (*i)++;
  if(is_loop){
    append_text_buffer(main_buffer, "START_");
    append_text_buffer(main_buffer, name);
    append_text_buffer(main_buffer, int_to_string(id));
    append_text_buffer(main_buffer, ":\n");

  }

  if(*i>=lexed_count || lexed_buffer[*i].type != LPAREN){
    parser_error("expected '(' after ? or ??", lexed_buffer[*i-1]);
    return 1;
  }
  (*i)++;
    
    TokenType compereson_type;
  //move 2 values to registers 1 and 2 for compereson 
   for(int j=0; j<2; j++){
 
    if(*i>=lexed_count ||  lexed_buffer[*i].type != NUMBER &&  lexed_buffer[*i].type != CHAR &&  lexed_buffer[*i].type != VARIABLE){
      parser_error("Branches should have 2 values included A and B ", lexed_buffer[*i]);
      return 1;
    }

    Token value = lexed_buffer[*i];
    if(value.type == VARIABLE){
      value.word = lookup_name(scopes, lexed_buffer[*i]);
      if(value.word == NULL) return 1;
    }
    codegen_load_brunch_value(value, (j==0 ? "$t1" : "$t2") );
    (*i)++;
 
    if(j==0 &&  *i<lexed_count ){
      if(is_comperison(lexed_buffer[*i].type)){
        compereson_type=lexed_buffer[*i].type;
        (*i)++;
      
      }
      else{
        parser_error(" ther was no compereson in brunch ", lexed_buffer[*i-1]);
        return 1;
      }
    }
  }
  if(*i>=lexed_count || lexed_buffer[*i].type != RPAREN){
    parser_error("brunch never closed with ')'", lexed_buffer[*i-1]);
    return 1;
  }
   (*i)++;

  if(*i>=lexed_count || lexed_buffer[*i].type != START){
    parser_error("expected '{' to start branch body", lexed_buffer[*i-1]);
    return 1;
  }
  (*i)++;
  
  codegen_compear_values(name, id,compereson_type);

  if(parse_statements(i, true, scopes)) return 1;
  
  if(is_loop){
    append_text_buffer(main_buffer, "j START_");
    append_text_buffer(main_buffer, name);
    append_text_buffer(main_buffer, int_to_string(id));
    append_text_buffer(main_buffer, "\n");
  }

  append_text_buffer(main_buffer, name);
  append_text_buffer(main_buffer, int_to_string(id));
  append_text_buffer(main_buffer, ":\n");
  return 0; 
}


int parse_statements(int *i, bool function, char** scopes){
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
      if (parse_declaration(i, scopes)) return 1;
    }
    else if(lexed_buffer[*i].type == ASEMBLY){
     
      char* expanded = expand_asm(lexed_buffer[*i], scopes);
      if(expanded == NULL) return 1;
      append_text_buffer(main_buffer, expanded);
      append_text_buffer(main_buffer, "\n");
      free(expanded);
      (*i)++;
    }
    else if(lexed_buffer[*i].type == VARIABLE){
      if(parse_asigner(i, scopes)) return 1;
    }
    else if(lexed_buffer[*i].type == FUN_DEF){
      if(parse_fundef(i, scopes)) return 1;
    }
    else if(lexed_buffer[*i].type == FUNCTION){ //inline asembly and usage of a function pased variables in them ?
                                                //return is nececerly . idea i think is creating return variable  in function call time and with defoult value of 0 ? 
                                                //need to threat function us values mening alow use same way us numbers variables and chars
      if(parse_funcall(i, scopes)) return 1;
    }
    else if(lexed_buffer[*i].type == IF){
      if(parse_branch(i,false, scopes))return 1; 
    }
    else if(lexed_buffer[*i].type == LOOP){
      if(parse_branch(i,true, scopes))return 1; 
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
  label_counter = 0;
  char* global_scopes[2] = { "main", NULL };
  return parse_statements(&i, false, global_scopes);
}
