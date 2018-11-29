#define MIN 5
#define LEVELS 8
#define PAGE 4096

enum flag {Free, Taken}; 

struct head {
	enum flag status;
	short int level;
	struct head *next; 
	struct head *prev
}; 
// creates a new 4ki byte aligned block
struct head *new() {
	struct head *new = (struct head *) mmap(NULL, PAGE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (new == MAP_FAILED) { 
		retrun NULL; 
	}
	assert(((long int)new & 0xfff) == 0); // 12 last bits should be zero
	new->status = Free; 
	new->level = LEVELS -1;
	return new;
}
//finds buddy of given block by toggeling the bit distingushing the level
struct head *buddy(struct head* block){
	int index = block->level; 
	long int mask = 0x1 << (index + MIN); 
	return (struct head*)((long int)block ^ mask);
}
//returns pinter to block on lower level
struct head *split(struct head *block) {
	int index = block->level - 1; 
	int mask = 0x1 << (index + MIN);
	return (struct head *) ((long int) block | mask);
}
//finds primary block in pair to merge
struct head *primary(struct head* block){
	int index = block->level;
	long int mask = 0xffffffffffffffff << (1 + index + MIN);
	return (struct *head) ((long int) block & mask); 
}
//hides header
void *hide(struct head* block){
	return (void*)(block + 1);
}

struct head *magic(void *memory){
	return ((struct head*)memory - 1);
}

int level(int req){
	int total = req + sizeof(struct head);
	int i = 0; 
	int size = 1 << MIN;
	while(total > size){
		size <<= 1;
		i += 1; 
	}
	return i;
} 
