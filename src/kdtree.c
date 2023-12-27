#include "kdtree.h"

void print_node(kdtree_t *tree) {
    printf("[%d,%d] %ld\n", tree->x, tree->y, (long)tree->value); 
}

kdtree_t * kdtree_create(int x, int y, char * value) {
    kdtree_t * tree = malloc(sizeof(kdtree_t));
    tree->x = x;
    tree->y = y;
    tree->value = value;
    tree->left = NULL;
    tree->right = NULL;
    return tree;
}

kdtree_t * kdtree_insert_rec(kdtree_t **root, int x, int y, char * value, int depth) {
    if ((*root) == NULL)
        return *root = kdtree_create(x, y, value);

    int e = x < (*root)->x;
    if (depth % 2 == 1) e = y < (*root)->y;

    if (e)
        return kdtree_insert_rec(&(*root)->left, x, y, value, depth+1);
    return kdtree_insert_rec(&(*root)->right, x, y, value, depth+1);
}

kdtree_t *  kdtree_insert(kdtree_t ** root, int x, int y, char * value) {
    return kdtree_insert_rec(root, x, y, value, 0);
}

char * kdtree_search_rec(kdtree_t *root, int x, int y, int depth) {
    if (root == NULL)
        return NULL;

    if (root->x == x && root->y == y)
        return root->value;
    
    unsigned int r, p;
    if (depth % 2 == 0) {
        r = root->x;
        p = x;
    } else {
        r = root->y;
        p = y;
    }

    if (p < r) {
        if (root->left == NULL)
            return NULL;

        return kdtree_search_rec(root->left, x, y, depth+1);
    }

    if (root->right == NULL)
        return NULL;

    return kdtree_search_rec(root->right, x, y, depth+1);
}

char * kdtree_search(kdtree_t *root, int x, int y) {
    return kdtree_search_rec(root, x, y, 0);
}

void kdtree_free(kdtree_t **root) {
    if ((*root)->left != NULL)
        kdtree_free(&(*root)->left);
    
    if ((*root)->right != NULL)
        kdtree_free(&(*root)->right);

    free(*root);
}

void kdtree_walk(kdtree_t *root, void (* consume)(kdtree_t*)) {
    if (root == NULL)
        return;

    kdtree_walk(root->left, consume);
    consume(root);
    kdtree_walk(root->right, consume);
}

int kdtree_size(kdtree_t *root) {
    if (root == NULL)
        return 0;
    
    return 1 + kdtree_size(root->left) + kdtree_size(root->right);
}

int kdtree_fwrite(kdtree_t *root, FILE *file) {
    return fwrite(root, 1, sizeof(kdtree_t), file)
     | kdtree_fwrite(root->left, file)
     | kdtree_fwrite(root->right, file);
}

int kdtree_fread(kdtree_t **root, FILE *file) {
    return fread(*root, 1, sizeof(kdtree_t), file)
        | kdtree_fread(&(*root)->left, file)
        | kdtree_fread(&(*root)->right, file);
}
