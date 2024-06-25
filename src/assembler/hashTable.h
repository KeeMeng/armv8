#define KEY_SIZE 128

typedef struct {
	char key[KEY_SIZE];
	void* value;
} Entry;

typedef struct EntryNode* EntryNode;
struct EntryNode {
	Entry entry;
	EntryNode next;
};

typedef struct {
	unsigned int capacity;
	EntryNode* arr;
} HashTable;

// define global variables
extern HashTable* symbolTable;
extern char* fileBuffer;

// functions
extern unsigned int hashFunction(const char* str, int capacity);
extern HashTable* createHashTable(unsigned int capacity);
extern void* getEntryHashTable(HashTable* st, const char* key);
extern bool isInHashTable(HashTable* ht, const char* key);
extern bool insertHashTable(HashTable* st, char* key, void* value);
extern bool removeHashTable(HashTable* st, char* key);
extern void printHashTable(HashTable* st);
extern void freeHashTable(HashTable* st);
