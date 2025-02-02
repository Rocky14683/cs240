/* Includes */

#include "hw8.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Function Prototypes */

void print_ingredients(ingredient_t *);
void print_orders(order_t *);
int clean_stdin();
static int read_int(const char *);
static void read_line(const char *restrict, char *restrict, size_t);
static double read_double(const char *);
static void *read_ptr(const char *);

/* Define Statements */
#define MAX_BUF_LEN (1024)

/*
 * This function is used to run the different functions implemented in file
 * hw8.c. User input is used to determine which function to run and what input
 * is passed to it. Upon completion, the function returns zero.
 */

int main() {

  //Disable buffering to suppress memory leaks
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  setvbuf(stdin, NULL, _IONBF, 0);

  int result = 0;
  int choice = 0;

  ingredient_t *ingredient_p = NULL;
  order_t *order_p = NULL;
  char order_name[MAX_BUF_LEN] = {'\0'};
  char source[MAX_BUF_LEN] = {'\0'};
  char ingredient_name[MAX_BUF_LEN] = {'\0'};

  // list of orders
  order_t *orders = NULL;

  // init variables
  double weight = 0.0;
  int ingredients_needed = 0;
  int order_id = 0;

  // move ingredient variables
  int from_id = 0;
  int to_id = 0;

  while (1) {
    printf("\nOPTIONS:\n"
           "0) Quit\n"
           "1) create_ingredient()\n"
           "2) create_order()\n"
           "3) stock_ingredient()\n"
           "4) unstock_ingredient()\n"
           "5) move_ingredient()\n"
           "6) prune_source()\n"
           "7) find_heaviest_order()\n"
           "8) delete_ingredient()\n"
           "9) cook_orders()\n"
           "10) cancel_order()\n"
           "11) print_orders()\n"
          );

    choice = read_int("Select a function: ");

    switch (choice) {
      case 0: // quit
        printf("\nGoodbye!\n\n");
        return 0;

      case 1: // create ingredient

        read_line("Enter ingredient name: ", ingredient_name, MAX_BUF_LEN);
        read_line("Enter source: ", source, MAX_BUF_LEN);
        weight = read_double("Enter the weight: ");

        ingredient_p = create_ingredient(ingredient_name, source,
                                         weight);
        printf("Created ingredient:\n");
        print_ingredients(ingredient_p);
        break;

      case 2: // create order

        read_line("Enter order name: ", order_name, MAX_BUF_LEN); 
        order_id = read_int("Enter order id: ");
        ingredients_needed = read_int("Enter number of ingredients needed: ");

        order_p = create_order(order_name, order_id,
                                        ingredients_needed);
        order_p->next = orders;
        orders = order_p;
        break;

      case 3: // stock ingredient

        order_id = read_int("Enter order id: ");
        ingredient_p = (ingredient_t *) read_ptr("Enter pointer to ingredient to add: ");

        result = stock_ingredient(orders, order_id, ingredient_p);
        printf("The value returned by stock_ingredient() was %d\n", result);
        break;

      case 4: // unstock ingredient

        order_p = (order_t *) read_ptr("Enter pointer to order to remove ingredient from: ");
        ingredient_p = (ingredient_t *) read_ptr("Enter pointer to ingredient to remove: ");

        ingredient_t *ret = unstock_ingredient(order_p, ingredient_p);
        printf("Ingredient returned by unstock_ingredient():\n");
        print_ingredients(ret);
        break;

      case 5: // move ingredient

        from_id = read_int("Enter order id of order to move ingredient from: ");
        to_id = read_int("Enter order id of order to move ingredient to: ");
        ingredient_p = read_ptr("Enter pointer to ingredient to move: ");

        result = move_ingredient(orders, from_id, to_id, ingredient_p);
        printf("The value returned by move_ingredient() was %d\n", result);
        break;

      case 6: // prune source

        read_line("Enter source to prune: ", source, MAX_BUF_LEN);
        ingredient_t *ingredients_pruned = prune_source(orders, source);
        printf("Ingredients pruned:\n");
        print_ingredients(ingredients_pruned);
        break;

      case 7: // heaviest order

        order_p = find_heaviest_order(orders);
        printf("\nHeaviest order:\n");
        print_orders(order_p);
        break;

      case 8: // delete ingredient

        order_id = read_int("Enter order id of order to delete from: ");
        ingredient_p = read_ptr("Enter pointer to ingredient to delete: ");

        result = delete_ingredient(orders, order_id, ingredient_p);
        printf("The value returned by delete_ingredient() is %d\n", result);
        break;

      case 9: // cook orders
        orders = cook_orders(orders);
        break;

      case 10: // cancel order

        order_id = read_int("Enter order id to cancel: ");
        orders = cancel_order(orders, order_id);
        break;

      case 11: // print all orders and their stocked ingredients
        print_orders(orders);
        break;

      default:
        printf("\nInvalid input! Try again...\n");
        break;
    } /* switch (choice) */
  } /* while (1) */
  return 0;
} /* main() */

/*
 * Function to print list of ingredients
 */

void print_ingredients(ingredient_t * list) {
  ingredient_t * temp = list;
  while (temp != NULL) {
    printf("    at %p\n", temp);
    printf("    %s: %lf kg, from %s\n\n", temp->name, temp->weight, temp->source);
    temp = temp->next;
  }
  printf("    (NULL)\n\n");
} /* print_ingredient() */

/*
 * Function to print a list of orders/orders
 */

void print_orders(order_t * list) {
  order_t * temp = list;
  while (temp != NULL) {
    printf("at %p\n", temp);
    printf("%s:\n  ID: %d\n  ingredients needed: %d\n\n", temp->name,
           temp->order_id, temp->ingredients_needed);
    print_ingredients(temp->ingredient_list);
    temp = temp->next;
  }
  printf("(NULL)\n");
} /* print_order() */

/*
 * This function is used to handle the input reading
 */

int clean_stdin() {
  while (getchar() != '\n') {
  }
  return 1;
} /* clean_stdin() */

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

