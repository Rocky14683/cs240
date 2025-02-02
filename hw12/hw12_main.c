/*
 * Homework 12 main program
 * CS 240, Fall 2024
 * Purdue University
 */

#include "hw12.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Input helper functions */

static int read_int(const char *);
static void read_line(const char *restrict, char *restrict, size_t);
static double read_double(const char *);
static void *read_ptr(const char *);

/* Runner helper functions */

void run_create_node(void);

/*
 *  This function is used to run the functions in hw12.c. User input is used
 *  to determine which function to run and what is passed to it. main()
 *  prints information related to running the chosen function.
 */

int main(void) {
  node_t *tree = NULL;

  for (;;) {
    printf("\n"
           "Select an action:\n"
           "  0)  Exit\n"
           "  --\n"
           "  1)  create_node()\n"
           "  2)  delete_node()\n"
           "  3)  print_node()\n"
           "  4)  compare_nodes()\n"
           "  --\n"
           "  5)  insert_node()\n"
           "  6)  remove_node()\n"
           "  7)  find_nodes()\n"
           "  --\n"
           "  8)  print_tree()\n"
           "  9)  delete_tree()\n"
           "\n"
        );
    int choice = read_int("Enter your choice: ");
    printf("\n");

    if ((choice < 0) || (choice > 9)) {
      printf("Invalid choice.\n");
      continue;
    }

    if (choice == 0) {  /* Exit */
      break;
    }
    else if (choice == 1) {  /* create_node() */
      run_create_node();
    }
    else if (choice == 2) {  /* delete_node() */
      node_t *node = read_ptr("Enter node address (or 0 to cancel): ");
      if (node) {
        delete_node(&node);
        printf("\ndelete_node() set the node pointer to: %p\n", node);
      }
    }
    else if (choice == 3) {  /* print_node() */
      node_t *node = read_ptr("Enter node address (or 0 to cancel): ");
      if (node) {
        printf("\n");
        print_node(node);
      }
    }
    else if (choice == 4) {  /* compare_nodes() */
      node_t *first = read_ptr("Enter first node address (or 0 to cancel): ");
      if (first) {
        node_t *second =
          read_ptr("Enter second node address (or 0 to cancel): ");
        if (second) {
          int cmp = compare_nodes(first, second);
          printf("\ncompare_nodes() returned: %d\n", cmp);
        }
      }
    }
    else if (choice == 5) {  /* insert_node() */
      node_t *node = read_ptr("Enter node address (or 0 to cancel): ");
      if (node) {
        insert_node(&tree, node);
      }
    }
    else if (choice == 6) {  /* remove_node() */
      node_t *node = read_ptr("Enter node address (or 0 to cancel): ");
      if (node) {
        printf("\nRoot node before removal: %p\n", tree);
        remove_node(&tree, node);
        printf("Root node after removal: %p\n", tree);
      }
    }
    else if (choice == 7) {  /* find_nodes() */
      float min = read_double("Enter lower distance bound: ");
      float max = read_double("Enter upper distance bound: ");
      int n_nodes = 0;
      node_t **nodes = find_nodes(tree, min, max, &n_nodes);
      printf("\nfind_nodes() returned array %p and length %d\n",
             nodes, n_nodes);
      int print_choice = read_int("Enter 1 to print array or 0 to skip: ");
      if (print_choice == 1) {
        for (int i = 0; i < n_nodes; i++) {
          printf("\n");
          print_node(nodes[i]);
        }
      }
      free(nodes);
      nodes = NULL;
    }
    else if (choice == 8) {  /* print_tree() */
      print_tree(tree);
    }
    else if (choice == 9) {  /* delete_tree() */
      printf("Root pointer before deletion: %p\n", tree);
      delete_tree(&tree);
      printf("Root pointer after deletion: %p\n", tree);
    }
  }
} /* main() */

/*
 * Runs the create_node() function
 */

void run_create_node(void) {
  printf("Select node type:\n"
         "  1)  Chunk\n"
         "  2)  Entity\n");
  int type = 0;
  for (;;) {
    type = read_int("Enter your choice: ");
    if ((type == 1) || (type == 2)) {
      break;
    }
    printf("Invalid choice. Try again.\n");
  }
  printf("\n");

  node_t *node = NULL;

  if (type == 1) {  /* Chunk */
    int x = read_int("Enter chunk X coordinate: ");
    int y = read_int("Enter chunk Y coordinate: ");
    int z = read_int("Enter chunk Z coordinate: ");

    /* Fill blocks with random data, since it doesn't really matter. */

    printf("Generating random block data...\n");
    unsigned char blocks[CHUNKSIZE][CHUNKSIZE][CHUNKSIZE] = {0};
    for (unsigned int i = 0; i < CHUNKSIZE; i++) {
      for (unsigned int j = 0; j < CHUNKSIZE; j++) {
        for (unsigned int k = 0; k < CHUNKSIZE; k++) {
          blocks[i][j][k] = rand() % UCHAR_MAX;
        }
      }
    }

    chunk_t *data = create_chunk_data(x, y, z, blocks);
    create_node(&node, data, print_chunk_data, delete_chunk_data,
                calculate_chunk_distance);
  }
  else if (type == 2) {  /* Entity */
    float x = read_double("Enter X coordinate: ");
    float y = read_double("Enter Y coordinate: ");
    float z = read_double("Enter Z coordinate: ");
    char name[BUFSIZ] = {'\0'};
    read_line("Enter entity name: ", name, sizeof(name));
    int type = read_int("Enter entity type: ");
    int id = read_int("Enter entity ID: ");
    float health_pct = read_double("Enter entity health: ");

    entity_t *data = create_entity_data(x, y, z, name, type, id, health_pct);
    create_node(&node, data, print_entity_data, delete_entity_data,
                calculate_entity_distance);
  }

  printf("\ncreate_node() returned node: %p\n", node);
} /* run_create_node() */

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
  fgets(buf, size, stdin);
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
 * Prints the prompt, then reads a line containing a single double from
 * standard input. Repeats the prompt/input stage until a valid input is read.
 * Returns the double read.
 */

static double read_double(const char *prompt) {
  do {
    double value = 0.0;
    fputs(prompt, stdout);
    if ((scanf("%lf", &value) == 1) && (getchar() == '\n')) {
      return value;
    }
    else {
      while (getchar() != '\n') {
      }
      printf("Invalid input. Try again.\n");
    }
  } while (1);
} /* read_double() */

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
