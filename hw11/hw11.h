#ifndef HW11_H
#define HW11_H

/*
 * Homework 11 header file
 * CS 240, Fall 2024
 * Purdue University
 */

#define OK               (-1)
#define ALREADY_EXISTS   (-2)
#define INVALID_USERNAME (-3)
#define NOT_FOUND        (-4)

typedef struct tree_node {
  int user_id;
  struct list_node *children;
} tree_node_t;

typedef struct list_node {
  char c;
  struct tree_node *child;
  struct list_node *next;
} list_node_t;

tree_node_t *create_prefix_tree(void);
int insert_user(tree_node_t *, const char *, int);
int get_user_id(tree_node_t *, const char *);
int delete_user(tree_node_t *, const char *);
void delete_tree(tree_node_t **);

#endif // HW11_H
