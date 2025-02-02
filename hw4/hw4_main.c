/* Homework 4 Main function
 * CS 240 - Spring 2023
 * Last updated Jan 7, 2023
 */

#include "hw4.h"

#include <stdio.h>

static int read_int(const char *prompt);
static double read_double(const char *prompt);

/*
 *  This function is used to run the functions implemented in hw4.c.
 *  User input is used to determine which function to run and what
 *  input is passed to it. main() prints the return value of the
 *  selected function.
 */

int main() {
  while (1) {
    printf("\n\n");
    printf("0) Quit\n"
           "1) neg_complex(complex_t)\n"
           "2) inv_complex(complex_t)\n"
           "3) exp_complex(complex_t)\n"
           "4) julia(complex_t, complex_t)\n"
           "5) add_complex(complex_t, complex_t)\n"
           "6) sub_complex(complex_t, complex_t)\n"
           "7) mul_complex(complex_t, complex_t)\n"
           "8) div_complex(complex_t, complex_t)\n"
           "9) dot_complex(complex_t, complex_t)\n"
    );

    int choice = read_int("Select a function: ");

    complex_t comp_one = {0.0, 0.0};
    complex_t comp_two = {0.0, 0.0};
    complex_t result = {0.0, 0.0};
    double double_result = 0.0;
    int int_result = 0;

    // get the complex numbers
    if ((choice <= 9) && (choice >= 1)) {
      comp_one.x = read_double("Enter the x (real) component of the"
                               " complex number: ");
      comp_one.y = read_double("Enter the y (imaginary) component of"
                               " the complex number: ");
    }
    if ((choice >= 4) && (choice <= 9)) {
      comp_two.x = read_double("Enter the x (real) component of the"
                               " second complex number: ");
      comp_two.y = read_double("Enter the y (imaginary) component of"
                               " the second complex number: ");
    }

    switch (choice) {
      case 0: // quit
        printf("\nGoodbye!\n\n");
        return 0;

      case 1: // negate complex number
        result = neg_complex(comp_one);
        break;

      case 2: // invert complex number
        result = inv_complex(comp_one);
        break;

      case 3: // exponential of complex
        result = exp_complex(comp_one);
        break;

      case 4: // get julia
        int_result = julia(comp_one, comp_two);
        break;

      case 5: // add complex
        result = add_complex(comp_one, comp_two);
        break;

      case 6: // sub complex
        result = sub_complex(comp_one, comp_two);
        break;

      case 7: // mul complex
        result = mul_complex(comp_one, comp_two);
        break;

      case 8: // div complex
        result = div_complex(comp_one, comp_two);
        break;

      case 9: // dot complex
        double_result = dot_complex(comp_one, comp_two);
        break;

      default:
        printf("\nInvalid selection! Try again...");
        break;
    } /* switch (choice) */

    // print results
    if (choice == 4) {
      printf("The value returned was %d\n", int_result);
    }
    else if (choice == 9) {
      printf("The value returned was %f\n", double_result);
    }
    else {
      printf("The value returned was (%f, %f)\n", result.x, result.y);
    }
  } /* while (1) */

  /* not reached. */

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
      while (getchar() != '\n') {
      }
      printf("Invalid input. Try again.\n");
    }
  } while (1);
} /* read_int() */

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
