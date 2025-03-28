#include "allocator.h"


static void insert_node(block_t **root, block_t *block)
{
    while (*root)
        if ((*root)->size < block->size)
            root = &(*root)->r;
        else
            root = &(*root)->l;

    block->l = block->r = NULL;
    *root = block;
    block->pparent = root;
}

void print_tree(block_t *root)
{
    if (!root)
        return;
    print_tree(root->l);
    printf("%ld ", root->size);
    print_tree(root->r);
}


static block_t **find_node_by_size(block_t **root, size_t size)
{
    block_t **ret = NULL;
    while (*root) {
        if ((*root)->size == size)
            return root;
        else if (((*root)->size >= size)) {
            ret = root;
            root = &(*root)->l;
        } else
            root = &(*root)->r;
    }
    if (!ret)
        return NULL;

    return ret;
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
            (*pred_ptr)->pparent = node_ptr;
            (*node_ptr)->r = old_right;
            old_right->pparent = &(*node_ptr)->r;
        } else {
            block_t *old_left = (*node_ptr)->l;
            block_t *old_right = (*node_ptr)->r;
            block_t *pred_node = *pred_ptr;
            remove_node(pred_ptr);
            *node_ptr = pred_node;
            pred_node->pparent = node_ptr;
            (*node_ptr)->l = old_left;
            (*node_ptr)->r = old_right;
            old_left->pparent = &(*node_ptr)->l;
            old_right->pparent = &(*node_ptr)->r;
        }
    } else if ((*node_ptr)->l || (*node_ptr)->r) {
        block_t *child = ((*node_ptr)->l) ? (*node_ptr)->l : (*node_ptr)->r;
        *node_ptr = child;
        child->pparent = node_ptr;
    } else
        *node_ptr = NULL;

    target->l = NULL;
    target->r = NULL;
}

allocator_t *alloc_create(size_t size)
{
    size = ALIGN_UP(size);
    allocator_t *allocator =
        malloc(sizeof(allocator_t) + sizeof(block_t) * 2 + size);
    allocator->size = size;
    allocator->root = NULL;
    block_t *first_block = (block_t *) (allocator + 1);
    block_t *last_block = (block_t *) (first_block->mem + size);
    first_block->size = size + sizeof(block_t);
    first_block->prev_size = 1;
    insert_node(&allocator->root, first_block);
    last_block->size = 1;
    return allocator;
}

void *alloc_alloc(allocator_t *allocator, size_t size)
{
    size = ALIGN_UP(size);
    block_t **pblock =
        find_node_by_size(&allocator->root, size + sizeof(block_t) * 2);
    if (!pblock)
        return NULL;

    block_t *block = *pblock,
            *left_block = (block_t *) ((char *) block + size + sizeof(block_t));
    remove_node(pblock);
    left_block->size = block->size - size - sizeof(block_t);
    NEXT_BLOCK(left_block)->prev_size = left_block->size;
    block->size = (size + sizeof(block_t)) | 1;
    left_block->prev_size = block->size;
    insert_node(&allocator->root, left_block);

    return block->mem;
}

block_t *merge(block_t *l, block_t *r)
{
    l->size = CLEAR_USE_BIT(l->size) + CLEAR_USE_BIT(r->size);
    return l;
}

void alloc_free(allocator_t *allocator, void *ptr)
{
    block_t *block = container_of(ptr, block_t, mem);
    block_t **pblock;
    if (!PREV_INUSE(block)) {
        pblock = PREV_BLOCK(block)->pparent;
        remove_node(pblock);
        block = merge(PREV_BLOCK(block), block);
    }
    if (!NEXT_INUSE(block)) {
        pblock = NEXT_BLOCK(block)->pparent;
        remove_node(pblock);
        block = merge(block, NEXT_BLOCK(block));
    }
    block->size = CLEAR_USE_BIT(block->size);
    NEXT_BLOCK(block)->prev_size = block->size;
    insert_node(&allocator->root, block);
}