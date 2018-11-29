void *balloc(size_t size);
void bfree(void *mem);

void test();
struct head *new();
struct head *buddy(struct head* block);
struct head *primary(struct head* block);
struct head *split(struct head *block);
void *hide(struct head* block);
struct head *magic(void *memory);
int level(int req);
struct head *find(int index);
void *balloc(size_t size);
void insert(struct head *block); 
void insert(struct head *block);
void bfree(void *memory);
