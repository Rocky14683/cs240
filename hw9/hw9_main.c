/*
 * Homework 9 main program
 * CS 240, Fall 2024
 * Purdue University
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hw9.h"

/*
 * This macro acts like printf(3), but prefixes each line with the number of
 * tabs specified by the "tabs" variable in the caller's scope. Each call must
 * print a single line, as the tabs are printed before each underlying printf(3)
 * call, not before each individual line.
 *
 * According to the code standard, constants must have uppercase names, but
 * nothing disallows lowercase macros.
 */

#define printf_indented(...) \
  do { \
    for (int i = 0; i < (tabs); i++) { \
      printf("\t"); \
    } \
    printf(__VA_ARGS__); \
  } while (0)

#define CC_RESET         "\x1b[m"
#define CC_BOLD_YELLOW   "\x1b[1;33m"
#define CC_CYAN          "\x1b[36m"
#define CC_UNDERLINE_ON  "\x1b[21m"
#define CC_UNDERLINE_OFF "\x1b[24m"
#define CANCEL_MESSAGE \
  CC_CYAN "Entering " CC_UNDERLINE_ON "0" CC_UNDERLINE_OFF \
  " will cancel and return to the previous menu." CC_RESET "\n"

/* Helper functions. */

static void print_planet(const planet_t *, bool, unsigned int);
static void print_resource(const resource_t *, unsigned int);
static void print_spaceship(const spaceship_t *, unsigned int);
static int read_int(const char *);
static void read_line(const char *restrict, char *restrict, size_t);
static void *read_ptr(const char *);
static double read_double(const char *);

/*
 * Main program function. Tests the functions defined in hw9.c using input from
 * the user.
 */

