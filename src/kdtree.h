#include <stdio.h>
#include <stdlib.h>

typedef struct KDTree {
    unsigned int x;
    unsigned int y;
    char    *value;

    struct KDTree *left;
    struct KDTree *right;
} kdtree_t;

//! insert an element into a kdtree
kdtree_t *  kdtree_insert(kdtree_t ** root, int x, int y, char * value);

//! return a value from a kdtree, NULL if not present
char * kdtree_search(kdtree_t *root, int x, int y);

//! free memory for a kdtree
void kdtree_free(kdtree_t **root);

//! in order walk of nodes in kdtree
void kdtree_walk(kdtree_t *root, void (* consume)(kdtree_t*));

//! get number of nodes in a kdtree
int kdtree_size(kdtree_t *root);

//! write tree to a file
int kdtree_fwrite(kdtree_t *root, FILE *file);

//! read a tree from file
int kdtree_fread(kdtree_t **root, FILE *file);

