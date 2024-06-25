#include "assembleDefs.h"
#include "hashTable.h"

unsigned int hashFunction(const char* str, int capacity) {
	// sum up chars
	int len = strlen(str);
	int total = 0;

	for (int i = 0; i < len; i++) {
		total += str[i];
	}

	return (total + len) % capacity;
}

HashTable* createHashTable(unsigned int capacity) {
	// allocate new symbol table on heap
	HashTable* newST = malloc(sizeof(HashTable));
	newST->capacity = capacity;

	// allocate dynamic array of pointers as roots of linked lists
	newST->arr = malloc(capacity * sizeof(EntryNode));
	return newST;
}

void* getEntryHashTable(HashTable* st, const char* key) {
	// compute hash value as index of key
	unsigned int hashValue = hashFunction(key, st->capacity);
	printf("Hash value is: %d\n", hashValue);

	// get value
	if (st->arr[hashValue]) {
		// follow linked list to end
		EntryNode current = st->arr[hashValue];

		while (current != NULL) {
			printf("current key value is (%s, %p), searching for %s \n", current->entry.key, current->entry.value, key);
			if (strcmp(current->entry.key, key) == 0) {
				// found
				printf("Found\n");
				return &(current->entry.value);
			}

			// move along linked list
			current = current->next;
		}
	}
	return NULL;
}

// check if key is in hash
bool isInHashTable(HashTable* ht, const char* key) {
	// iterate through all cells
	return getEntryHashTable(ht, key) != NULL;
}

// insert key value pair into st
bool insertHashTable(HashTable* st, char* key, void* value) {
	// compute hash value as index of key
	unsigned int hashValue = hashFunction(key, st->capacity);

	printf("%s hashes to %d\n", key, hashValue);

	// get value
	if (st->arr[hashValue]) {
		// follow linked list to end
		EntryNode current = st->arr[hashValue];
		EntryNode prev = current;

		while (current != NULL) {
			if (strcmp(current->entry.key, key) == 0) {
				// change value
				current->entry.value = value;
				
				// already in list
				return false;
			}

			// move along linked list
			prev = current;
			current = current->next;
		}

		// add new node
		EntryNode newNode = malloc(sizeof(struct EntryNode));
		strcpy(newNode->entry.key, key);
		newNode->entry.value = value;
		prev->next = newNode;
	} else {
		// add new node
		st->arr[hashValue] = malloc(sizeof(struct EntryNode));
		strcpy(st->arr[hashValue]->entry.key, key);
		st->arr[hashValue]->entry.value = value;
	}

	return true;
}

// remove an entry, identified by its key, from the table
bool removeHashTable(HashTable* st, char* key) {
	// compute hash value as index of key
	unsigned int hashValue = hashFunction(key, st->capacity);

	// get value
	if (st->arr[hashValue]) {
		// need to handle edge case where first element is to be removed

		// follow linked list to end
		EntryNode current = st->arr[hashValue];
		EntryNode prev = current;

		bool isFirst = true;

		while (current->next != NULL) {
			if (strcmp(current->entry.key, key) == 0) {
				// found - remove item
				if (isFirst) {
					st->arr[hashValue] = current->next;
				} else {
					prev->next = current->next;
				}

				// free current node
				free(current);
				return true;
			}

			// move along linked list
			prev = current;
			current = current->next;

			isFirst = false;
		}
	}
	return false;
}

// free whole symbol table
void freeHashTable(HashTable* st) {
	// remove all keys
	for (unsigned int i = 0; i < st->capacity; i++) {
		printf("i = %d\n", i);
		if (st->arr[i] != NULL) {
			// free all nodes
			EntryNode current = st->arr[i];

			while (current != NULL) {
				// remove node
				removeHashTable(st, current->entry.key);
				current = current->next;
			}

			free(st->arr[i]);
			st->arr[i] = NULL;
		}
	}
	printf("Finished freeing hash table\n");
}
