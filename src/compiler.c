#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "text_buffer.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

#define MAX_INCLUDES 100
#define NAME_SIZE 100

TextBuffer *declaration_buffer;
TextBuffer *main_buffer;

//exemple of include library code 
// % a.yu b.yu %

char *include_stack[MAX_INCLUDES];
long header_end[MAX_INCLUDES];
int stack_index = 0;
int stack_size=0;
long source_buffer_size = 0;
char include_buffer[NAME_SIZE];

// returns file in char* and sets size
char* read_file(const char *name, long *size){ 
  FILE *f = fopen(name, "rb");
  if(f == NULL){
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  rewind(f);
  char *buf = malloc(n + 1);
  if(buf == NULL){ fclose(f); return NULL; }
  size_t got = fread(buf, 1, n, f);
  fclose(f);
  buf[got] = '\0';
  *size = (long)got;
  return buf;
}

int add_to_stack(char *filename){
  if(stack_index >= MAX_INCLUDES){
    printf("Too many includes (max is 100). Most likely a recursive include, last name: %s\n", filename);
    return 1;
  }
  include_stack[stack_index++] = filename;
  stack_size+=1;
  return 0;
}

int check_extension(char *filename){
  int len = strlen(filename);
  if(len < 4){
    printf("invalid exstension use .yu file extension ");
    return 1;
  }
  if(filename[len - 3] == '.' &&
     filename[len - 2] == 'y' &&
     filename[len - 1] == 'u'){
    return 0;
  }
  printf("invalid exstension use .yu file extension ");
  return 1;
}

int is_space(char c){
  return c==' ' || c=='\t' || c=='\r' || c=='\n';
}

// recursive function
int populate_stack(int index){
  if(index>=stack_size) return 0;
  if(check_extension(include_stack[index])) return 1;

  long n;
  char *text = read_file(include_stack[index], &n);
  
  if(text == NULL){
    printf("cannot open file %s\n", include_stack[index]);
    return 1;
  }

  for(long i=0;i<n;i++){
    char c=text[i];
    if(is_space(c)){
    }
    else if (c == '%') {
      i++;
      while (i < n && text[i] != '%') {
        
        while (i < n && text[i] != '%' && is_space(text[i])) {
            i++;
        }

        if (i >= n || text[i] == '%') break;

        long start = i;
        while (i < n && text[i] != '%' && !is_space(text[i])) {
           i++;
        }
        long end = i;

        long size = end-start;
        char *word = malloc(size + 1);
        if (word == NULL) {
          printf("no space to alloc word\n");
          free(text);
          return 1;
        }

        strncpy(word, &text[start], size);
        word[size] = '\0'; 

        if (add_to_stack(word)){ free(word); free(text); return 1; }

      }
      if(i<n){
        header_end[index]=i+1;
        break;
      } 
      else{
        printf("you did not close include section with %% in file %s\n", include_stack[index]);
        free(text);
        return 1;
      }
    }
    else{
      header_end[index]=i;
      break;
      
    }
  }
  source_buffer_size += n - header_end[index] + 1;
  free(text);

  return populate_stack(index+1);
}

int file_to_source(){
  long pos = 0;
  for(int i = stack_index - 1; i >= 0; i--){
    long n;
    char *text = read_file(include_stack[i], &n);
    if(text == NULL){
      printf("cannot open file %s\n", include_stack[i]);
      return 1;
    }

    long len = n - header_end[i];
    memcpy(source_buffer + pos, text + header_end[i], len);
    pos += len;
    source_buffer[pos++] = '\n';
    free(text);
  }
  source_buffer[pos] = '\0';
  file_size = pos;
  return 0;
}

int main(int args,char *argv[]){

  if(args<2){
    printf("no sourcefile");
    return 1;
  }

  char *src_file = argv[1];
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
  
  if(add_to_stack(src_file)) return 1;
  if(populate_stack(0)) return 1;

  source_buffer = malloc(source_buffer_size + 1);
  if(source_buffer == NULL){
    printf("no space for source buffer");
    return 1;
  }
  if(file_to_source()) return 1;

  if(lexer())return 1;
  if(parser())return 1;
 
  append_text_buffer(main_buffer,"li $v0, 10\n");
  append_text_buffer(main_buffer,"syscall\n");

  FILE  *outputptr;
  outputptr = fopen(input_file,"w");
  if(outputptr == NULL){
    printf("cannot write output file");
    return 1;
  }
  text_buffer_to_file(declaration_buffer,outputptr);
  text_buffer_to_file(main_buffer,outputptr);
  fclose(outputptr);
  
  //lexer_debug();
  for(int i=0;i<lexed_count;i++)
    free(lexed_buffer[i].word);
  for(int i=1;i<stack_index;i++)// 0 index was not malloced
    free(include_stack[i]);
  free(source_buffer);
  free(lexed_buffer);
  free_text_buffer(declaration_buffer);
  free_text_buffer(main_buffer);
  return 0;
}
