#pragma once
#include <stdbool.h>

typedef enum {
  FUNCTION = 0,
  VARIABLE_TYPE,
  VARIABLE,
  NUMBER,
  STRING,
  ASIGNER,
  ARRAY_NUMBER,//not yet in lexer
  ERROR,
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


