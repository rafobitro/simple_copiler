#pragma once
#include <stdbool.h>

typedef enum {
  FUNCTION = 0, // #FUNCTION_NAME is a way to call function 
  VARIABLE_TYPE, // idk if i need this rigth now i dont even chack this 
  VARIABLE, // any text without seperator
  NUMBER, // just number 
  ASIGNER, // = 
  ARRAY_NUMBER, // anything inside []
  FUN_DEF, // @FUNCTION_NAME is way to define function 
  ASEMBLY, // anything inside "" is asembly code lexing without chacking 
  IF,  // is ? 
  LOOP,// is duble ?? 
  START, // { 
  END, // } 
  CHAR, // 'ANY SINGLE CHARACTER '
  LPAREN, // ( 
  RPAREN, // )
  COMMA, // ,
  ERROR, // i dont use this
}TokenType;

typedef struct {
    TokenType type;
    char *word;
    int line;
} Token;


extern char *source_buffer;
extern Token *lexed_buffer;
extern int lexed_count;
extern long file_size;



bool is_letter(char c);
bool is_digit(char c);
bool is_seperator(char c);
int lexer(void);
void lexer_debug(void);
int copy_to_lexed_buffer(int start, int end, int word_count);


