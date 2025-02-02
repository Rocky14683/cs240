/*
 * Homework 12
 * Your Name
 * CS 240, Fall 2024
 * Purdue University
 */

#include "hw12.h"

#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Function to print the internal data of a chunk_t struct passed as a void
 * pointer to the standard output stream.
 */

void print_chunk_data(void *data) {
  assert(data);
  chunk_t *chunk = (chunk_t *) data;
  int filled = 0;
  for (int i = 0; i < CHUNKSIZE; i++) {
    for (int j = 0; j < CHUNKSIZE; j++) {
      for (int k = 0; k < CHUNKSIZE; k++) {
        if (chunk->blocks[i][j][k] > 0) {
          filled++;
        }
      }
    }
  }
  printf("Chunk at (%d, %d, %d): %u/%u blocks filled\n",
         chunk->chunk_x, chunk->chunk_y, chunk->chunk_z, filled,
         CHUNKSIZE * CHUNKSIZE * CHUNKSIZE);
} /* print_chunk_data() */

/*
 * Function to print the internal data of a entity_t struct passed as a void
 * pointer to the standard output stream.
 */

void print_entity_data(void *data) {
  assert(data);
  entity_t *entity = (entity_t *) data;
  assert(entity->name);
  printf("Entity \"%s\" at (%.0f, %.0f, %.0f), type %d, id %d, health %.0f%%\n",
         entity->name, entity->x, entity->y, entity->z, entity->type,
         entity->id, entity->health_pct * 100.0);
} /* print_entity_data() */

/*
 * Function to print a tree node by dynamically dispatching the print function
 * stored in the node on the data stored in the node
 */

void print_node(node_t *node) {
  assert(node);
  assert(node->data);
  assert(node->print);
  (node->print)(node->data);
} /* print_node() */

/*
 * Function to recursively print the tree using an in-order traversal
 */

void print_tree(node_t *root) {
  if (!root) {
    return;
  }
  print_tree(root->left_child);
  print_node(root);
  print_tree(root->right_child);
} /* print_tree() */

/*
 * Delete the entire tree
 */

void delete_tree(node_t **root) {
  assert(root);
  if (!*root) {
    return;
  }
  delete_tree(&((*root)->left_child));
  delete_tree(&((*root)->right_child));
  delete_node(root);
} /* delete_tree() */

/*
 * Create a new node with the given data and function pointers
 */

void create_node(node_t **root, void *ptr_data, void (*print_fn)(void *),
                 void (*delete_fn)(void **), float (*distance_fn)(void *)) {
  assert(root);
  assert(*root == NULL);
  assert(ptr_data);
  assert(print_fn);
  assert(delete_fn);
  assert(distance_fn);
  *root = malloc(sizeof(node_t));
  assert(*root);
  (*root)->left_child = NULL;
  (*root)->right_child = NULL;
  (*root)->data = ptr_data;
  (*root)->print = print_fn;
  (*root)->delete = delete_fn;
  (*root)->distance = distance_fn;
} /* create_node() */

/*
 * Delete a node and its data
 */

void delete_node(node_t **node) {
  assert(node);
  assert(*node);
  assert((*node)->delete);
  assert((*node)->data);
  ((*node)->delete)(&((*node)->data));
  free(*node);
  *node = NULL;
} /* delete_node() */

/*
 * Compare two nodes by their distance functions
 */

int compare_nodes(node_t *node_a, node_t *node_b) {
  assert(node_a);
  assert(node_b);
  assert(node_a->distance);
  assert(node_b->distance);
  assert(node_a->data);
  assert(node_b->data);
  float distance_a = node_a->distance(node_a->data);
  float distance_b = node_b->distance(node_b->data);

  if (distance_a < distance_b) {
    return -1;
  }
  else if (distance_a > distance_b) {
    return 1;
  }
  return 0;
} /* compare_nodes() */

/*
 * Insert a node into the tree
 */

void insert_node(node_t **root, node_t *node) {
  assert(root);
  assert(node);
  node_t *cur = *root;
  node_t *parent = NULL;

  while (cur != NULL) {
    parent = cur;
    if (compare_nodes(node, cur) <= 0) {
      cur = cur->left_child;
    }
    else {
      cur = cur->right_child;
    }
  }

  if (!parent) {
    *root = node;
    return;
  }

  if (compare_nodes(node, parent) <= 0) {
    parent->left_child = node;
  }
  else {
    parent->right_child = node;
  }

} /* insert_node() */

/*
 * Find the inorder predecessor of a node
 */

node_t *find_inorder_predecessor(node_t *node) {
  node_t *current = node->left_child;
  while ((current) && (current->right_child)) {
    current = current->right_child;
  }
  return current;
} /* find_inorder_predecessor() */

/*
 * Remove a node from the tree recursively
 */