int main() {
  planet_t *planets_head = NULL;

  for (;;) {
    printf("\nSelect an option:\n"
           "   --\n"
           "   0) Exit\n"
           "   --\n"
           "   1) create_planet()\n"
           "   2) add_resource()\n"
           "   3) add_planet()\n"
           "   4) destroy_planet()\n"
           "   5) remove_resource()\n"
           "   6) move_spaceship()\n"
           "   7) merge_resources()\n"
           "   8) rotate_planets()\n"
           "   9) find_planet()\n"
           "  10) apocalypse()\n"
           "   --\n"
           "  11) Print planet list\n"
           "  12) Print planet at address\n"
           "  13) Print resource at address\n"
           "  14) Print spaceship at address\n"
           "  15) Add spaceship to planet\n"
           "   --\n");

    int choice = read_int("Enter your choice: ");
    printf("\n");

    /* Use if/else instead of switch/case because of variable scoping. */

    if (choice == 0) { /* Exit */
      break;
    }
    else if (choice == 1) { /* create_planet() */
      char name[BUFSIZ] = {'\0'};
      int id = read_int("Enter new planet ID: ");
      read_line("Enter new planet name: ", name, sizeof(name));
      planet_t *planet = NULL;
      create_planet(name, id, &planet);
      printf("\ncreate_planet() returned the following planet:\n");
      print_planet(planet, false, 1);
    }
    else if (choice == 2) { /* add_resource() */
      planet_t *planet =
        read_ptr("Enter the address of the planet to add to.\n"
                 CANCEL_MESSAGE "Address of planet: ");
      if (planet) {
        char name[BUFSIZ] = {'\0'};
        read_line("Enter new resource name: ", name, sizeof(name));
        double volume = read_double("Enter new resource volume: ");
        int dollars = read_int("Enter new resource value, dollar part: ");
        int cents = read_int("Enter new resource value, cents part: ");
        add_resource(planet, name, volume, dollars, cents);
        printf("\nThe planet now looks as follows:\n");
        print_planet(planet, true, 1);
      }
    }
    else if (choice == 3) { /* add_planet() */
      planet_t *new =
        read_ptr("Enter the address of the planet to add.\n"
                 CANCEL_MESSAGE "Address of planet: ");
      if (new) {
        add_planet(&planets_head, new);
        printf("add_planet() set the last argument to %p\n", planets_head);
      }
    }
    else if (choice == 4) { /* destroy_planet() */
      planet_t *planet =
        read_ptr("Enter the address of the planet to remove.\n"
                 CANCEL_MESSAGE "Address of planet: ");
      if (planet) {
        destroy_planet(&planet);
        printf("add_planet() set the last argument to %p\n", planet);
        planets_head = planet;
      }
    }
    else if (choice == 5) { /* remove_resource() */
      planet_t *planet =
        read_ptr("Enter the address of the planet to remove from.\n"
                 CANCEL_MESSAGE "Address of planet: ");
      if (planet) {
        char name[BUFSIZ];
        read_line("Enter name of resource to remove: ", name, sizeof(name));
        remove_resource(planet, name);
        printf("\nThe planet now looks as follows:\n");
        print_planet(planet, true, 1);
      }
    }
    else if (choice == 6) { /* move_spaceship() */
      spaceship_t *ship =
        read_ptr("Enter the address of the spaceship to move.\n"
            CANCEL_MESSAGE "Address of spaceship: ");
      if (!ship) {
        continue;
      }
      int planet_id = read_int("Enter ID of planet to move to: ");
      planet_t *dest_planet = NULL;
      int result = move_spaceship(ship, planet_id, &dest_planet);
      printf("move_spaceship() returned %d and set the last argument to %p\n",
             result, dest_planet);
    }
    else if (choice == 7) { /* merge_resources() */
      planet_t *first =
        read_ptr("Enter the address of the first planet to merge.\n"
                 CANCEL_MESSAGE "Address of planet: ");
      if (!first) {
        continue;
      }
      planet_t *second =
        read_ptr("Enter the address of the first planet to merge.\n"
                 CANCEL_MESSAGE "Address of planet: ");
      if (!second) {
        continue;
      }
      int result = merge_resources(first, second);
      printf("merge_resources() returned %d\n", result);
    }
    else if (choice == 8) { /* rotate_planets() */
      /* Store the initial order of the list as an array of the planets' IDs. */

      size_t n_planets = 0;
      for (planet_t *cur = planets_head; cur; cur = cur->next_planet) {
        n_planets++;
      }
      int *before_ids = calloc(n_planets, sizeof(*before_ids));
      size_t i = 0;
      for (planet_t *cur = planets_head; cur; cur = cur->next_planet) {
        before_ids[i++] = cur->planet_id;
      }

      /* Perform rotation */

      int steps = read_int("Enter amount to rotate list by: ");
      int result = rotate_planets(&planets_head, steps);
      printf("rotate_planets() returned %d\n\n", result);

      /* Print list before */

      printf("Planet list order before rotating (by planet ID):\n");
      for (i = 0; i < n_planets; i++) {
        printf("%d", before_ids[i]);
        if (i != (n_planets - 1)) {
          printf(" <> ");
        }
      }
      printf("\n");

      /* Print list after */

      printf("\nPlanet list order after rotating (by planet ID):\n");
      for (planet_t *cur = planets_head; cur; cur = cur->next_planet) {
        printf("%d", cur->planet_id);
        if (cur->next_planet) {
          printf(" <> ");
        }
      }
      printf("\n");
    }
    else if (choice == 9) { /* find_planet() */
      spaceship_t *ship =
        read_ptr("Enter the address of the spaceship to move.\n"
            CANCEL_MESSAGE "Address of spaceship: ");
      if (ship) {
        planet_t *result = find_planet(planets_head, ship);
        printf("find_planet() returned %p\n", result);
      }
    }
    else if (choice == 10) { /* apocalypse() */
      apocalypse(&planets_head);
      printf("apocalypse() set the argument to %p\n", planets_head);
    }
    else if (choice == 11) { /* Print planet list */
      printf(planets_head ? "Planet list:\n" : "Planet list is empty.\n");
      for (planet_t *cur = planets_head; cur; cur = cur->next_planet) {
        print_planet(cur, false, 1);
      }
    }
    else if (choice == 12) { /* Print planet at address */
      planet_t *planet = read_ptr("Enter planet address: ");
      print_planet(planet, true, 1);
    }
    else if (choice == 13) { /*  Print resource at address */
      resource_t *resource = read_ptr("Enter resource address: ");
      print_resource(resource, 1);
    }
    else if (choice == 14) { /*  Print spaceship at address */
      spaceship_t *spaceship = read_ptr("Enter spaceship address: ");
      print_spaceship(spaceship, 1);
    }
    else if (choice == 15) { /* Add spaceship to planet */
      printf(CC_BOLD_YELLOW "Warning" CC_RESET ": "
             "The assignment does not include a function to add a\n"
             "         spaceship to a planet in the correct order, so this\n"
             "         function will just create a temporary planet with a\n"
             "         single spaceship and call move_spaceship() to move it\n"
             "         to the desired planet.\n\n");

      /* Create spsaceship */

      char name[BUFSIZ];
      int ship_id = read_int("Enter the new spaceship ID: ");
      read_line("Enter new spaceship name: ", name, sizeof(name));
      double fuel = read_double("Enter new spaceship fuel capacity: ");
      spaceship_t *ship = calloc(1, sizeof(*ship));
      ship->spaceship_id = ship_id;
      ship->fuel_capacity = fuel;
      ship->name = malloc(strlen(name) + 1);
      strcpy(ship->name, name);

      printf("\n");
      int planet_id = read_int("Enter the ID of the planet to add to: ");

      /* Create temporary planet and just use move_spaceship() to add. */

      planet_t *from = calloc(1, sizeof(*from));
      from->spaceship_list = ship;
      if (planets_head) {
        from->next_planet = planets_head;
      }
      planets_head->prev_planet = from;
      if (planets_head) {
        planets_head->prev_planet = from;
      }

      planet_t *dest_planet = from;
      int result = move_spaceship(ship, planet_id, &dest_planet);
      printf("move_spaceship() returned %d and set the last argument to %p\n",
             result, dest_planet);

      /* Clean up temporary planet */

      if (planets_head) {
        planets_head->prev_planet = NULL;
      }
      free(from);
      from = NULL;
    }
    else { /* Invalid option */
      printf("Please enter a valid option.\n");
    }
  }
  
  return 0;
} /* main() */

