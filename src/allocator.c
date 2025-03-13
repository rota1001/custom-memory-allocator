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
