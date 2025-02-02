#include "hw6.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 * Prints the information of a stop.
 */

static void print_stop(stop_t *stop) {
  printf("\tName: %s\n", stop->name);
  printf("\tLocation: %s\n", stop->location);
  printf("\tLink (frequency):\n");
  for (int i = 0; i < MAX_LINKS; i++) {
    printf("\t\t%s (%d)\n",
           *stop->links[i] ? stop->links[i] : "<empty>",
           stop->bus_frequency[i]);
  }
} /* print_stop() */

/*
 * This function is used to run the different functions implemented in file
 * hw6.c. User input is used to determine which function to run and what input
 * is passed to it. Upon completion, the function returns zero.
 */

int main() {
  int choice = 0;
  bool good_file = false;
  while (true) {
    printf(
      "Select a function to test:\n"
      "    1) read_stops()\n"
      "    2) find_hubs()\n"
      "    3) print stops\n"
      "    4) Exit\n"
      "Enter your choice: "
    );

    scanf("%d", &choice);
    getchar();

    char in_file[MAX_FILE_LEN] = {0};

    switch (choice) {
      case 1:
        good_file = false;

        /* read filename and open file */

        printf("\nEnter the input file name: ");
        scanf("%49[^\n]", in_file);

        /* run read_stops() and print result */

        int num_stops = read_stops(in_file);
        printf("\nread_stops() returned %d\n\n", num_stops);

        if (num_stops >= 0) {
          good_file = true;
        }
        break;

      case 2:
        if (!good_file) {
          printf(
            "\ng_stop_array is not loaded. Use read_stops() first!\n\n"
          );
          break;
        }

        /* run find_hubs() and print result */

        int num_hubs = find_hubs();
        printf("\nfind_hubs() returned %d\n\n", num_hubs);
        break;

      case 3:
        if (!good_file) {
          printf(
            "\ng_stop_array is not loaded. Use read_stops() first!\n\n"
          );
          break;
        }

        printf("The following stops were loaded:\n");
        for (int i = 0; i < g_stop_count; i++) {
          printf("\nStop %d:\n", i + 1);
          print_stop(&g_stop_array[i]);
        }
        printf("\n");
        break;

      case 4:
        printf("\nGoodbye!\n\n");
        return 0;

      default:
        printf("\nSelect one of the given options...!\n\n");
        break;
    }
  }

  return 0;
} /* main() */

/* vim: set ft=c ts=4 sw=2 et : */
