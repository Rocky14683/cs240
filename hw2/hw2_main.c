
/*
 * Homework 2 main program
 * CS 240, Fall 2024
 * Purdue University
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hw2.h"

/* Helper function declarations */

static int read_int(const char *);
static void read_line(const char *restrict, char *restrict, size_t);

/*
 * This function is used to run the functions implemented in hw2.c.
 * User input is used to determine which function to run what input
 * is passed to it. Upon completion, the function returns zero.
 */

int main() {
  while (1) {
    int choice = 0;
    printf(
            "Select a function to test:\n"
            "    1) calculate_bill()\n"
            "    2) generate_monthly_water_report()\n"
            "    3) get_electricity_usage()\n"
            "    4) get_average_resident_water_usage()\n"
            "    5) Exit\n"
    );
    choice = read_int("Enter your choice: ");

    /* Variables to store input parameters in */

    int month = 0;
    int year = 0;
    char dorm[MAX_NAME_LENGTH] = {0};
    char in_file[MAX_FILE_LENGTH] = {0};
    char out_file[MAX_FILE_LENGTH] = {0};

    if ((choice >= 1) && (choice <= 4)) {
      read_line("Enter the input file name: ", in_file, sizeof(in_file));
    }

    switch (choice) {
      case 1:    /* calculate_bill() */
        read_line("Enter the dorm name: ", dorm, sizeof(dorm));
        double bill = calculate_bill(in_file, dorm);
        printf(
                "\ncalculate_bill(%s, %s) returned %f\n\n",
                in_file, dorm, bill
        );
        break;

      case 2:    /* generate_monthly_water_report() */
        read_line("Enter the dorm name: ", dorm, sizeof(dorm));
        year = read_int("Enter the year: ");
        read_line("Enter the output file name: ", out_file, sizeof(out_file));
        int err_code = generate_monthly_water_report(
                in_file, dorm, year, out_file
        );
        printf(
                "\ngenerate_monthly_water_report(%s, %s, %d, %s) "
                "returned %d\n", in_file, dorm, year, out_file, err_code
        );
        if (err_code < 0) {
          printf(
                  "Consider opening the output file to see its"
                  "contents.\n\n"
          );
        }
        break;

      case 3:    /* get_electricity_usage() */
        read_line("Enter the dorm name: ", dorm, sizeof(dorm));
        year = read_int("Enter the year: ");
        int electricity = get_electricity_usage(in_file, dorm, year);
        printf(
                "\nget_electricity_usage(%s, %s, %d) returned %d\n\n",
                in_file, dorm, year, electricity
        );
        break;

      case 4:    /* get_average_resident_water_usage() */
        year = read_int("Enter the year: ");
        month = read_int("Enter the month: ");

        float water = get_average_resident_water_usage(in_file, month, year);
        printf(
                "\nget_average_resident_water_usage(%s, %d, %d) returned %f\n\n",
                in_file, month, year, water
        );
        break;

      case 5:    /* Exit */
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

int read_int(const char *prompt) {
  do {
    int num = 0;
    fputs(prompt, stdout);
    scanf("%d", &num);
    if (getchar() == '\n') {
      return num;
    }
    else {
      while (getchar() != '\n') {
      }
      printf("Invalid choice. Try again.\n");
    }
  } while (1);
} /* read_int() */

/*
 * Prints the given prompt, then reads a line from standard input, and stores it
 * in the given buffer. Truncates the stored string to the given size, including
 * the NUL terminator.
 */

void read_line(const char *restrict prompt, char *restrict buf, size_t size) {
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
