#include <sys/types.h>
#include <unistd.h>
#include "my_malloc.h"
Node * head = NULL;
void * begin = NULL;


void *all_malloc(size_t size, char c) {
  if (begin == NULL) begin = sbrk(0);
  if (head == NULL) {
    Node* cur = expand_block(size);
    return (void*)cur + sizeof(Node);
  }

  /* find a free block */
  Node* cur = c == 'f' ? ff_get_free(size) : bf_get_free(size);

  /*  cannot find a free block */
  if (cur == NULL) {
    Node* cur = expand_block(size);
    return (void*)cur + sizeof(Node);
  }

  /* else we find such a free block */
  // if the block is not big enough to split
  if (cur->size < size + sizeof(Node) + 4) {
    cur->isFree = 0;
    remove_block(cur);
    return sizeof(Node) + (void*)cur;
  }
  // else the block is big enough
  Node* newNode =(Node*)((void*)cur + sizeof(Node) + (cur->size - sizeof(Node) - size));
  cur->size -= sizeof(Node) + size;
  set_default(newNode, size);
  return (void*)newNode + sizeof(Node);
}


void all_free(void* ptr) {
  ptr -= sizeof(Node);
  Node * cur = (Node *)ptr;
  if (cur->isFree == 1) return;
  cur->isFree = 1;
  /* Insert current block into the free list */
  Insert_free(cur);
  Merge_free(cur);
  //print_free_list();
}

void print_free_list() {
  Node* cur = head;
  Node* tail;
  while (cur != NULL) {
    printf("Free:%d, size:%ld\n", cur->isFree, cur->size);
    if (cur->next == NULL) tail = cur;
    cur = cur->next;
  }
  /*
  printf("reverse:\n");
  while (tail != NULL) {
    printf("Free:%d, size:%ld\n", tail->isFree, tail->size);
    tail = tail->prev;
  }
  */
  printf("\n\n");
}

void Insert_free(Node* node) {
  head = Insert_free_helper(node, head);
}

Node* Insert_free_helper(Node* node, Node * cur) {
  if (cur == NULL) {
    node->prev = NULL;
    node->next = NULL;
    return node;
  }
  if ((void*)node < (void*)cur) {
    node->prev = NULL;
    node->next = cur;
    cur->prev = node;
    return node;
  }
  cur->next = Insert_free_helper(node, cur->next);
  cur->next->prev = cur;
  return cur;
}

void Merge_free(Node* cur) {
  void* p =  (void*)cur + sizeof(Node) + cur->size;
  if (cur->next != NULL && p == (void*)cur->next) {
    Merge_two_free(cur, cur->next);
  }
  Node* prev = cur->prev;
  if (prev != NULL) {
    p = (void*)prev + sizeof(Node) + prev->size;
    if (p == (void*)cur) {
      Merge_two_free(prev, cur);
    }
  }
}


void Merge_two_free(Node* node1, Node* node2) {
  node1->size += sizeof(Node) + node2->size;
  node1->next = node2->next;
  if (node2->next != NULL)
    node2->next->prev = node1;
}

Node * expand_block(size_t size) {
  void * p = sbrk(0);
  sbrk(sizeof(Node) + size);
  Node * cur = (Node*)p;
  set_default(cur, size);
  return cur;
}

void set_default(Node* cur, size_t size) {
  cur->size = size;
  cur->isFree = 0;
  cur->prev = NULL;
  cur->next = NULL;
}

void remove_block(Node* cur) {
  if (cur->prev == NULL && cur->next == NULL) {
    head = cur->next;
  }
  else if (cur->prev == NULL && cur->next != NULL) {
    cur->next->prev = cur->prev;
    head = cur->next;
    cur->next = NULL;
  }
  else if (cur->prev != NULL && cur->next == NULL) {
    cur->prev->next = cur->next;
    cur->prev = NULL;
  }
  else {
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;
    cur->prev = NULL;
    cur->next = NULL;
  }
}
Node* ff_get_free(size_t size) {
  Node* cur = head;
  while (cur != NULL) {
    if (cur->size >= size) break;
    cur = cur->next;
  }
  return cur;
}
Node* bf_get_free(size_t size) {
  Node* cur = head;
  Node* min_node = NULL;
  size_t min_size = 0;
  while (cur != NULL) {
    if (cur->size >= size) {
      if (cur->size < min_size) {
        min_node = cur;
        min_size = cur->size;
      }
    }
    cur = cur->next;
  }
  return min_node;
}



void *ff_malloc(size_t size) {
  return all_malloc(size, 'f');
}

void ff_free(void* ptr) {all_free(ptr);}

void *bf_malloc(size_t size) {
  return all_malloc(size, 'b');
}

void bf_free(void* ptr) {all_free(ptr);}

unsigned long get_data_segment_size() {
  return sbrk(0) - (void*)begin;
}
unsigned long get_data_segment_free_space_size() {
  long sum = 0;
  Node* cur = head;
  while (cur != NULL) {
    sum += sizeof(Node) + cur->size;
    cur = cur->next;
  }
  return sum;
}




