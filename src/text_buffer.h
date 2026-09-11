#pragma once
#define CHUNK_SIZE  4096

typedef struct TextChunk{
  char data[CHUNK_SIZE];
  int used;
  struct TextChunk *next;
}TextChunk;

typedef struct{
  TextChunk *current;
  TextChunk *head;
}TextBuffer;


TextChunk* new_chunk(void);
TextBuffer* init_text_buffer(void);

void append_text_buffer(TextBuffer *buffer,char *text);
void free_text_buffer(TextBuffer *buffer);
void text_buffer_to_file(TextBuffer *buffer, FILE *file);

