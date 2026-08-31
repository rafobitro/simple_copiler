#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text_buffer.h"

TextChunk* new_chunk(){
  TextChunk *chunk = malloc(sizeof(TextChunk));
  chunk->used=0;
  chunk->next=NULL;
  return chunk;
}

TextBuffer* init_text_buffer(){
  TextBuffer *buffer= malloc(sizeof(TextBuffer));
  buffer->current=new_chunk();
  buffer->head=buffer->current;
  return buffer;
}

void append_text_buffer(TextBuffer *buffer,char *text){
  int text_size=strlen(text);
  int free_chunk_space=CHUNK_SIZE-buffer->current->used;
  if( text_size<=free_chunk_space){
    memcpy(buffer->current->data+buffer->current->used,text,text_size);
    buffer->current->used+=text_size;
  }
  else{
    memcpy(buffer->current->data+buffer->current->used,text,free_chunk_space);
    buffer->current->used+=free_chunk_space;
    buffer->current->next=new_chunk();
    buffer->current=buffer->current->next;
    append_text_buffer(buffer,text+free_chunk_space);
  }

}



void free_text_buffer(TextBuffer *buffer){
  
  while(buffer->head !=NULL){
    TextChunk* next=buffer->head->next;
    free(buffer->head);
    buffer->head=next;
  }

  free(buffer);

}


void text_buffer_to_file(TextBuffer *buffer, FILE *file){
    TextChunk* head=buffer->head;

   while(head !=NULL){
    fwrite(head->data, 1, head->used, file);
    head=head->next;
  }
}



