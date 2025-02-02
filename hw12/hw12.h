#ifndef HW12_H
#define HW12_H

/*
 * Homework 12 header file
 * CS 240, Fall 2024
 * Purdue University
 */

#define CHUNKSIZE (4)

/* Generic node structure */

typedef struct node {
  struct node *left_child;
  struct node *right_child;
  void *data;
  void (*print)(void *);
  void (*delete)(void **);
  float (*distance)(void *);
} node_t;


/* Chunk structure */

typedef struct chunk {
  int chunk_x;
  int chunk_y;
  int chunk_z;
  unsigned char blocks[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE];
} chunk_t;

/* Entity structure */

typedef struct entity {
  float  x;
  float  y;
  float  z;
  char  *name;
  int    id;
  int    type;
  float  health_pct;
} entity_t;

/*
 * Functions of the tree
 */

void print_tree(node_t *);
void delete_tree(node_t **);

/*
 * Functions of the tree nodes
 */

void print_node(node_t *);
void create_node(node_t **, void *, void (*)(void*),
                 void (*)(void **), float (*)(void *));
void delete_node(node_t **);
int compare_nodes(node_t *, node_t *);
void insert_node(node_t **, node_t *);
void remove_node(node_t **, node_t *);
node_t **find_nodes(node_t *, float, float, int *);

/*
 * Functions for accessing/manipulating node data
 */

/* data access/manipulation functions for chunk structure */

void  print_chunk_data(void *);
void *create_chunk_data(int x, int y, int z,
                        unsigned char blocks[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE]);
void  delete_chunk_data(void **);
float calculate_chunk_distance(void *);

/* data access/manipulation functions for entity structure */

void  print_entity_data(void *);
void *create_entity_data(float x, float y, float z, const char *name, int id,
                         int type, float health_pct);
void  delete_entity_data(void **);
float calculate_entity_distance(void *);

#endif // HW12_H
