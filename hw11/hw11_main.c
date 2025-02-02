/*
 * Homework 11 main program
 * CS 240, Fall 2024
 * Purdue University
 */

#include <stdio.h>
#include <string.h>

#include "hw11.h"

static int read_int(const char *);
static void read_line(const char *restrict, char *restrict, size_t);
static void print_tree(const tree_node_t *, unsigned int);

int main() {
  /* Store the root of the tree. */

  tree_node_t *root = NULL;

  for (;;) {
    printf("\nSelect a function:\n"
           "  0)  Exit\n"
           "  --\n"
           "  1)  create_prefix_tree()\n"
           "  2)  insert_user()\n"
           "  3)  get_user_id()\n"
           "  4)  delete_user()\n"
           "  5)  delete_tree()\n"
           "  --\n"
           "  6)  Print tree\n"
           );
    int choice = read_int("Enter your selection: ");
    printf("\n");

    /* User input variables */

    char username[BUFSIZ] = {'\0'};
    int user_id = 0;

    if ((choice == 2) || (choice == 3) || (choice == 4)) {
      read_line("Enter a username: ", username, sizeof(username));
    }

    if (choice == 0) { /* Exit */
      break;
    }
    else if (choice == 1) { /* create_prefix_tree() */
      root = create_prefix_tree();
      printf("create_prefix_tree() returned: %p\n", root);
      printf("This node will be used as the root "
             "of the tree for future operations.\n");
    }
    else if (choice == 2) { /* insert_user() */
      user_id = read_int("Enter a user ID: ");
      int result = insert_user(root, username, user_id);
      printf("insert_user() returned: %d\n", result);
    }
    else if (choice == 3) { /* get_user_id() */
      int result = get_user_id(root, username);
      printf("get_user_id() returned: %d\n", result);
    }
    else if (choice == 4) { /* delete_user() */
      int result = delete_user(root, username);
      printf("delete_user() returned: %d\n", result);
    }
    else if (choice == 5) { /* delete_tree() */
      delete_tree(&root);
      printf("delete_tree() updated the root pointer to: %p\n", root);
    }
    else if (choice == 6) { /* Print tree */
      print_tree(root, 0);
    }
  }
}

/*
 * Prints the prefix tree in a nested textual form. The first argument is a
 * pointer to the node to start at and the second argumentis the indentation
 * level to start at.
 */

static void print_tree(const tree_node_t *root, unsigned int level) {
  if (!root) {
    printf("(null)\n");
    return;
  }

  printf("ID %d (%p)\n", root->user_id, root);

  for (list_node_t *node = root->children; node; node = node->next) {
    /* Print indentation. Each level is 7 spaces. */

    for (int i = 0; i < level; i++) {
      printf("       ");
    }

    printf("+ %c -> ", node->c);
    print_tree(node->child, level + 1);
  }
} /* print_tree() */

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
      scanf("%*[^\n]%*c");
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
    scanf("%*[^\n]%*c");
  }
} /* read_line() */

/* vim: set ft=c ts=4 sw=2 et : */
