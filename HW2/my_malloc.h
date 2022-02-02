#include <stdlib.h>
#include <stdio.h>
typedef struct Node {
  size_t size;
  //int isFree; // 0 indicate not free 1 indicate free
  struct Node * next;
  struct Node * prev;
}Node;

/* for version 1 */
void *all_malloc(size_t size);
void all_free(void* ptr);
void Insert_free(Node* node);
void Merge_free(Node* cur);
void Merge_two_free(Node* node1, Node* node2);
Node * expand_block(size_t size);
void set_default(Node* cur, size_t size);
void remove_block(Node* cur);
Node* bf_get_free(size_t size);

void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);



/* for version 2 */
void *all_malloc_v2(size_t size);
void all_free_v2(void* ptr);
void Insert_free_v2(Node* node);
void Merge_free_v2(Node* cur);
void Merge_two_free_v2(Node* node1, Node* node2);
Node * expand_block_v2(size_t size);
void set_default_v2(Node* cur, size_t size);
void remove_block_v2(Node* cur);
Node* bf_get_free_v2(size_t size);

void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);
