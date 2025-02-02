/*
 * Homework 10 main program
 * CS 240, Fall 2024
 * Purdue University
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "hw10.h"

#define LINESZ (4096)

static void import_trees_from_file(const char *, catalog_node_t **,
                                   maze_node_t **);
static void print_catalog_tree(catalog_node_t *, unsigned int);
static void print_maze_tree(maze_node_t *, unsigned int);
static int read_int(const char *);
static void read_line(const char *restrict, char *restrict, size_t);
static char read_char(const char *);
static void *read_ptr(const char *);

/*
 * Runs the functions defined in hw10.c based on user input.
 */

int main() {
  /* Variables for user input. */

  char *name = malloc(LINESZ);
  assert(name);
  char *path = malloc(LINESZ);
  assert(name);

  /* Trees. */

  catalog_node_t *catalog_tree = NULL;
  maze_node_t *maze_tree = NULL;

  for (;;) {
    /* Get operation. */

    printf("\nSelect an operation:\n\n"
           "  0)  Exit\n"
           " ---\n"
           "  1)  create_catalog_node()\n"
           "  2)  create_maze_node()\n"
           "  3)  insert_catalog_node()\n"
           "  4)  insert_maze_node()\n"
           "  5)  find_exotic_artifact()\n"
           "  6)  find_artifacts_in_path()\n"
           "  7)  delete_catalog_tree()\n"
           "  8)  delete_maze_tree()\n"
           " ---\n"
           "  9)  Print catalog tree\n"
           " 10)  Print maze tree\n"
           " ---\n"
           " 11)  Import trees from file\n"
           "\n");
    int choice = 0;
    for (;;) {
      choice = read_int("Enter your choice: ");
      if ((choice >= 0) && (choice <= 11)) {
        break;
      }
      printf("Invalid choice. Try again.\n");
    }
    printf("\n");

    /* Perform operation. */

    if (choice == 0) {  /* Exit */
      break;
    }
    else if (choice == 1) {  /* create_catalog_node() */
      read_line("Enter artifact name: ", name, LINESZ);
      read_line("Enter artifact path: ", path, LINESZ);
      catalog_node_t *node = create_catalog_node(name, path);
      printf("create_catalog_node() returned %p\n", node);
    }
    else if (choice == 2) {  /* create_maze_node() */
      read_line("Enter artifact name: ", name, LINESZ);
      maze_node_t *node = create_maze_node(name);
      printf("create_maze_node() returned %p\n", node);
    }
    else if (choice == 3) {  /* insert_catalog_node() */
      catalog_node_t *new = read_ptr("Enter address of node to insert: ");
      if (new) {
        int ret = insert_catalog_node(&catalog_tree, new);
        printf("insert_catalog_node() returned %d\n", ret);
      }
    }
    else if (choice == 4) {  /* insert_maze_node() */
      maze_node_t *new = read_ptr("Enter address of node to insert: ");
      read_line("Enter path to the new artifact's position: ", path, LINESZ);
      if (new) {
        int ret = insert_maze_node(&maze_tree, new, path);
        printf("insert_maze_node() returned %d\n", ret);
      }
    }
    else if (choice == 5) {  /* find_exotic_artifact() */
      char *ret_str = NULL;
      char c = read_char("Enter character to search for: ");
      int ret = find_exotic_artifact(catalog_tree, c, &ret_str);
      printf("find_exotic_artifact() returned %d\n", ret);
      if (ret_str) {
        printf("The returned string is \"%s\"\n", ret_str);
        free(ret_str);
        ret_str = NULL;
      }
      else {
        printf("The returned string is NULL.\n");
      }
    }
    else if (choice == 6) {  /* find_artifacts_in_path() */
      char *ret_str = NULL;
      read_line("Enter path to search along: ", path, LINESZ);
      int ret = find_artifacts_in_path(maze_tree, path, &ret_str);
      printf("find_artifacts_in_path() returned %d\n", ret);
      if (ret_str) {
        printf("The returned string is \"%s\"\n", ret_str);
        free(ret_str);
        ret_str = NULL;
      }
      else {
        printf("The returned string is NULL.\n");
      }
    }
    else if (choice == 7) {  /* delete_catalog_tree() */
      delete_catalog_tree(&catalog_tree);
      printf("delete_catalog_tree() returned.\n"
             "The catalog tree root pointer is now %p\n",
             catalog_tree);
    }
    else if (choice == 8) {  /* delete_maze_tree() */
      delete_maze_tree(&maze_tree);
      printf("delete_maze_tree() returned.\n"
             "The maze tree root pointer is now %p\n",
             maze_tree);
    }
    else if (choice == 9) {  /* Print catalog tree */
      print_catalog_tree(catalog_tree, 0);
    }
    else if (choice == 10) {  /* Print maze tree */
      print_maze_tree(maze_tree, 0);
    }
    else if (choice == 11) {  /* Import trees from file */
      printf(
        "The file to read from should contain lines in the following format:\n"
        "\n"
        "Item name:<><<>>><\n"
        "\n"
        "Each line will be interpreted as an artifact and read into both the\n"
        "maze and catalog trees. The existing trees will not be deleted first.\n"
        "You can do that using the delete_*_tree() functions.\n"
        "\n"
        );

      /* Re-use name buffer for filename. */

      read_line("Enter the filename to import from: ", name, LINESZ);
      printf("\n");
      import_trees_from_file(name, &catalog_tree, &maze_tree);
    }
  }

  /* Free buffers. */

  if (name) {
    free(name);
    name = NULL;
  }
  if (path) {
    free(path);
    path = NULL;
  }
} /* main() */