/*
 * Print a planet_t's information to stdout. Indents output using the number of
 * tabs specified by the third argument. If the second argument is true, prints
 * the planet's resources and spaceships.
 */

static void print_planet(const planet_t *planet, bool nested,
                         unsigned int tabs) {
  if (planet) {
    printf_indented("Address: %p\n", planet);
    printf_indented("ID: %d\n", planet->planet_id);
    printf_indented("Name: %s\n", planet->name);
    if (nested) {
      printf_indented("Spaceship list:%s\n",
                      planet->spaceship_list ? "" : " (null)");
      for (spaceship_t *cur = planet->spaceship_list;
           cur;
           cur = cur->next_ship) {
        print_spaceship(cur, tabs + 1);
      }
      printf_indented("Resource list:%s\n",
                      planet->resource_list ? "" : " (null)");
      for (resource_t *cur = planet->resource_list;
           cur;
           cur = cur->next_resource) {
        print_resource(cur, tabs + 1);
      }
    }
    else {
      printf_indented("Spaceship list: %p\n", planet->spaceship_list);
      printf_indented("Resource list: %p\n", planet->resource_list);
    }
    printf_indented("Previous planet: %p\n", planet->prev_planet);
    printf_indented("Next planet: %p\n", planet->next_planet);
    printf("\n");
  }
  else {
    printf("(null)\n");
  }
} /* print_planet() */

/*
 * Print a resource_t's information to stdout. Indents output using the number of
 * tabs specified by the second argument.
 */

static void print_resource(const resource_t *resource, unsigned int tabs) {
  if (resource) {
    printf_indented("Address: %p\n", resource);
    printf_indented("Name: %s\n", resource->name);
    printf_indented("Volume: %lf\n", resource->volume);
    printf_indented("Value: $%d.%d\n", resource->value.dollars,
                    resource->value.cents);
    printf_indented("Previous resource: %p\n", resource->prev_resource);
    printf_indented("Next resource: %p\n", resource->next_resource);
    printf("\n");
  }
  else {
    printf("(null)\n");
  }
} /* print_resource() */

/*
 * Print a spaceship_t's information to stdout. Indents output using the number of
 * tabs specified by the second argument.
 */

static void print_spaceship(const spaceship_t *spaceship, unsigned int tabs) {
  if (spaceship) {
    printf_indented("Address: %p\n", spaceship);
    printf_indented("ID: %d\n", spaceship->spaceship_id);
    printf_indented("Name: %s\n", spaceship->name);
    printf_indented("Fuel capacity: %lf\n", spaceship->fuel_capacity);
    printf_indented("Previous spaceship: %p\n", spaceship->prev_ship);
    printf_indented("Next spaceship: %p\n", spaceship->next_ship);
    printf("\n");
    /*print_spaceship_list(spaceship->spaceship_list, tabs + 1);*/
  }
  else {
    printf("(null)\n");
  }
} /* print_spaceship() */

#if 0
static void print_spaceship_list(const spaceship_t *head, unsigned int tabs) {
} /* print_spaceship_list() */
#endif

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
