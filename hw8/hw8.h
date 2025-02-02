#ifndef HW8_H
#define HW8_H

#include <stdio.h>

/* Constant definitions */

/* Error codes */

#define SUCCESS      (0)
#define NOT_FOUND    (-1)
#define INVALID_MOVE (-2)

/* Structure Declarations */

typedef struct ingredient_struct {
  char *name;
  double weight;
  char *source;
  struct ingredient_struct *next;
} ingredient_t;

typedef struct order_struct {
  char *name;
  int order_id;
  int ingredients_needed;
  struct ingredient_struct *ingredient_list;
  struct order_struct *next;
} order_t;

/* Function prototypes */
ingredient_t *create_ingredient(char *, char *, double);
order_t *create_order(char *, int, int);
int stock_ingredient(order_t *, int, ingredient_t *);
ingredient_t *unstock_ingredient(order_t *, ingredient_t *);
int move_ingredient(order_t *, int, int, ingredient_t *);
ingredient_t * prune_source(order_t *, char *);
order_t *find_heaviest_order(order_t *);
int delete_ingredient(order_t *, int, ingredient_t *);
order_t *cook_orders(order_t *);
order_t *cancel_order(order_t *, int);

#endif // HW8_H
