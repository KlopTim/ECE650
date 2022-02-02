#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "my_malloc.h"
Node * head = NULL;
__thread Node * head_v2 = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;



/* ----------------------------- version1 begin ------------------------*/


void *all_malloc(size_t size) {
  if (head == NULL) {
    Node* cur = expand_block(size);
    return (void*)((char*)cur + sizeof(Node));
  }

  /* find a free block */
  Node* cur = bf_get_free(size);

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


void *ts_malloc_lock(size_t size) {
  pthread_mutex_lock(&lock);
  void * p = all_malloc(size);
  pthread_mutex_unlock(&lock);
  return p;
}
void ts_free_lock(void *ptr) {
  pthread_mutex_lock(&lock);
  all_free(ptr);
  pthread_mutex_unlock(&lock);
}

/* ----------------------------- version1 end ------------------------*/







/* ----------------------------- version2 begin ------------------------*/
void *all_malloc_v2(size_t size) {
  if (head_v2 == NULL) {
    Node* cur = expand_block(size);
    return (void*)((char*)cur + sizeof(Node));
  }

  /* find a free block */
  Node* cur = bf_get_free(size);

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


void all_free_v2(void* ptr) {
  char* p = (char*)ptr - sizeof(Node);
  Node * cur = (Node *)p;
  //if (cur->isFree == 1) return;
  //cur->isFree = 1;
  /* Insert current block into the free list */
  Insert_free(cur);
  Merge_free(cur);
  //print_free_list();
}

void Insert_free_v2(Node* node) {
  if (head_v2 == NULL) {
    head_v2 = node;
    node->prev = NULL;
    node->next = NULL;
    return;
  }

  if ((char*)node < (char*)head_v2) {
    node->next = head_v2;
    head_v2->prev = node;
    node->prev = NULL;
    head_v2 = node;
    return;
  }

  Node* prev = head_v2;
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




void Merge_free_v2(Node* cur) {
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


void Merge_two_free_v2(Node* node1, Node* node2) {
  node1->size += sizeof(Node) + node2->size;
  node1->next = node2->next;
  if (node2->next != NULL)
    node2->next->prev = node1;
}

Node * expand_block_v2(size_t size) {
  pthread_mutex_lock(&lock);
  void * p = sbrk(0);
  sbrk(sizeof(Node) + size);
  pthread_mutex_unlock(&lock);
  Node * cur = (Node*)p;
  set_default(cur, size);
  return cur;
}

void set_default_v2(Node* cur, size_t size) {
  cur->size = size;
  //cur->isFree = 0;
  cur->prev = NULL;
  cur->next = NULL;
}

void remove_block_v2(Node* cur) {
  if (cur->prev == NULL && cur->next == NULL) {
    head_v2 = NULL;
  }
  else if (cur->prev == NULL && cur->next != NULL) {
    cur->next->prev = NULL;
    head_v2 = cur->next;
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

Node* bf_get_free_v2(size_t size) {
  Node* cur = head_v2;
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



void *ts_malloc_nolock(size_t size) {
  return all_malloc_v2(size);
}
void ts_free_nolock(void *ptr) {
  all_free_v2(ptr);
}

/* ----------------------------- version2 end ------------------------*/




