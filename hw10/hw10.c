/* Add any includes here */

#include "hw10.h"

#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>



/*
 * convert the given string to lowercase
 */

void str_tolower(char **str) {
  for (int i = 0; str[i]; i++) {
    (*str)[i] = tolower((*str)[i]);
  }
} /* str_tolower() */

/*
 * check if the given path is valid
 */

bool is_valid_node(char *path) {
  for (int i = 0; i < strlen(path); i++) {
    if (!((path[i] == '<') || (path[i] == '>'))) {
      return false;
    }
  }
  return true;
} /* is_valid_node() */

/*
 * insert a new maze node into the maze tree using recursion
 */

int insert_maze_node_recur(maze_node_t **root, maze_node_t *new, char *path) {
  if (strlen(path) == 0) {
    if (*root) {
      if (!(*root)->artifact_name) {
        maze_node_t *temp = *root;
        *root = new;
        new->left_turn = temp->left_turn;
        new->right_turn = temp->right_turn;
        free(temp);
        temp = NULL;
        return OK;
      }
      return NODE_EXISTS;
    }
    *root = new;
    return OK;
  }

  if (path[0] == '<') {
    if (!(*root)->left_turn) {
      (*root)->left_turn = malloc(sizeof(maze_node_t));
      assert((*root)->left_turn);
      (*root)->left_turn->artifact_name = NULL;
      (*root)->left_turn->left_turn = NULL;
      (*root)->left_turn->right_turn = NULL;
    }
    return insert_maze_node_recur(&(*root)->left_turn, new, path + 1);
  }
  else {
    if (!(*root)->right_turn) {
      (*root)->right_turn = malloc(sizeof(maze_node_t));
      assert((*root)->right_turn);
      (*root)->right_turn->artifact_name = NULL;
      (*root)->right_turn->left_turn = NULL;
      (*root)->right_turn->right_turn = NULL;
    }
    return insert_maze_node_recur(&(*root)->right_turn, new, path + 1);
  }
} /* insert_maze_node_recur() */

/*
 * find the artifacts in the maze tree using recursion
 */

int find_artifacts_recur(maze_node_t *node, char *path, char **ret) {
  if (!node) {
    return NOT_FOUND;
  }

  if (node->artifact_name != NULL) {
    if (strlen(*ret) != 0) {
      strcat(*ret, "\\");
    }
    strcat(*ret, node->artifact_name);
  }


  if (strlen(path) == 0) {
    if (node->artifact_name) {
      return OK;
    }
    else {
      return NOT_FOUND;
    }
  }

  if (path[0] == '<') {
    return find_artifacts_recur(node->left_turn, path + 1, ret);
  }
  else {
    return find_artifacts_recur(node->right_turn, path + 1, ret);
  }
} /* find_artifacts_recur() */

/*
 * insert a new catalog node into the catalog tree using recursion
 */

int insert_catalog_recur(catalog_node_t **root, catalog_node_t *new) {
  if ((*root) == NULL) {
    (*root) = new;
    return OK;
  }

  char *root_name = malloc(strlen((*root)->artifact_name) + 1);
  assert(root_name);
  strcpy(root_name, (*root)->artifact_name);

  char *new_name = malloc(strlen(new->artifact_name) + 1);
  assert(new_name);
  strcpy(new_name, new->artifact_name);

  str_tolower(&root_name);
  str_tolower(&new_name);

  int cmp = strcmp(root_name, new_name);

  free(root_name);
  root_name = NULL;
  free(new_name);
  new_name = NULL;

  if (cmp > 0) {
    return insert_catalog_recur(&(*root)->left_child, new);
  }
  else if (cmp < 0) {
    return insert_catalog_recur(&(*root)->right_child, new);
  }
  else {
    return NODE_EXISTS;
  }
} /* insert_catalog_recur() */

/*
 * create a new catalog node
 */

catalog_node_t *create_catalog_node(char *name, char *path) {
  assert(name);
  assert(path);


  assert(is_valid_node(path));

  catalog_node_t *new = malloc(sizeof(catalog_node_t));
  assert(new);
  new->artifact_name = malloc(strlen(name) + 1);
  assert(new->artifact_name);
  strcpy(new->artifact_name, name);
  new->path = malloc(strlen(path) + 1);
  assert(new->path);
  strcpy(new->path, path);

  new->left_child = NULL;
  new->right_child = NULL;

  return new;
} /* create_catalog_node() */

