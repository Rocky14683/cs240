/*
 * Homework 5 main program
 * CS 240, Fall 2024
 * Purdue University
 */

#include <string.h>
#include <stdio.h>

#include "hw5.h"

/* Helper function declarations */

static void decompose_date(int, int *, int *, int *);
static void print_item(item_t);
static void print_return_code(int, const char *);
static int read_int(const char *);
static item_t read_item_from_file(const char *);
static void read_line(const char *restrict, char *restrict, size_t);
static void read_routes(char[N_ROUTES][MAX_ROUTE_LEN]);

/*
 * This function is used to run the different functions implemented in file
 * hw5.c. User input is used to determine which function to run and what input
 * is passed to it. Upon completion, the function returns zero.
 */

int main() {
  char database_file[100] = {'\0'};
  FILE *database_fp = NULL;

  read_line("Enter the name of the input file to use: ",
            database_file, sizeof(database_file));

  database_fp = fopen(database_file, "rb+");
  if (database_fp == NULL) {
    printf("Unable to open %s for input.\n", database_file);
    return 1;
  }

  printf("\n\nFor functions that require an item_t as an input,\n"
         "please create a regular text file of the item with fields separated\n"
         "by newlines.\n"
         "See read_item_from_file() in hw5_main.c for more info.\n\n");

  while (1) {

    printf("\nOPTIONS:\n"
           "0) Quit\n"
           "1) read_item()\n"
           "2) write_item()\n"
           "3) add_item()\n"
           "4) decommission_category()\n"
           "6) find_item_by_id()\n"
           "5) find_optimal_route()\n"
           "7) find_expired_items()\n"
           "8) mark_used()\n"
           "9) balance_stations()\n");
    int choice = read_int("Select a function: ");
    printf("\n");

    char category[MAX_NAME_LEN] = {0};
    char destination = '\0';
    char item_file[100] = {'\0'};
    char routes[N_ROUTES][MAX_ROUTE_LEN] = {'\0'};
    float return_value = 0.0;
    int date = 0;
    int item_id = 0;
    int position = 0;
    int return_code = 0;
    item_t item = BAD_ITEM;

    switch (choice) {
      case 0: /* Quit */

        printf("Goodbye!\n\n");
        return 0;

      case 1: /* read_item() */

        position = read_int("Enter item position: ");
        item = read_item(database_fp, position);
        printf("read_item() returned the following item:\n");
        print_item(item);
        break;

      case 2: /* write_item() */

        position = read_int("Enter item position: ");
        read_line("Enter a file containing an item representation: ",
                  item_file, sizeof(item_file));
        item = read_item_from_file(item_file);
        printf("Read the following item information:\n");
        print_item(item);
        return_code = write_item(database_fp, item, position);
        print_return_code(return_code, "write_item");
        break;

      case 3: /* add_item() */

        read_line("Enter a file containing an item representation: ",
                  item_file, sizeof(item_file));
        item = read_item_from_file(item_file);
        printf("Read the following item information:\n");
        print_item(item);
        return_code = add_item(database_fp, item);
        print_return_code(return_code, "add_item");
        break;

      case 4: /* decommission_category() */

        read_line("Enter category superset: ", category, sizeof(category));
        return_code = decommission_category(database_fp, category);
        print_return_code(return_code, "send_item");
        break;

      case 5: /* find_optimal_route() */

        read_line("Enter a file containing an item representation: ",
                  item_file, sizeof(item_file));
        item = read_item_from_file(item_file);
        printf("Enter a destination station code: ");
        scanf("%c%*[^\n]%*c", &destination);
        read_routes(routes);
        return_code = find_optimal_route(item, destination, routes);
        print_return_code(return_code, "find_optimal_route");
        break;

      case 6: /* find_item_by_id() */

        item_id = read_int("Enter an item ID: ");
        item = find_item_by_id(database_fp, item_id);
        printf("find_item_by_id() returned the following item:\n");
        print_item(item);
        break;

      case 7: /* find_expired_items() */

        date = read_int(
                "Enter a value for the current date, in format MMDDYYYY: "
        );
        return_code = find_expired_items(database_fp, date);
        printf("find_expired_items() returned: %d", return_code);
        break;

      case 8: /* mark_used() */

        item_id = read_int("Enter an item ID: ");
        return_code = mark_used(database_fp, item_id);
        print_return_code(return_code, "mark_used");
        break;

      case 9: /* balance_stations() */

        return_value = balance_stations(database_fp);
        printf("balance_stations() returned %f\n", return_value);
        break;

      default:

        printf("\nInvalid selection.\n");
        break;
    }
  }

  return 0;
} /* main() */


/*
 * Function to read in information about an item
 */

