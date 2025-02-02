/*
 * Homework 11 template
 * CS 240, Fall 2024
 * Purdue University
 */

/* Add any includes here */

#include "hw11.h"

#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <printf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


/* Define the required functions here */


/*
 * Read an integer from standard input
 */

bool is_valid_username(const char *username) {
  while (*username) {
    if (!isalnum(*username)) {
      return false;
    }
    username++;
  }
  return true;
} /* is_valid_username() */

/*
 * Recursively delete a user from the tree
 */

int delete_user_recur(tree_node_t *root, const char *username) {
  if (root == NULL) {
    return NOT_FOUND;
  }

  if (strlen(username) == 0) {
    if (root->user_id == -1) {
      return NOT_FOUND;
    }
    root->user_id = -1;
    return OK;
  }

  list_node_t *cur = root->children;
  while (cur) {
    if (cur->c == username[0]) {
      return delete_user_recur(cur->child, username + 1);
    }
    cur = cur->next;
  }

  return NOT_FOUND;
} /* delete_user_recur() */

/*
 * Create a new prefix tree
 */

tree_node_t *create_prefix_tree(void) {
  tree_node_t *root = malloc(sizeof(tree_node_t));
  assert(root);
  root->user_id = -1;
  root->children = NULL;
  return root;
} /* create_prefix_tree() */

/*
 * Insert a new user into the tree
 */

int insert_user(tree_node_t *tree, const char *username, int id) {
  assert(tree);
  assert(username);
  assert(id >= 0);
  if (!is_valid_username(username)) {
    return INVALID_USERNAME;
  }

  tree_node_t *cur_tree = tree;
  int char_offset = 0;

  while (username[char_offset] != '\0') {
    bool insert = false;
    list_node_t *cur = cur_tree->children;
    list_node_t *prev = NULL;

    while (cur) {
      if (cur->c == username[char_offset]) {
        break;
      }
      else if (cur->c > username[char_offset]) {
        insert = true;
        break;
      }
      prev = cur;
      cur = cur->next;
    }


    if (!cur) {
      cur = malloc(sizeof(list_node_t));
      assert(cur);
      cur->c = username[char_offset];
      cur->child = malloc(sizeof(tree_node_t));
      assert(cur->child);
      cur->child->user_id = -1;
      cur->child->children = NULL;
      cur->next = NULL;

      if (prev) {
        prev->next = cur;
      }
      else {
        cur_tree->children = cur;
      }
    }
    else if (insert) {
      list_node_t *new_node = malloc(sizeof(list_node_t));
      assert(new_node);
      new_node->c = username[char_offset];
      new_node->child = malloc(sizeof(tree_node_t));
      assert(new_node->child);
      new_node->child->user_id = -1;
      new_node->child->children = NULL;
      new_node->next = cur;

      if (prev) {
        prev->next = new_node;
      }
      else {
        cur_tree->children = new_node;
      }
      cur = new_node;
    }

    char_offset++;
    cur_tree = cur->child;
  }

  if (cur_tree->user_id != -1) {
    return ALREADY_EXISTS;
  }


  cur_tree->user_id = id;

  return OK;
} /* insert_user() */

/*
 * Get the user ID from the tree given a username
 */

int get_user_id(tree_node_t *tree, const char *username) {
  assert(tree);
  assert(username);
  if (!is_valid_username(username)) {
    return INVALID_USERNAME;
  }

  tree_node_t *current = tree;
  int count = 0;
  while (username[count] != '\0') {
    list_node_t *child = current->children;
    while ((child) && (child->c != username[count])) {
      child = child->next;
    }
    if (!child) {
      return NOT_FOUND;
    }
    current = child->child;
    count++;
  }


  return current->user_id == -1 ? NOT_FOUND : current->user_id;
} /* get_user_id() */

/*
 * Delete a user from the tree
 */

int delete_user(tree_node_t *root, const char *username) {
  assert(root);
  assert(username);
  if (!is_valid_username(username)) {
    return INVALID_USERNAME;
  }

  return delete_user_recur(root, username);
} /* delete_user() */

/*
 * Recursively delete the tree
 */

void delete_tree(tree_node_t **root) {
  assert(root);
  if (!*root) {
    return;
  }

  list_node_t *cur = (*root)->children;
  list_node_t *next = NULL;

  while (cur) {
    next = cur->next;
    delete_tree(&(cur->child));
    free(cur);
    cur = NULL;
    cur = next;
  }

  free(*root);
  *root = NULL;
} /* delete_tree() */