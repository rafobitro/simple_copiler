#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "lexer.h"


char *source_buffer;
Token *lexed_buffer;
int lexed_count=0;
long file_size;



int lexer(){
  // it should be enogef and i dont like idea of of constantly growing alocations . like c++ style vetor
  lexed_buffer = calloc(file_size,sizeof(Token));
  int line_count=1;
  char c=0;
  char c2=source_buffer[0];
  int i=0;

  while(i<file_size){

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
    else if(c=='\r'){
      i++;
      continue;
    }
    else if(c=='='){ 
      lexed_buffer[lexed_count].type=ASIGNER;
      copy_to_lexed_buffer(i,i+1,lexed_count);
      lexed_buffer[lexed_count].line=line_count;
      lexed_count++;
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
    else if(c=='['){
      int start=i;
      while(true){
        if(c2==']') break;
        if(c2=='\0') {
          printf("ypu did not closed a [ ] wich you started in line");
          printf("%d\n", line_count);
          return 1;
        }
        if(c2=='\n') line_count++;

        i++;
        c2=source_buffer[i+1];
      }
      lexed_buffer[lexed_count].type = ARRAY_NUMBER;
      copy_to_lexed_buffer(start+1,i+1,lexed_count);
      lexed_buffer[lexed_count].line=line_count;
      lexed_count++;
      i+=2;
      continue;
    }
    else if(c=='"'){
      int start=i;
      while(true){
        if(c2=='"') break;
        if(c2=='\0') {
          printf("ypu did not closed a string wich you started in line");
          printf("%d\n", line_count);
          return 1;
        }
        if(c2=='\n') line_count++;

        i++;
        c2=source_buffer[i+1];
      }
      lexed_buffer[lexed_count].type = STRING;
      copy_to_lexed_buffer(start,i+2,lexed_count);
      lexed_buffer[lexed_count].line=line_count;
      lexed_count++;
      i+=2;
      continue;
    }
    else{
      printf("unrecognise symbole  ");
      printf("%c\n",source_buffer[i]);
      printf("file size ");
      printf("%ld\n", file_size);
      printf("index ");
      printf("%d ", i);
      return 1;
    }
    
    
  }
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
    if (lexed_buffer[i].type==ARRAY_NUMBER) printf("ARRAY_NUMBER\t\t");
    if (lexed_buffer[i].type==ASIGNER) printf("ASIGNER\t\t");
    printf("%s",lexed_buffer[i].word);
    printf("\t\t");
    printf("%d\n", lexed_buffer[i].line);
    printf("\n");
  }
}




bool is_letter(char c){
  return ( (c>='A' && c<='Z') || (c>='a' && c<='z') || (c == '_') );
}

bool is_digit(char c){
  return (c>='0' && c<='9');
}

bool is_seperator(char c){
  return ((c==' ') || (c=='"') || (c=='\n') || (c=='\t') || (c=='\r') || (c=='\0') || (c=='=')) ;
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


