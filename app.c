#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "memalloc.h"

int main(int argc, char* argv[]){
  int size = atoi(argv[1]);
  int method = atoi(argv[2]);
  int initial_chunkSize = 1024 * size;
  void* initial_chunkPointer = malloc(initial_chunkSize);
  printf("A %d KB memory chunk is created at %p ending at %p\n", size
	 , initial_chunkPointer,
	 (void*)((char*)initial_chunkPointer + initial_chunkSize));

  mem_init(initial_chunkPointer, initial_chunkSize, method);

  void* testAddr1 = mem_allocate(512*1024);
  void* testAddr2 = mem_allocate(256*1024);
  void* testAddr3 = mem_allocate(128*1024);
  void* testAddr4 = mem_allocate(64*1024);
  void* testAddr5 = mem_allocate(32*1024);
  mem_print();
  mem_free(testAddr2);
  mem_free(testAddr4);
  mem_print();
  void* testAddr6 = mem_allocate(32*1024);
  void* testAddr7 = mem_allocate(64*1024);
  mem_print();
  return 0;
}
