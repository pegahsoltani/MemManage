#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memalloc.h"

typedef struct BlockRecord_t{
  void* address;
  int size;
  struct BlockRecord_t* next;
} BlockRecord_t;


// global variables
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock;
void* chunkPointer;
int chunkSize;
int allocationMethod;
BlockRecord_t recordStart;
BlockRecord_t recordEnd;


int mem_init(void* input_chunkpointer, int input_chunksize, int input_method){

  // initializes the global variables to the parameters
  chunkPointer = input_chunkpointer;
  chunkSize = input_chunksize;
  allocationMethod = input_method;

  // initialize first and last block records
  recordStart.address = chunkPointer;
  recordEnd.address = chunkPointer + chunkSize;
  recordStart.size = 0;
  recordEnd.size = 0;
  recordStart.next = &recordEnd;
  recordEnd.next = NULL;

  return 0;
}


void* memHoleSearch(int size){
  void* availableAddress;

  bool foundSpace = false;
  BlockRecord_t *head;
  head = &recordStart;

  if(allocationMethod == 0){
    printf("Looking for holes in Chunk using FirstFit Method.\n");
    while(head->next != NULL){
      //first fit
      int memHole = head->next->address - (head->address+head->size);
      if((int)size <= (int)memHole){
	availableAddress = head->address+head->size;
	printf("Found an available spot at %p of size %d\n",
	       availableAddress, memHole);
	foundSpace = true;
	break;
      }
      head = head->next;
    }
  }


  if (allocationMethod == 1){
    printf("Looking for holes in Chunk using BestFit Method.\n");
    int bestSpotSize = chunkSize;
    int memHole;
    while (head->next != NULL){
      //best fit
      memHole = head->next->address - (head->address+head->size);
      if((int)size <= (int)memHole &&  bestSpotSize >= (int)memHole){
  	bestSpotSize = (int)memHole;
  	availableAddress = head->address+head->size;
      }
      head = head->next;
    }
    if (availableAddress != NULL){
      printf("Found an available spot at %p of size %d\n",
	     availableAddress, memHole);
      foundSpace = true;
    }
    else foundSpace = false;
  }


  if (allocationMethod == 2){
    printf("Looking for holes in Chunk using WorstFit Method.\n");
    int bestSpotSize = 0;
    int memHole;
    while (head->next != NULL){
      //worst fit
      memHole = head->next->address - (head->address+head->size);
      if((int)size <= (int)memHole &&  bestSpotSize <= (int)memHole){
  	bestSpotSize = (int)memHole;
  	availableAddress = head->address+head->size;
      }
      head = head->next;
    }
    if (availableAddress != NULL){
      printf("Found an available spot at %p of size %d\n",
	     availableAddress, memHole);
      foundSpace = true;
    }
    else foundSpace = false;
  }


  if (foundSpace) return availableAddress;
  else { printf("No available space (memHole) found.\n");return NULL;}
}


BlockRecord_t* traverseRecordsByAddr_le(void* checkAddr){
  BlockRecord_t *head, *p_head;
  p_head = &recordStart;
  head = p_head->next;
  while(head->next != NULL){
    /* printf("Checking block %p\n", head); */
    if (head->address >= checkAddr){
      /* printf("The previous block was the answer.\n"); */
      break;
    }
    p_head = p_head->next;
    head = p_head->next;
  }
  return p_head;
}


BlockRecord_t* traverseRecordsByAddr_eq(void* checkAddr){
  BlockRecord_t *head, *p_head;
  p_head = &recordStart;
  head = p_head->next;
  bool foundBlockRecord = false;
  while(head->next != NULL){
    /* printf("Checking block %p\n", head); */
    if (head->address == checkAddr){
      printf("Deleting Block Record at %p.\n", checkAddr);
      foundBlockRecord = true;
      break;
    }
    p_head = p_head->next;
    head = p_head->next;
  }
  if (foundBlockRecord) return p_head;
  else{
    printf("No Block record found at %p.\n", checkAddr);
    return NULL;
  }
}


BlockRecord_t* insertBlockRecord(void* availAddr, int size){
  BlockRecord_t* newBlockRecord;
  newBlockRecord->address = availAddr;
  newBlockRecord->size = size;

  BlockRecord_t* cursorBlock = traverseRecordsByAddr_le(availAddr);

  newBlockRecord->next = cursorBlock->next;
  cursorBlock->next = newBlockRecord;

  return newBlockRecord;
}

void* mem_allocate(int size){
  // find hole
  pthread_mutex_lock(&lock);//Locked
  printf("Looking for holes of size %d and greater.\n", size);
  void* availAddr =  memHoleSearch(size);

  // update block records

  if (availAddr !=NULL) {
    BlockRecord_t* newBlockRecord = insertBlockRecord(availAddr, size);
    printf("Created BlockRecord for block %p of size %d.\n", availAddr, size);
    return newBlockRecord->address;
  }
  else {
    printf("Memory allocation failed.\n");
    return NULL;
  }
  pthread_mutex_unlock(&lock);

}

void mem_free(void* memAddr){
    pthread_mutex_lock(&lock);//Locked
  BlockRecord_t *cursorBlock= traverseRecordsByAddr_eq(memAddr);
  if (cursorBlock != NULL){
    BlockRecord_t *blockToDelete = cursorBlock->next;
    cursorBlock->next = blockToDelete->next;

    printf("Memory at %p is freed up.\n", blockToDelete->address);
    blockToDelete = NULL;
  }
  else{
    printf("Block record deletion failed.\n");
  }
    pthread_mutex_unlock(&lock);

}

void mem_print(){
  struct BlockRecord_t *head, *p_head;
  p_head = &recordStart;
  head = p_head->next;

  printf("\n*************************\n****  Block Records  ****\n*************************\n");

  while(head->next != NULL){
    printf("Block Record: Size of %d Bytes, at %p.\n",
	   head->size, head->address);
    p_head = p_head->next;
    head = p_head->next;
  }

  printf("********** END **********\n\n");
}
