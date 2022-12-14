#include "mavalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

enum TYPE
{
  FREE = 0,
  USED
};

struct Node
{
  size_t size;
  enum TYPE type;
  void *arena;
  struct Node *next;
  struct Node *prev;
};

struct Node *alloc_list;
struct Node *previous_node;

void *arena;

enum ALGORITHM allocation_algorithm = FIRST_FIT;

int mavalloc_init(size_t size, enum ALGORITHM algorithm)
{
  if (size < 0)
  {
    return -1;
  }
  arena = malloc(ALIGN4(size));
  if (arena == NULL)
  {
    return -1;
  }
  allocation_algorithm = algorithm;

  alloc_list = (struct Node *)malloc(sizeof(struct Node));

  alloc_list->arena = arena;
  alloc_list->size = ALIGN4(size);
  alloc_list->type = FREE;
  alloc_list->next = NULL;
  alloc_list->prev = NULL;

  previous_node = alloc_list;

  return 0;
}

void mavalloc_destroy()
{
  free(arena);
  // iterate over the linked list and free the nodes
  struct Node *node = alloc_list;
  struct Node *prev = node;
  while (node)
  {
    node = node->next;
    free(prev);
    prev = node;
  }
  alloc_list = NULL;
  return;
}

void *mavalloc_alloc(size_t size)
{
  struct Node *node;

  // Only next fit wont start from the top
  if (allocation_algorithm != NEXT_FIT)
  {
    node = alloc_list;
  }
  else if (allocation_algorithm == NEXT_FIT)
  {
    node = previous_node;
  }
  else
  {
    printf("ERROR: Unknown allocation algorithm!\n");
    exit(0);
  }

  size_t aligned_size = ALIGN4(size);

  if (allocation_algorithm == FIRST_FIT)
  {
    while (node)
    {
      if (node->size >= aligned_size && node->type == FREE)
      {
        int leftover_size = 0;

        node->type = USED;
        leftover_size = node->size - aligned_size;
        node->size = aligned_size;

        if (leftover_size > 0)
        {
          struct Node *previous_next = node->next;
          struct Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

          leftover_node->arena = node->arena + size;
          leftover_node->type = FREE;
          leftover_node->size = leftover_size;
          leftover_node->next = previous_next;

          node->next = leftover_node;
        }
        previous_node = node;
        return (void *)node->arena;
      }
      node = node->next;
    }
  }
  else if (allocation_algorithm == NEXT_FIT)
  {
    struct Node *begin_node = node;
    while (node)
    {
      if (node->size >= aligned_size && node->type == FREE)
      {
        int leftover_size = 0;

        node->type = USED;
        leftover_size = node->size - aligned_size;
        node->size = aligned_size;

        if (leftover_size > 0)
        {
          struct Node *previous_next = node->next;
          struct Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

          leftover_node->arena = node->arena + size;
          leftover_node->type = FREE;
          leftover_node->size = leftover_size;
          leftover_node->next = previous_next;

          node->next = leftover_node;
        }
        previous_node = node;
        return (void *)node->arena;
      }
      node = node->next;
    }
    node = alloc_list;
    while (node)
    {
      if (node == begin_node)
      {
        printf("Nodes are all full");
        return (void *)node->arena;
      }
      if (node->size >= aligned_size && node->type == FREE)
      {
        int leftover_size = 0;

        node->type = USED;
        leftover_size = node->size - aligned_size;
        node->size = aligned_size;

        if (leftover_size > 0)
        {
          struct Node *previous_next = node->next;
          struct Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

          leftover_node->arena = node->arena + size;
          leftover_node->type = FREE;
          leftover_node->size = leftover_size;
          leftover_node->next = previous_next;

          node->next = leftover_node;
        }
        previous_node = node;
        return (void *)node->arena;
      }
      node = node->next;
    }
  }

  // Implement Worst Fit
  if (allocation_algorithm == WORST_FIT)
  {
    struct Node *max_node;
    while (node->type == USED)
    {
      node = node->next;
    }
    max_node = node;
    while (node->next != NULL)
    {
      node = node->next;
      if (node->type == FREE)
      {
        if (max_node->size < node->size)
        {
          max_node = node;
        }
      }
    }
    node = max_node;

    if (node->size >= aligned_size && node->type == FREE)
    {
      int leftover_size = 0;

      node->type = USED;
      leftover_size = node->size - aligned_size;
      node->size = aligned_size;

      if (leftover_size > 0)
      {
        struct Node *previous_next = node->next;
        struct Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

        leftover_node->arena = node->arena + size;
        leftover_node->type = FREE;
        leftover_node->size = leftover_size;
        leftover_node->next = previous_next;

        node->next = leftover_node;
      }
      previous_node = node;
      return (void *)node->arena;
    }
    node = node->next;
  }

  // Implement Best Fit
  if (allocation_algorithm == BEST_FIT)
  {
    struct Node *winner = NULL;
    int winning_size = INT_MAX;

    while (node)
    {

      if (node->type == FREE && node->size >= aligned_size)
      {
        int diff = (node->size - size);
        if (diff < winning_size)
        {
          winner = node;
          winning_size = diff;
        }
      }
      node = node->next;
    }

    if (winner)
    {
      int leftover_size = 0;

      winner->type = USED;
      leftover_size = winner->size - aligned_size;
      winner->size = aligned_size;

      if (leftover_size > 0)
      {
        struct Node *previous_next = winner->next;
        struct Node *leftover_node = (struct Node *)malloc(sizeof(struct Node));

        leftover_node->arena = winner->arena + size;
        leftover_node->type = FREE;
        leftover_node->size = leftover_size;
        leftover_node->next = previous_next;

        winner->next = leftover_node;
      }
      previous_node = winner;
      return (void *)winner->arena;
    }
  }

  return NULL;
}

void mavalloc_free(void *ptr)
{
  // printf("Trevor Bakker singlehandedly broke my wrist");
  struct Node *node = alloc_list;
  while (node)
  {
    if (node->arena == ptr)
    {
      if (node->type == FREE)
      {
        printf("Warning: Double free detected");
      }
      node->type = FREE;

      break;
    }
    node = node->next;
  }
  node = alloc_list;
  while (node){
    if( node -> type == FREE && node->next && node->next -> type == FREE )
    {
      struct Node * node_to_free = node->next;
      node->size = node->size + node->next->size;
      node->next = node->next->next;
      free( node_to_free);
      continue;
    }
    node = node->next;
  }

  return;
}

int mavalloc_size()
{
  int number_of_nodes = 0;
  struct Node *ptr = alloc_list;

  while (ptr)
  {
    number_of_nodes++;
    ptr = ptr->next;
  }

  return number_of_nodes;
}
