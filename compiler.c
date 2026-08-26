// this is compiler for my new language first compiler i made
// i almost have 0 knowlage about compilers so all this proces is improvisation
// i will almost not look how to create compilers only improvise 
// language name is talos meaning () 
// compiler name i mimic meaning ()
// file exstension is .yu meanung ()
// yop


#include <stdio.h>
#include <stdlib.h>


int main(int args,char *argv[]){

  if(args<2){
    printf("no sourcefile");
    return 1;
  }
  char *src_name = argv[1];
  //char output_name;
  int dot_index=-1;

  for(int i=0;src_name[i] != '\0' ;i++){
    if(src_name[i] == '.')
      dot_index=i;
  }
  if(dot_index==-1 || 
    src_name[dot_index+1]!='y' ||
    src_name[dot_index+2]!='u' ||
    src_name[dot_index+3]!='\0' ){
    printf("invalid exstension use .yu file extension ");
    return 1;
  }
  
  char input_file[dot_index+2];
  for(int i=0;i<=dot_index ;i++){
    input_file[i]=src_name[i];
  }
  input_file[dot_index+1]='s';



  FILE  *outputptr;

  outputptr = fopen(input_file,"w");

  fprintf(outputptr, ".data\n");

  fprintf(outputptr, ".text\n");
  fprintf(outputptr, ".globl main\n");
  fprintf(outputptr, "main:\n");











  
  fprintf(outputptr, "li $v0, 10\n");
  fprintf(outputptr, "syscall\n");



  



}