item_t read_item_from_file(const char *file_name) {
  FILE *fp = fopen(file_name, "r");
  item_t item = BAD_ITEM;
  if (fp == NULL) {
    return BAD_ITEM;
  }
  if (fscanf(fp, "%d\n", &item.id) != 1) {
    return BAD_ITEM;
  }

  if (fscanf(fp, "%c\n", &item.station_code) != 1) {
    return BAD_ITEM;
  }

  if (fscanf(fp, "%19[^\n]\n", item.name) != 1) {
    return BAD_ITEM;
  }
  if (fscanf(fp, "%19[^\n]\n", item.category) != 1) {
    return BAD_ITEM;
  }
  if (fscanf(fp, "%d\n", (int *) &item.status) != 1) {
    return BAD_ITEM;
  }
  if (fscanf(fp, "%f\n", &item.mass) != 1) {
    return BAD_ITEM;
  }
  if (fscanf(fp, "%d\n", &item.manufacture_date) != 1) {
    return BAD_ITEM;
  }
  return item;
} /* read_item_from_file() */


/*
 * Function to print item information
 */

void print_item(item_t item) {
  printf("ID: %d\nStation code: %c\nName: %s\nCategory: %s\n",
         item.id, item.station_code, item.name, item.category);
  switch (item.status) {
    case AVAILABLE:
      printf("Status: AVAILABLE\n");
      break;
    case IN_USE:
      printf("Status: IN_USE\n");
      break;
    case EXPIRED:
      printf("Status: EXPIRED\n");
      break;
    default:
      printf("Status: %d\n", item.status);
      break;
  }
  printf("Mass: %.2f\n", item.mass);

  int month = 0;
  int day = 0;
  int year = 0;
  decompose_date(item.manufacture_date, &month, &day, &year);
  printf("Manufacture date: %08d (%d/%d/%04d)\n",
         item.manufacture_date, month, day, year);
} /* print_item() */


/*
 * Function to print a function's return value, including the name of the error
 * code it may have returned. First argument is the return value, second is the
 * function name.
 */

void print_return_code(int val, const char *function) {
  printf("%s() returned: %d ", function, val);
  switch (val) {
    case OK:
      printf("(OK)\n");
      break;
    case NO_ITEM:
      printf("(NO_ITEM)\n");
      break;
    case WRITE_ERR:
      printf("(WRITE_ERR)\n");
      break;
    case NO_SPACE:
      printf("(NO_SPACE)\n");
      break;
    case MALFORMED_ITEM:
      printf("(MALFORMED_ITEM)\n");
      break;
    case NO_ROUTE:
      printf("(NO_ROUTE)\n");
      break;
    case ALREADY_BALANCED:
      printf("(ALREADY_BALANCED)\n");
      break;
    default:
      printf("\n");
      break;
  }
} /* print_return_code() */

/*
 * Function to decompose MMDDYYYY date into separate components.
 * First argument is the date to decompose, the remaining arguments are pointers
 * to variables in which to store the month, day, and year, respectively.
 */

void decompose_date(int date, int *month, int *day, int *year) {
  *year = date % 10000;
  *month = (date / 1000000) % 100;
  *day = (date / 10000) % 100;
} /* decompose_date() */

/*
 * Prints the prompt, then reads a line containing a single integer from
 * standard input. Repeats the prompt/input stage until a valid input is read.
 * Returns the integer read.
 */

int read_int(const char *prompt) {
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

void read_line(const char *restrict prompt, char *restrict buf, size_t size) {
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
 * Prompt the user for several routes, reading them into the given 2-D character
 * array of routes. Empty characters/routes are filled with NUL bytes.
 */

void read_routes(char routes[N_ROUTES][MAX_ROUTE_LEN]) {
  printf("\nYou will now be asked to enter up to %d routes, "
         "each up to %d characters.\n",
         N_ROUTES, MAX_ROUTE_LEN);
  printf("Enter an empty line to leave the remaining routes empty.\n");
  for (int i = 0; i < N_ROUTES; i++) {
    char tmp_route[MAX_ROUTE_LEN + 1] = { '\0' };
    read_line("Enter route: ", tmp_route, sizeof(tmp_route));
    if (tmp_route[0] == '\0') {
      /* If empty route entered, zero the rest of the routes and stop. */

      for (int j = i; j < N_ROUTES; j++) {
        memset(routes[j], 0, MAX_ROUTE_LEN);
      }
      break;
    }

    /* Use memcpy() instead of strcpy() because the remaining bytes need
     * to be set to 0, which strcpy() won't do. */

    memcpy(routes[i], tmp_route, MAX_ROUTE_LEN);
  }
} /* read_routes() */

/* vim: ts=4 sw=2 et */
