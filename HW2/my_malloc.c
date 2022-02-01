#include <sys/types.h>
#include <unistd.h>
#include "my_malloc.h"
Node * head = NULL;
void * begin = NULL;


void *all_malloc(size_t size, char c) {
  if (begin == NULL) begin = sbrk(0);
  if (head == NULL) {
    Node* cur = expand_block(size);
    return (void*)((char*)cur + sizeof(Node));
  }

  /* find a free block */
  Node* cur = c == 'f' ? ff_get_free(size) : bf_get_free(size);

  /*  cannot find a free block */
  if (cur == NULL) {
    Node* cur = expand_block(size);
    return (void*)((char*)cur + sizeof(Node));
  }

  /* else we find such a free block */
  // if the block is not big enough to split
  if (cur->size < size + sizeof(Node)) {
    //cur->isFree = 0;
    remove_block(cur);
    return (void*)(sizeof(Node) + (char*)cur);
  }
  // else the block is big enough
  Node* newNode =(Node*)((char*)cur + sizeof(Node) + (cur->size - sizeof(Node) - size));
  cur->size -= sizeof(Node) + size;
  set_default(newNode, size);
  return (void*)((char*)newNode + sizeof(Node));
}


void all_free(void* ptr) {
  char* p = (char*)ptr - sizeof(Node);
  Node * cur = (Node *)p;
  //if (cur->isFree == 1) return;
  //cur->isFree = 1;
  /* Insert current block into the free list */
  Insert_free(cur);
  Merge_free(cur);
  //print_free_list();
}

void Insert_free(Node* node) {
  if (head == NULL) {
    head = node;
    node->prev = NULL;
    node->next = NULL;
    return;
  }

  if ((char*)node < (char*)head) {
    node->next = head;
    head->prev = node;
    node->prev = NULL;
    head = node;
    return;
  }

  Node* prev = head;
  while (prev->next != NULL) {
    if ((char*)node < (char*)(prev->next)) {
      break;
    }
    prev = prev->next;
  }
  if (prev->next == NULL) {
    prev->next = node;
    node->prev = prev;
    node->next = NULL;
  }
  else {
    node->prev = prev;
    node->next = prev->next;
    node->prev->next = node;
    node->next->prev = node;
  }

}


/*
void Insert_free(Node* node) {
  head = Insert_free_helper(node, head);
}

Node* Insert_free_helper(Node* node, Node * cur) {
  if (cur == NULL) {
    node->prev = NULL;
    node->next = NULL;
    return node;
  }
  if ((char*)node < (char*)cur) {
    node->prev = NULL;
    node->next = cur;
    cur->prev = node;
    return node;
  }
  cur->next = Insert_free_helper(node, cur->next);
  cur->next->prev = cur;
  return cur;
}
*/



void Merge_free(Node* cur) {
  char* p =  (char*)cur + sizeof(Node) + cur->size;
  if (cur->next != NULL && p == (char*)cur->next) {
    Merge_two_free(cur, cur->next);
  }
  Node* prev = cur->prev;
  if (prev != NULL) {
    p = (char*)prev + sizeof(Node) + prev->size;
    if (p == (char*)cur) {
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
  //cur->isFree = 0;
  cur->prev = NULL;
  cur->next = NULL;
}

void remove_block(Node* cur) {
  if (cur->prev == NULL && cur->next == NULL) {
    head = NULL;
  }
  else if (cur->prev == NULL && cur->next != NULL) {
    cur->next->prev = NULL;
    head = cur->next;
    cur->next = NULL;
  }
  else if (cur->prev != NULL && cur->next == NULL) {
    cur->prev->next = NULL;
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
  while (cur != NULL) {
    if (cur->size >= size) {
      if (min_node == NULL || cur->size < min_node->size) {
        min_node = cur;
        if (min_node->size == size) break;
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