node_t *remove_node_recur(node_t *root, node_t *target) {
  if (!root) {
    return NULL;
  }


  if (compare_nodes(target, root) == -1) {
    root->left_child = remove_node_recur(root->left_child, target);
  }
  else if (compare_nodes(target, root) == 1) {
    root->right_child = remove_node_recur(root->right_child, target);
  }
  else if (target == root) {
    if ((!root->left_child) && (!root->right_child)) {
      delete_node(&root);
      return NULL;
    }
    else if ((!root->left_child) || (!root->right_child)) {
      node_t *child = root->left_child ? root->left_child : root->right_child;
      delete_node(&root);
      return child;
    }
    else {
      node_t *predecessor = find_inorder_predecessor(root);
      memcpy(root->data, predecessor->data, sizeof(entity_t));
      root->delete = predecessor->delete;
      root->print = predecessor->print;
      root->distance = predecessor->distance;
      root->left_child = remove_node_recur(root->left_child, predecessor);
    }
  }
  return root;
} /* remove_node_recur() */

/*
 * Remove a node from the tree
 */

void remove_node(node_t **root, node_t *target) {
  assert(root);
  assert(*root);
  assert(target);
  *root = remove_node_recur(*root, target);
} /* remove_node() */

/*
 * Find nodes within a given distance range
 */

void find_node_recur(node_t *root, float lower_bound, float upper_bound,
                     node_t **array, int *count) {
  if (!root) {
    return;
  }
  float distance = root->distance(root->data);
  if ((distance >= lower_bound) && (distance <= upper_bound)) {
    if (array != NULL) {
      array[*count] = root;
    }
    (*count)++;
  }
  find_node_recur(root->left_child, lower_bound, upper_bound, array, count);
  find_node_recur(root->right_child, lower_bound, upper_bound, array, count);
} /* find_node_recur() */

/*
 * Find nodes within a given distance range and increase the count
 */

node_t **
find_nodes(node_t *root, float lower_bound, float upper_bound, int *value_ptr) {

  assert(root);
  assert(upper_bound >= lower_bound);
  assert(lower_bound >= 0);
  assert(upper_bound >= 0);
  assert(value_ptr);

  *value_ptr = 0;
  find_node_recur(root, lower_bound, upper_bound, NULL, value_ptr);

  if (*value_ptr == 0) {
    return NULL;
  }

  node_t **array = malloc(sizeof(node_t*) * (*value_ptr));
  assert(array);

  int index = 0;
  find_node_recur(root, lower_bound, upper_bound, array, &index);

  return array;
} /* find_nodes() */

/*
 * Create a new chunk data structure
 */

void *create_chunk_data(int x, int y, int z,
                        unsigned char blocks[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE]) {
  assert(blocks);
  chunk_t *chunk = malloc(sizeof(chunk_t));
  assert(chunk);
  chunk->chunk_x = x;
  chunk->chunk_y = y;
  chunk->chunk_z = z;
  for (int i = 0; i < CHUNKSIZE; i++) {
    for (int j = 0; j < CHUNKSIZE; j++) {
      for (int k = 0; k < CHUNKSIZE; k++) {
        chunk->blocks[i][j][k] = blocks[i][j][k];
      }
    }
  }
  return chunk;
} /* create_chunk_data() */

/*
 * Delete a chunk data structure
 */

void delete_chunk_data(void **chunk_ptr) {
  assert(chunk_ptr);
  assert(*chunk_ptr);

  chunk_t *chunk = (chunk_t *) *chunk_ptr;
  free(chunk);
  *chunk_ptr = NULL;
} /* delete_chunk_data() */

/*
 * Calculate the distance of a chunk from the origin
 */

float calculate_chunk_distance(void *data) {
  assert(data);
  chunk_t *chunk = (chunk_t *) data;

  float center_x = 4.0 * chunk->chunk_x + 2.0;
  float center_y = 4.0 * chunk->chunk_y + 2.0;
  float center_z = 4.0 * chunk->chunk_z + 2.0;

  float dist = sqrtf(
          center_x * center_x + center_y * center_y + center_z * center_z);

  return dist;
} /* calculate_chunk_distance() */

/*
 * Create a new entity data structure
 */

void *create_entity_data(float x, float y, float z, const char *name, int id,
                         int type, float health_pct) {
  assert(name);
  assert(x >= 0);
  assert(y >= 0);
  assert(z >= 0);
  assert(id >= 0);
  assert(type >= 0);
  assert((health_pct <= 100) && (health_pct >= 0));
  entity_t *entity = malloc(sizeof(entity_t));
  assert(entity);
  entity->x = x;
  entity->y = y;
  entity->z = z;
  entity->name = malloc(strlen(name) + 1);
  assert(entity->name);
  strcpy(entity->name, name);
  entity->id = id;
  entity->type = type;
  entity->health_pct = health_pct;
  return entity;
} /* create_entity_data() */

/*
 * Delete an entity data structure
 */

void delete_entity_data(void **entity_ptr) {
  assert(entity_ptr);
  assert(*entity_ptr);

  entity_t *entity = (entity_t *) *entity_ptr;
  free(entity->name);
  free(entity);
  *entity_ptr = NULL;
} /* delete_entity_data() */

/*
 * Calculate the distance of an entity from the origin
 */

float calculate_entity_distance(void *data) {
  assert(data);
  entity_t *entity = (entity_t *) data;

  float dist = sqrtf(entity->x * entity->x + entity->y * entity->y +
                     entity->z * entity->z);
  return dist;
} /* calculate_entity_distance() */
