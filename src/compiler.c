#include <stdlib.h>
#include <stdio.h>
#include "text_buffer.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int file_to_buffer(char* filename);
TextBuffer *declaration_buffer;//= init_text_buffer();
TextBuffer *main_buffer;// = init_text_buffer();


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
  
  declaration_buffer=init_text_buffer();
  append_text_buffer(declaration_buffer,".data\n");
   
  main_buffer=init_text_buffer();
  append_text_buffer(main_buffer,".text\n");
  append_text_buffer(main_buffer,".globl main\n");
  append_text_buffer(main_buffer,"main:\n");
  append_text_buffer(main_buffer,"li $v0, 10\n");
  append_text_buffer(main_buffer,"syscall\n");
  
  if(file_to_buffer(src_file))return 1;
  if(lexer())return 1;
  if(parser());//return 1;
  
  FILE  *outputptr;
  outputptr = fopen(input_file,"w");
  text_buffer_to_file(declaration_buffer,outputptr);
  text_buffer_to_file(main_buffer,outputptr);
  fclose(outputptr);
  
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
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    printf("faild to open source file");
    return 1;
  }
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);
  source_buffer=malloc(file_size+1);
  fread(source_buffer,1,file_size,file);
  source_buffer[file_size] = '\0';
  //source_buffer[file_size+1] = '\0';
  fclose(file);
  
  return 0;
}
