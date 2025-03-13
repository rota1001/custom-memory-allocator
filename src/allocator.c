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