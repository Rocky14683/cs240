/*
 * Homework 7 main program
 * CS 240, Fall 2024
 * Purdue University
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "hw7.h"

/* Helper function declarations */

static int read_int(const char *);
static void read_line(const char *restrict, char *restrict, size_t);
static void print_stop(const stop_t *stop);

/*
 * Prints the information contained in a stop structure.
 *
 * For pointers in link_ptrs, if the pointer points to an element of
 * g_stop_array, the index of that element is printed along with the address of
 * the pointer, to make it easier to see the links between the items.
 * If the pointer points to an address which is not the start of an element of
 * g_stop_array, a note that that pointer points outside the array is printed.
 */

static void print_stop(const stop_t *stop) {
  printf("\tAddress: %p\n", stop);
  printf("\tName: %s\n", stop->name);
  printf("\tLocation: %s\n", stop->location);
  printf("\tLink (frequency) -> pointer address:\n");
  for (int i = 0; i < MAX_LINKS; i++) {
    printf("\t\t%s (%d) -> %p",
           *stop->links[i] ? stop->links[i] : "<empty>",
           stop->bus_frequency[i], stop->link_ptrs[i]);

    /* Print index in g_stop_array if pointer points to a valid element. A
     * pointer points to a valid element if the offset from g_stop_array is a
     * multiple of sizeof(stop_t) and if the index would be within MAX_STOPS. */

    unsigned long offset = (long) stop->link_ptrs[i] - (long) g_stop_array;
    unsigned long index = offset / sizeof(stop_t);
    if ((offset % sizeof(stop_t) == 0) && (index < MAX_STOPS)) {
      printf(" (g_stop_array[%lu])", index);
    }
    else {
      printf(" (outside g_stop_array)");
    }
    printf("\n");
  }
} /* print_stop() */

/*
 * Print the return value of a function which returns an integer, including the
 * name of the error code if one exists.
 */

static void print_result(const char *funcname, int result) {
  /* ERR_CODE_MAP is an array of the error code names, where each index is the
   * negated version of the error code constant, so that all indices are
   * positive, as the error codes themselves are negative.
   */

  const char *ERR_CODE_MAP[] = {
    [-OK] = "OK",
    [-FILE_READ_ERR] = "FILE_READ_ERR",
    [-BAD_RECORD] = "BAD_RECORD",
    [-TOO_MUCH_DATA] = "TOO_MUCH_DATA",
    [-NO_DATA] = "NO_DATA",
    [-NOT_FOUND] = "NOT_FOUND",
    [-PATH_IS_CYCLE] = "PATH_IS_CYCLE",
  };

  printf("%s() returned %d", funcname, result);
  if (result <= 0) {
    printf(" (%s)", ERR_CODE_MAP[-result]);
  }
  printf("\n");
} /* print_result() */

/*
 * This function is used to run the different functions implemented in file
 * hw6.c. User input is used to determine which function to run and what input
 * is passed to it. Upon completion, the function returns zero.
 */

int main() {
  /* While good_file is not used outside of the loop, its value must persist
   * between iterations, so it is defined here. */

  bool good_file = false;

  while (true) {
    printf(
      "Select a function to test:\n"
      "    1) read_stops()\n"
      "    2) connect_stops()\n"
      "    3) longest_min_freq_path()\n"
      "    4) hubs_in_max_freq_path()\n"
      "    5) print stops\n"
      "    0) Exit\n");

    int choice = read_int("Enter your choice: ");
    printf("\n");

    char in_file[MAX_FILE_LEN] = {'\0'};
    int result = 0;
    int start_i = 0;

    if ((choice > 1) && (!good_file)) {
      printf("\ng_stop_array is not loaded. Use read_stops() first!\n\n");
      continue;
    }

    if ((choice == 3) || (choice == 4)) {
      start_i = read_int("Enter the index of the stop to start at: ");
      if (start_i >= g_stop_count) {
        printf("Error: Index must be less than g_stop_count, which is %d\n",
               g_stop_count);
        break;
      }
    }

    switch (choice) {

      case 1:
        read_line("Enter the input file: ", in_file, sizeof(in_file));
        result = read_stops(in_file);
        print_result("read_stops", result);
        good_file = (result >= 0);
        break;

      case 2:
        result = connect_stops();
        print_result("connect_stops", result);
        break;

      case 3:
        result = longest_min_freq_path(&g_stop_array[start_i]);
        print_result("longest_min_freq_path", result);
        break;

      case 4: {
        /* buf's length is enough to store each stop's name followed by a
         * newline, plus the final NUL terminator. */

        char buf[MAX_NAME_SIZE * MAX_STOPS + 1] = {'\0'};
        result = hubs_in_max_freq_path(&g_stop_array[start_i], buf);
        print_result("hubs_in_max_freq_path", result);
        printf("Value of output string "
               "(surrounding quotes added for readability):\n"
               "\"%s\"\n\n", buf);
        break;
      }

      case 5:
        printf("The following stops were loaded:\n");
        for (int i = 0; i < g_stop_count; i++) {
          printf("\nStop %d:\n", i);
          print_stop(&g_stop_array[i]);
        }
        printf("\n");
        break;

      case 0:
        printf("\nGoodbye!\n\n");
        return 0;

      default:
        printf("\nSelect one of the given options.\n\n");
        break;
    }
  }

  return 0;
} /* main() */

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
  fgets(buf, size - 1, stdin);
  size_t len = strlen(buf);
  if (buf[len - 1] == '\n') {
    buf[len - 1] = '\0';
  }
  else {
    scanf("%*[^\n]%*c");
  }
} /* read_line() */

/* vim: set ft=c ts=4 sw=2 et : */