/*
 * create a new maze node
 */

maze_node_t *create_maze_node(char *name) {
  assert(name);

  maze_node_t *new = malloc(sizeof(maze_node_t));
  assert(new);
  new->artifact_name = malloc(strlen(name) + 1);
  assert(new->artifact_name);
  strcpy(new->artifact_name, name);

  new->left_turn = NULL;
  new->right_turn = NULL;

  return new;
} /* create_maze_node() */

/*
 * insert a new catalog node into the catalog tree
 */

int insert_catalog_node(catalog_node_t **root, catalog_node_t *new) {
  assert(root);
  assert(new);
  assert(!new->left_child);
  assert(!new->right_child);

  if (*root == NULL) {
    *root = new;
    return OK;
  }


  return insert_catalog_recur(root, new);
} /* insert_catalog_node() */

/*
 * insert a new maze node into the maze tree
 */

int insert_maze_node(maze_node_t **root, maze_node_t *new, char *path) {
  assert(root);
  assert(new);
  assert(path);
  assert(!new->left_turn);
  assert(!new->right_turn);
  assert(is_valid_node(path));
  if (!*root) {
    *root = malloc(sizeof(maze_node_t));
    assert(*root);
    (*root)->artifact_name = NULL;
    (*root)->left_turn = NULL;
    (*root)->right_turn = NULL;
  }

  return insert_maze_node_recur(root, new, path);
} /* insert_maze_node() */

/*
 * find the exotic artifact in the catalog tree and update the ret string
 */

int find_exotic_artifact(catalog_node_t *root, char exotic, char **ret) {
  assert(ret);
  assert(!*ret);
  assert(isalpha(exotic));
  int ret_m = 0;

  if (!root) {
    return NOT_FOUND;
  }

  exotic = tolower(exotic);

  if (tolower(root->artifact_name[0]) == exotic) {
    *ret = malloc(strlen(root->path) + 1);
    assert(*ret);
    strcpy(*ret, root->path);
    return OK;
  }

  ret_m = find_exotic_artifact(root->left_child, exotic, ret);
  if (ret_m == OK) {
    return OK;
  }

  return find_exotic_artifact(root->right_child, exotic, ret);
} /* find_exotic_artifact() */

/*
 * find the artifacts with the given path and update the ret string
 */

int find_artifacts_in_path(maze_node_t *root, char *path, char **ret) {
  assert(ret);
  assert(!*ret);
  assert(path);
  assert(is_valid_node(path));

  char *temp = malloc(MAX_BUF_LEN);
  assert(temp);
  memset(temp, 0, MAX_BUF_LEN);

  int result = find_artifacts_recur(root, path, &temp);
  if (strlen(temp) == 0) {
    free(temp);
    temp = NULL;
    return result;
  }
  *ret = malloc(MAX_BUF_LEN);
  assert(*ret);
  memset(*ret, 0, MAX_BUF_LEN);
  strcpy(*ret, temp);
  free(temp);
  temp = NULL;
  return OK;
} /* find_artifacts_in_path() */

/*
 * delete the entire catalog tree
 */

void delete_catalog_tree(catalog_node_t **root) {
  assert(root);
  if (!(*root)) {
    return;
  }

  if ((*root)->left_child) {
    delete_catalog_tree(&(*root)->left_child);
  }
  if ((*root)->right_child) {
    delete_catalog_tree(&(*root)->right_child);
  }
  free((*root)->artifact_name);
  (*root)->artifact_name = NULL;
  free((*root)->path);
  (*root)->artifact_name = NULL;
  free(*root);
  *root = NULL;
} /* delete_catalog_tree() */

/*
 * delete the entire maze tree
 */

void delete_maze_tree(maze_node_t **root) {
  assert(root);
  if (!(*root)) {
    return;
  }

  if ((*root)->left_turn) {
    delete_maze_tree(&(*root)->left_turn);
  }
  if ((*root)->right_turn) {
    delete_maze_tree(&(*root)->right_turn);
  }
  free((*root)->artifact_name);
  (*root)->artifact_name = NULL;
  free(*root);
  *root = NULL;
} /* delete_maze_tree() */