/*
 * Imports tree data from a text file. The format of the file is specified in
 * the if statement which handles option 9 in the main() function.
 *
 * If an error occurs, a message is printed to stderr.
 */

static void import_trees_from_file(const char *filename,
                                   catalog_node_t **catalog_tree_ptr,
                                   maze_node_t **maze_tree_ptr) {
  assert(filename);
  assert(catalog_tree_ptr);
  assert(maze_tree_ptr);

  FILE *fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "Failed to open file for reading.\n");
    return;
  }

  char *name = malloc(LINESZ);
  assert(name);
  char *path = malloc(LINESZ);
  assert(path);

  unsigned int line = 1;
  for (;;) {
    char c = '\0';
    int ret = fscanf(fp, "%4095[^:]:%4095[^\n]%c", name, path, &c);
    if (ret == EOF) {
      break;
    }
    else if ((ret != 3) || (!*name) || (!*path) || (c != '\n')) {
      fprintf(stderr, "Failed reading line %d.\n", line);
      return;
    }
    catalog_node_t *catalog_node = create_catalog_node(name, path);
    if (insert_catalog_node(catalog_tree_ptr, catalog_node) != OK) {
      fprintf(stderr, "Failed to insert catalog node at line %d.\n", line);
      return;
    }
    maze_node_t *maze_node = create_maze_node(name);
    if (insert_maze_node(maze_tree_ptr, maze_node, path) != OK) {
      fprintf(stderr, "Failed to insert maze node at line %d.\n", line);
      return;
    }

    line++;
  }

  free(name);
  name = NULL;
  free(path);
  path = NULL;

  fclose(fp);
  fp = NULL;

  printf("Successfully read %d entries.\n", line - 1);
} /* import_trees_from_file() */

/*
 * Prints a textual representation of the catalog tree to stdout.
 */

static void print_catalog_tree(catalog_node_t *root, unsigned int level) {
  if (!root) {
    printf("NULL\n");
    return;
  }
  printf("Name: %s, Path: %s\n", root->artifact_name, root->path);
  for (int i = 0; i < level; i++) {
    printf("\t");
  }
  printf("Left child -> ");
  print_catalog_tree(root->left_child, level + 1);
  for (int i = 0; i < level; i++) {
    printf("\t");
  }
  printf("Right child -> ");
  print_catalog_tree(root->right_child, level + 1);
} /* print_catalog_tree() */

/*
 * Prints a textual representation of the maze tree to stdout.
 */

static void print_maze_tree(maze_node_t *root, unsigned int level) {
  if (!root) {
    printf("NULL\n");
    return;
  }
  printf("Name: %s\n", root->artifact_name ? root->artifact_name : "(NULL)");
  for (int i = 0; i < level; i++) {
    printf("\t");
  }
  printf("Left turn -> ");
  print_maze_tree(root->left_turn, level + 1);
  for (int i = 0; i < level; i++) {
    printf("\t");
  }
  printf("Right turn -> ");
  print_maze_tree(root->right_turn, level + 1);
} /* print_maze_tree() */

/*
 * Prints the prompt, then reads a line containing a single integer from
 * standard input. Repeats the prompt/input stage until a valid input is read.
 * Returns the integer read.
 */

static int read_int(const char *prompt) {
  do {
    int num = 0;
    fputs(prompt, stdout);
    if ((scanf("%d", &num) == 1) && (getchar() == '\n')) {
      return num;
    }
    else {
      while (getchar() != '\n') {
      }
      printf("Invalid input. Try again.\n");
    }
  } while (1);
} /* read_int() */

/*
 * Prints the given prompt, then reads a line from standard input, and stores it
 * in the given buffer. Truncates the stored string to the given size, including
 * the NUL terminator.
 */

static void read_line(const char *restrict prompt, char *restrict buf,
                      size_t size) {
  fputs(prompt, stdout);
  fgets(buf, size - 1, stdin);
  size_t len = strlen(buf);
  if (buf[len - 1] == '\n') {
    buf[len - 1] = '\0';
  }
  else {
    while (getchar() != '\n') {
    }
  }
} /* read_line() */

/*
 * Prints the prompt, then reads a line containing a single character
 * from standard input. Repeats the prompt/input stage until a valid input
 * is read. If EOF is reached, this function returns the NUL character.
 */

static char read_char(const char *prompt) {
  do {
    fputs(prompt, stdout);
    int c = getchar();
    if (c == EOF) {
      return '\0';
    }
    else if ((c != '\n') && (getchar() == '\n')) {
      return (char) c;
    }
    while (getchar() != '\n') {
    }
    printf("Invalid input. Please enter a single character.\n");
  } while (1);
} /* read_char() */

/*
 * Prints the prompt, then reads a line containing a single pointer from
 * standard input. Format of the pointer is the same as would be printed
 * by printf(3). Repeats the prompt/input stage until a valid input is read.
 * Returns the pointer read.
 *
 * Note: There is no simple & consistent way to verify that a pointer entered
 * is valid. It is very likely that if the user enters an invalid pointer,
 * the program will crash.
 */

static void *read_ptr(const char *prompt) {
  do {
    void *ptr;
    fputs(prompt, stdout);
    if ((scanf("%p", &ptr) == 1) && (getchar() == '\n')) {
      return ptr;
    }
    else {
      while (getchar() != '\n') {
      }
      printf("Invalid input. Try again.\n");
    }
  } while (1);
} /* read_ptr() */
