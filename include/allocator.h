#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ALIGN_UP(size) (((size) + 7) & ~0x7)
#define PREV_INUSE(block) (((block_t *) (block))->prev_size & 1)
#define INUSE(block) (((block_t *) (block))->size & 1)


typedef struct block {
    size_t size;
    size_t prev_size;
    struct block *l, *r;
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