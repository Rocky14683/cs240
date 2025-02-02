/*
 * Homework 3 main program
 * CS 240, Fall 2024
 * Purdue University
 */

#include <stdio.h>
#include <string.h>

#include "hw3.h"

/* Helper function forward declarations */

static void print_return_float_value(float, const char *);
static void print_return_value(int, const char *);
static int  read_int(const char *);
static void read_line(const char *restrict, char *restrict, size_t);

/*
 * Print the return value of a function which returns an integer.
 */

void print_return_value(int val, const char *function) {
  printf("The value returned by the %s() was: %d\n", function, val);
} /* print_return_value() */

/*
 * Print the return value of a function which returns a float.
 */

void print_return_float_value(float val, const char *function) {
  printf("The value returned by the %s() was: %f\n", function, val);
} /* print_return_float_value() */

/*
 * This function is used to run the functions defined in hw1.c.
 * User input is used to determine the inputs to the functions.
 * Upon completion, returns 0.
 */

int main() {
  int choice = 0;
  char file[1000] = {'\0'};
  char astronaut[MAX_NAME_LEN] = {'\0'};
  char mission_type[MAX_NAME_LEN] = {'\0'};
  int val = 0;
  float float_val = 0.0;

  /* Main loop */

  while (1) {
    printf("-----------------------------------------\n");
    printf("0) Quit\n"
           "1) read_tables()\n"
           "2) get_total_time()\n"
           "3) highest_success_mission()\n"
           "4) avg_mission_duration()\n"
           "5) longest_mission_astronaut()\n"
           "6) popular_equipment_pair()\n"
           "7) calc_efficiency_score()\n"
           "8) generate_report()\n\n");
    choice = read_int("Select a function: ");

    /* Prompt for file name when needed */

    if ((choice == 1) || (choice == 8)) {
      char *prompt = "Enter the output file name: ";
      if (choice == 1) {
        prompt = "Enter the input file name: ";
      }
      read_line(prompt, file, sizeof(file));
    }

    /* Prompt for astronaut name when needed */

    if ((choice == 2) || (choice == 7) || (choice == 8)) {
      read_line("Enter the astronaut's name: ", astronaut, sizeof(astronaut));
    }

    /* Prompt for mission type whene needed */

    if (choice == 4) {
      read_line("Enter the desired mission type: ",
                mission_type, sizeof(mission_type));
    }

    /* Call appropriate functions based on user selection */

    switch (choice) {
      case 0:
        printf("Goodbye.\n");
        return 0;

      case 1:
        val = read_tables(file);
        print_return_value(val, "read_tables");
        break;

      case 2:
        val = get_total_time(astronaut);
        print_return_value(val, "get_total_time");
        break;

      case 3:
        val = highest_success_mission();
        print_return_value(val, "highest_success_mission");
        break;

      case 4:
        float_val = avg_mission_duration(mission_type);
        print_return_float_value(float_val, "avg_mission_duration");
        break;

      case 5:
        printf("The astronaut with the longest mission is: %s\n",
               longest_mission_astronaut());
        break;

      case 6:
        val = popular_equipment_pair();
        print_return_value(val, "popular_equipment_pair");
        break;

      case 7:
        float_val = calc_efficiency_score(astronaut);
        print_return_float_value(float_val, "calc_efficiency_score");
        break;

      case 8:
        val = generate_report(file, astronaut);
        print_return_value(val, "generate_report");
        break;

      default:
        printf("Invalid choice. Please select one of the options.\n");
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
      printf("Invalid input. Try again.\n");
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
