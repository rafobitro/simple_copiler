#include <stdio.h>
#include <stdlib.h>


int main(){

  FILE  *fptr;

  fptr = fopen("test.s","w");

  fprintf(fptr, ".data\n");

  fprintf(fptr, ".text\n");
  fprintf(fptr, ".globl main\n");
  fprintf(fptr, "main:\n");











  
  fprintf(fptr, "li $v0, 10\n");
  fprintf(fptr, "syscall\n");



  



}


