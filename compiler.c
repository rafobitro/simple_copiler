

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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
    char word[64];
    int line;
} Token;

char source_buffer[8192];
Token lexed_buffer[2048];
int cursor = 0;
int file_to_buffer(char* filename);



bool is_letter(char c);
bool is_digit(char c);
bool is_seperator(char c);
void copy_to_lexed_buffer(int start, int end, int word_count);

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
  
  FILE  *outputptr;
  outputptr = fopen(input_file,"w");
  fprintf(outputptr, ".data\n");
  fprintf(outputptr, ".text\n");
  fprintf(outputptr, ".globl main\n");
  fprintf(outputptr, "main:\n");
  
  fprintf(outputptr, "li $v0, 10\n");
  fprintf(outputptr, "syscall\n");
  
  return 0;
}

int file_to_buffer(char* filename){
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    printf("faild to open source file");
    return 1;
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);
  if(file_size>=8192){
    printf("source file is too big . This compiler cen not handle 8192 characther + files. \n why?  : becouse it is compiler version 0.00001 ");
    return 1;
  }
  fread(source_buffer, 1, file_size, file);
  fclose(file);
  return 0;
}

int lexer(int file_size){
  int line_count=0;
  int lexed_count=0;
  int i=0;
  while(i<file_size){
    char c=source_buffer[i];
    char c2=source_buffer[i+1];
    if(c==' '){
      i++;
      continue;
    }
    
    if(c=='\n'){
      line_count++;
      i++;
      continue;
    }
    if(is_digit(c)){
      //last char is \0 so it will not go out of bounds
      int start=i;
      while(!is_digit(c2)){
        i++;
        c2=source_buffer[i+1];
      }
      
      if(is_seperator(c2)){  // if afther number is space or other seperator 
        lexed_buffer[lexed_count].type=NUMBER;
        copy_to_lexed_buffer(start,i,lexed_count);
        lexed_buffer[lexed_count].line=line_count;
        lexed_count++;
        continue;
         
      } // if not it is invalid argument like 124eser cen not be part of (my) programing language .
      else{
        printf(" combination of number and letters is not alowed like in line  "); 
        printf("%d\n", line_count);
        return 1;
      }
    }
    if(is_letter(c) || c=='$' || c=='#'){
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
      copy_to_lexed_buffer(start,i,lexed_count);
      lexed_buffer[lexed_count].line=line_count;
      lexed_count++;
      
    }
    if(c=='"'){
      int start=i;
      while(true){
        if(c2=='"') break;
        if(c2=='\0') {
          printf("ypu did not closed a  string wich you started in line");
          printf("%d\n", line_count);
        }
        i++;
        c2=source_buffer[i+1];
      }
      lexed_buffer[lexed_count].type = STRING;
      copy_to_lexed_buffer(start,i,lexed_count);
      lexed_buffer[lexed_count].line=line_count;
      lexed_count++;
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
  return ((c==' ') || (c=='"')); // i will add more seperators letter if needed

}

void copy_to_lexed_buffer (int start , int end, int word_count){
    int size=end -start +1;
    char word[size];
    for(int j=0,i=start;i<end;i++,j++){
      word[j]=source_buffer[i];
    }
    word[size-1]='\0';
    strcpy(lexed_buffer[word_count].word, word);
}
