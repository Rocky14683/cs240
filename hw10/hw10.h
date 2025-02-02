#ifndef HW10_H
#define HW10_H

/* Constant definitions */
#define MAX_BUF_LEN      (256)

/* Error Code Definitions */

#define OK              (0)
#define NOT_FOUND       (-1)
#define NODE_EXISTS     (-2)

/* Structure Definitions */

typedef struct catalog_node {
  char* artifact_name;
  char *path;
  struct catalog_node *left_child;
  struct catalog_node *right_child;
} catalog_node_t;

typedef struct maze_node {
  char* artifact_name;
  struct maze_node *left_turn;
  struct maze_node *right_turn;
} maze_node_t;

/* Function Prototypes */

catalog_node_t *create_catalog_node(char *, char *);
maze_node_t *create_maze_node(char *);
int insert_catalog_node(catalog_node_t **, catalog_node_t *);
int insert_maze_node(maze_node_t **, maze_node_t *, char *);
int find_exotic_artifact(catalog_node_t *, char, char **);
int find_artifacts_in_path(maze_node_t *, char *, char **);
void delete_catalog_tree(catalog_node_t **);
void delete_maze_tree(maze_node_t **);

#endif // HW10_H
