#include <stdlib.h>
#include <stdio.h>
typedef struct Node {
  size_t size;
  //int isFree; // 0 indicate not free 1 indicate free
  struct Node * next;
  struct Node * prev;
}Node;


void *all_malloc(size_t size, char c);
void all_free(void* ptr);
void print_free_list();
void Insert_free(Node* node);
Node* Insert_free_helper(Node* node, Node * cur);
void Merge_free(Node* cur);
void Merge_two_free(Node* node1, Node* node2);
Node * expand_block(size_t size);
void set_default(Node* cur, size_t size);
void remove_block(Node* cur);
Node* ff_get_free(size_t size);
Node* bf_get_free(size_t size);
void *ff_malloc(size_t size);
void ff_free(void *ptr);
void *bf_malloc(size_t size);
void bf_free(void *ptr);

unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();