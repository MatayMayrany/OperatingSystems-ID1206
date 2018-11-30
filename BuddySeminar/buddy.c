#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>
#include <errno.h>


#define MIN  5     
#define LEVELS 8   
#define PAGE 4096

enum flag {Free, Taken};


struct head {
  enum flag status;
  short int level;
  struct head *next;
  struct head *prev;  
};


/* The free lists */

struct head *flists[LEVELS] = {NULL};


/* These are the low level bit fidling operations */
  
struct head *new() {
  struct head *new = mmap(NULL, PAGE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if(new == MAP_FAILED) {
    printf("mmap failed: error %d\n", errno);
    return NULL;
  }
  assert(((long int)new & 0xfff) == 0);  // 12 last bits should be zero 
  new->status = Free;
  new->level = LEVELS -1;
  return new;
}


struct head *buddy(struct head* block) {
  int index = block->level;
  long int mask =  0x1 << (index + MIN);
  return (struct head*)((long int)block ^ mask);
}

struct head *primary(struct head* block) {
  int index = block->level;
  long int mask =  0xffffffffffffffff << (1 + index + MIN);
  return (struct head*)((long int)block & mask);
}

struct head *split(struct head *block) {
  int index = block->level - 1;
  int mask =  0x1 << (index + MIN);
  return (struct head *)((long int)block | mask );
}


void *hide(struct head* block) {
  return (void*)(block + 1);
}

struct head *magic(void *memory) {
  return (struct head*)(((struct head*)memory) - 1);
}

int level(int req) {
  int total = req  + sizeof(struct head);

  int i = 0;
  int size = 1 << MIN;
  while(total > size) {
    size <<= 1;
    i += 1;
  }
  return i;
}

struct head *find(int index) {
	struct head *block;
	if (flists[LEVELS-1] == NULL ) {
		//printf("Created new 4KI block \n");
		flists[LEVELS-1] = new();
		
	}
	for (int i = 1; i <= LEVELS-1; i++){
		if (flists[index] != NULL) {
			//printf("Requested block is found \n");
	 		block = flists[index];
			if ((block->next) != NULL) {
				block->next->prev = NULL;
	 			flists[index] = block-> next;
			} else {
	  			flists[index] = NULL;
			}
			block->status = Taken;
			block->next = NULL;
			block->prev = NULL;
			return block;
		} else if (flists[index + i] != NULL) {
			block = flists[index + i];
			struct head *block2 = split(block);
			if (block->next) {
	 		block->next->prev = NULL;
	 	 	flists[index + i] = block->next;
			} else {
	 			flists[index + i] = NULL;
			}
		short int level = block->level - 1;
		block->level = level;
		block->status = Free;
		block->next = block2;
		block->prev = NULL;
		block2->level = level;
		block2->status = Free;
		block2->next = NULL;
		block2-> prev = block;
		flists[index + i - 1] = block;
		return find(index);
		}
		else if (index == LEVELS-1 ){
			//printf("New block! \n");
			flists[LEVELS-1] = new();
			return find(index);
		}
	}
	find(index);
}



void *balloc(size_t size) {
  if( size == 0 ){
    return NULL;
  }
  int index = level(size);
  struct head *taken = find(index);
  return hide(taken);
}


void insert(struct head *block) {
  block->status = 0;
  if (block->level < LEVELS-1) {
  struct head *buddyBlock = buddy(block);
 
    if (buddyBlock->status == 0 && (buddyBlock->level == block->level)) {//IF there exists such a buddy
 
      int tempLevel = block->level;
      struct head *mergedBlock = primary(block);
      mergedBlock->level = tempLevel + 1;
      mergedBlock->status = 0;
      mergedBlock->prev = NULL;
      mergedBlock->next = NULL;
      flists[tempLevel] = NULL;
      insert(mergedBlock);
    }else{
      //Then we cant do anything other than putting it in the list
      block->next = NULL;
      block->prev = NULL;
      flists[block->level] = block;
    }
  }else{
    flists[LEVELS-1] = block;
  }
}

void bfree(void *memory) {

  if(memory != NULL) {
    struct head *block = magic(memory);
    insert(block);
  }
  return;
}

void test(){
	struct head *testBlock = new();
	struct head *hidden = hide(testBlock);
	printf("size of head is: %ld\n", sizeof(struct head));
	balloc(3000); 
	printf("Level of block given for 7 bytes should be 0 : %d\n", level(7));
        printf("Level of block given for 9 bytes should be 1 : %d\n", level(9));
        printf("Level of block given for 40 bytes should be 1 : %d\n", level(40));
        printf("Level of block given for 41 bytes should be 2 : %d\n", level(41));
        printf("address of original block: %p should  address after hide (%p) and magic: %p\n", testBlock, hidden,(magic(hidden)));
        printf("spliting testBlock: %p gives: %p\n", testBlock, split(testBlock));
	printf("spliting the split: %p gives: %p\n", split(testBlock), split(split(testBlock)));
	//printf("buddy of the split: %p is: %p\n", split(testBlock), buddy(split(testBlock)));
        printf("getting primary of double splitBlock: %p gives: %p\n", split(split(testBlock)), primary(split(split(testBlock))));
}

