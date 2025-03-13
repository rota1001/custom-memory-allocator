#include "allocator.h"


static void insert_node(block_t **root, block_t *block)
{
    while (*root)
        if ((*root)->size < block->size ||
            (((*root)->size == block->size) && ((*root) < block)))
            root = &(*root)->r;
        else
            root = &(*root)->l;

    block->l = block->r = NULL;
    *root = block;
}

void print_tree(block_t *root)
{
    if (!root)
        return;
    print_tree(root->l);
    printf("%ld ", root->size);
    print_tree(root->r);
}

static block_t **find_node(block_t **root, block_t *target)
{
    while (*root) {
        if ((*root)->l &&
            (((*root)->l->size > target->size) ||
             (((*root)->l->size == target->size) && ((*root)->l >= target))))
            root = &(*root)->l;
        else if ((*root)->size > target->size ||
                 (((*root)->size == target->size) && (*root >= target)))
            break;
        else
            root = &(*root)->r;
    }

    if (!(*root) || (*root != target))
        return NULL;

    return root;
}

static block_t **find_node_by_size(block_t **root, size_t size)
{
    while (*root) {
        if ((*root)->l && ((*root)->l->size >= size))
            root = &(*root)->l;
        else if ((*root)->size >= size)
            break;
        else
            root = &(*root)->r;
    }
    if (!(*root))
        return NULL;

    return root;
}


/*
 * Structure representing a free memory block in the memory allocator.
 * The free tree is a binary search tree that organizes free blocks (of type
 * block_t) to efficiently locate a block of appropriate size during memory
 * allocation.
 */
static void remove_node(block_t **node_ptr)
{
    block_t *target = *node_ptr;

    if ((*node_ptr)->l && (*node_ptr)->r) {
        block_t **pred_ptr = &(*node_ptr)->l;

        while ((*pred_ptr)->r)
            pred_ptr = &(*pred_ptr)->r;

        if (*pred_ptr == (*node_ptr)->l) {
            block_t *old_right = (*node_ptr)->r;
            *node_ptr = *pred_ptr;
            (*node_ptr)->r = old_right;
        } else {
            block_t *old_left = (*node_ptr)->l;
            block_t *old_right = (*node_ptr)->r;
            block_t *pred_node = *pred_ptr;
            remove_node(pred_ptr);
            *node_ptr = pred_node;
            (*node_ptr)->l = old_left;
            (*node_ptr)->r = old_right;
        }
    } else if ((*node_ptr)->l || (*node_ptr)->r) {
        block_t *child = ((*node_ptr)->l) ? (*node_ptr)->l : (*node_ptr)->r;
        *node_ptr = child;
    } else
        *node_ptr = NULL;

    target->l = NULL;
    target->r = NULL;
}

allocator_t *alloc_create(size_t size)
{
    size = ALIGN_UP(size);
    allocator_t *allocator = malloc(sizeof(allocator_t) + sizeof(block_t) * 2 + size);
    allocator->size = size;
    allocator->root = NULL;
    block_t *first_block = (block_t *)(allocator + 1);
    block_t *last_block = (block_t *)(first_block->mem + size);
    first_block->size = size;
    first_block->prev_size = 1;
    insert_node(&allocator->root, first_block);
    last_block->size = 1;
}