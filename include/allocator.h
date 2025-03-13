#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ALIGN_UP(size) (((size) + 7) & ~0x7)
#define PREV_INUSE(block) (((block_t *) (block))->prev_size & 1)
#define INUSE(block) (((block_t *) (block))->size & 1)

#define CLEAR_USE_BIT(size) ((size) & ~0x1)

#define NEXT_BLOCK(block) \
    ((block_t *) ((char *) (block) + CLEAR_USE_BIT(((block_t *) (block))->size)))
#define PREV_BLOCK(block) \
    ((block_t *) ((char *) (block) - CLEAR_USE_BIT(((block_t *) (block))->prev_size)))

#define NEXT_INUSE(block) (INUSE(NEXT_BLOCK(block)))

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - offsetof(type, member)))
#endif

typedef struct block {
    size_t size;
    size_t prev_size;
    struct block *l, *r;
    struct block **pparent;
    char mem[0];
} block_t;

typedef struct allocator {
    size_t size;
    block_t *root;
    char mem[0];
} allocator_t;

/**
 * alloc_create - Create a new allocator with a
 * maximum memory size, which will be aligned up to 8 bytes
 * @size: maximum memory size
 */
allocator_t *alloc_create(size_t size);

/**
 * print_tree - Print the tree in preorder
 * @root: the root node
 */
void print_tree(block_t *root);


/**
 * alloc_alloc - Allocate memory with allocator and size
 * If there is not any invalid block, it will return NULL
 * @allocator: the allocator
 * @size: the size of memory needed
 */
void *alloc_alloc(allocator_t *allocator, size_t size);

/**
 * alloc_free - Free an allocated memory
 * @allocator: the allocator
 * @ptr: pointer to the memory region
 */
void alloc_free(allocator_t *allocator, void *ptr);