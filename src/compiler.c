#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "text_buffer.h"

typedef enum {
  FUNCTION = 0,
  VARIABLE_TYPE,
  VARIABLE,
  NUMBER,
  STRING,
  ERROR,
}TokenType;

typedef struct {
    TokenType type;
    char *word;
    int line;
} Token;

char *source_buffer;
Token *lexed_buffer;
int lexed_count=0;
int file_to_buffer(char* filename);
long file_size;

TextBuffer *declaration_buffer;//= init_text_buffer();
TextBuffer *main_buffer;// = init_text_buffer();

bool is_letter(char c);
bool is_digit(char c);
bool is_seperator(char c);
int copy_to_lexed_buffer(int start, int end, int word_count); 
int lexer();
void lexer_debug();


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
  
  if(file_to_buffer(src_file))return 1;
  if(lexer())return 1;

  declaration_buffer=init_text_buffer();
  append_text_buffer(declaration_buffer,".data\n");
  
  main_buffer=init_text_buffer();
  append_text_buffer(main_buffer,".text\n");
  append_text_buffer(main_buffer,".globl main\n");
  append_text_buffer(main_buffer,"main:\n");

  append_text_buffer(main_buffer,"li $v0, 10\n");
  append_text_buffer(main_buffer,"syscall\n");

  FILE  *outputptr;
  outputptr = fopen(input_file,"w");
  text_buffer_to_file(declaration_buffer,outputptr);
  text_buffer_to_file(main_buffer,outputptr);

  
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
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    printf("faild to open source file");
    return 1;
  }
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);

  source_buffer=malloc(file_size);
  fread(source_buffer,1,file_size,file);
  source_buffer[file_size] = '\0';
  fclose(file);

  
  return 0;
}

int lexer(){
  // it should be enogef and i dont like idea of of constantly growing alocations . like c++ style vetor
  lexed_buffer = malloc(file_size*sizeof(Token));

  int line_count=0;
  char c=0;
  char c2=source_buffer[0];
  int i=0;
  while(c2!='\0'){
    c=source_buffer[i];
    c2=source_buffer[i+1];
    if(c==' '){
      i++;
      continue;
    } 
    else if(c=='\n'){
      line_count++;
      i++;
      continue;
    }
    else if(is_digit(c)){
      //last char is \0 so it will not go out of bounds
      int start=i;
      while(is_digit(c2)){
        i++;
        c2=source_buffer[i+1];
      }
      
      if(is_seperator(c2)){  // if afther number is space or other seperator 
        lexed_buffer[lexed_count].type=NUMBER;
        copy_to_lexed_buffer(start,i+1,lexed_count);
        lexed_buffer[lexed_count].line=line_count;
        lexed_count++;
        i++;
        continue;
         
      } // if not it is invalid argument like 124eser cen not be part of (my) programing language .
      else{
        printf(" combination of number and letters is not alowed like in this line  "); 
        printf("%d\n", line_count);
        return 1;
      }
    }
    else if(is_letter(c) || c=='$' || c=='#'){
      int start=i;
      while(!is_seperator(c2)){
        i++;
        c2=source_buffer[i+1];
      }
      
      if(c=='$'){
        lexed_buffer[lexed_count].type=VARIABLE_TYPE;
      }
      else if(c=='#'){
        lexed_buffer[lexed_count].type=FUNCTION;
      }
      else{
        lexed_buffer[lexed_count].type=VARIABLE;
      }
      copy_to_lexed_buffer(start,i+1,lexed_count);
      lexed_buffer[lexed_count].line=line_count;
      lexed_count++;
      i++;
      continue;
      
    }
    else if(c=='"'){
      int start=i;
      while(true){
        if(c2=='"') break;
        if(c2=='\0') {
          printf("ypu did not closed a  string wich you started in line");
          printf("%d\n", line_count);
          return 1;
        }
        i++;
        c2=source_buffer[i+1];
      }
      lexed_buffer[lexed_count].type = STRING;
      copy_to_lexed_buffer(start+1,i+1,lexed_count);
      lexed_buffer[lexed_count].line=line_count;
      lexed_count++;
      i+=2;
      continue;

    }
    else{
      printf("unrecognise symbole  ");
      printf("%c\n",c);
      return 1;
    }
    
    
  }
    return 0;
}

bool is_letter(char c){
  return ( (c>='A' && c<='Z') || (c>='a' && c<='z') || (c == '_') );
}

bool is_digit(char c){
  return (c>='0' && c<='9');
}

bool is_seperator(char c){
  return ((c==' ') || (c=='"')) || (c=='\n') || (c=='\0'); // i will add more seperators letter if needed

}

int copy_to_lexed_buffer (int start , int end, int word_count){
    int size=end -start +1;
    char *word= malloc(size);
    if(word==NULL){
      printf("no space to alloc word");
      return 1;  
    }
    for(int j=0,i=start;i<end;i++,j++){
      word[j]=source_buffer[i];
    }
    word[size-1]='\0';
    lexed_buffer[word_count].word=word;
    return 0;
}


void lexer_debug(){

  printf("====================LEXER DEBUG====================\n");
  for(int i=0;i<lexed_count;i++){
    if (lexed_buffer[i].type==STRING) printf("STRING\t\t");
    if (lexed_buffer[i].type==NUMBER) printf("NUMBER\t\t");
    if (lexed_buffer[i].type==FUNCTION) printf("FUNCTION\t\t");
    if (lexed_buffer[i].type==VARIABLE) printf("VARIABL\t\t");
    if (lexed_buffer[i].type==VARIABLE_TYPE) printf("VARIABLE_TYPE\t\t");

    printf(lexed_buffer[i].word);
    printf("\t\t");
    printf("%d\n", lexed_buffer[i].line);
    printf("\n");
  }
}



