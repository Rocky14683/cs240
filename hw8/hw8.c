/*
 * Homework 8 Template
 * CS 240, Fall 2024
 */

#include "hw8.h"

#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

/*
 * Create an ingredient with the given name, source, and weight.
 */

ingredient_t *
create_ingredient(char *ingredient_name, char *source, double weight) {
  assert(ingredient_name != NULL);
  assert(source != NULL);
  assert(weight > 0);
  ingredient_t *ingredient = (ingredient_t *) malloc(sizeof(ingredient_t));
  assert(ingredient != NULL);
  ingredient->name = (char *) malloc(strlen(ingredient_name) + 1);
  assert(ingredient->name != NULL);
  strcpy(ingredient->name, ingredient_name);
  ingredient->source = (char *) malloc(strlen(source) + 1);
  assert(ingredient->source != NULL);
  strcpy(ingredient->source, source);
  ingredient->weight = weight;
  ingredient->next = NULL;
  return ingredient;
} /* create_ingredient() */

/*
 * Create an order with the given name, order_id, and ingredients_needed.
 */

order_t *create_order(char *order_name, int order_id, int ingredients_needed) {
  assert(order_name != NULL);
  assert(order_id > 0);
  assert(ingredients_needed >= 0);
  order_t *order = (order_t *) malloc(sizeof(order_t));
  assert(order != NULL);
  order->name = (char *) malloc(strlen(order_name) + 1);
  assert(order->name != NULL);
  strcpy(order->name, order_name);
  order->order_id = order_id;
  order->ingredients_needed = ingredients_needed;
  order->ingredient_list = NULL;
  order->next = NULL;

  return order;
} /* create_order() */

/*
 * Stock an ingredient in the given order.
 */

int stock_ingredient(order_t *orders, int order_id,
                     ingredient_t *ingredient_to_add) {
  assert(orders != NULL);
  assert(order_id > 0);
  assert(ingredient_to_add != NULL);
  bool found = false;

  order_t *cur_order = orders;
  while (cur_order != NULL) {
    if (cur_order->order_id == order_id) {
      found = true;
      break;
    }
    cur_order = cur_order->next;
  }

  if (!found) {
    return NOT_FOUND;
  }

  ingredient_t *cur_ingredient = cur_order->ingredient_list;
  while (cur_ingredient != NULL) {
    if (strcmp(cur_ingredient->name, ingredient_to_add->name) == 0) {
      return INVALID_MOVE;
    }
    cur_ingredient = cur_ingredient->next;
  }

  cur_ingredient = cur_order->ingredient_list;
  ingredient_t *prev_ingredient = NULL;
  while (cur_ingredient != NULL) {
    if (ingredient_to_add->weight >= cur_ingredient->weight) {
      break;
    }
    prev_ingredient = cur_ingredient;
    cur_ingredient = cur_ingredient->next;
  }

  if (prev_ingredient == NULL) {
    cur_order->ingredient_list = ingredient_to_add;
  }
  else {
    prev_ingredient->next = ingredient_to_add;
  }

  ingredient_to_add->next = cur_ingredient;
  cur_order->ingredients_needed--;

  return SUCCESS;
} /* stock_ingredient() */

/*
 * Unstock an ingredient from the given order.
 */

ingredient_t *unstock_ingredient(order_t *order, ingredient_t *ingredient) {
  assert(order != NULL);
  assert(ingredient != NULL);

  ingredient_t *cur_ingredient = order->ingredient_list;
  ingredient_t *prev_ingredient = NULL;
  while (cur_ingredient != NULL) {
    if (cur_ingredient == ingredient) {
      break;
    }
    prev_ingredient = cur_ingredient;
    cur_ingredient = cur_ingredient->next;
  }

  if (cur_ingredient == NULL) {
    return NULL;
  }

  if (prev_ingredient == NULL) {
    order->ingredient_list = cur_ingredient->next;
  }
  else {
    prev_ingredient->next = cur_ingredient->next;
  }


  cur_ingredient->next = NULL;
  order->ingredients_needed++;

  return cur_ingredient;
} /* unstock_ingredient() */

/*
 * Move an ingredient from one order to another.
 */

int move_ingredient(order_t *orders, int from_id, int to_id,
                    ingredient_t *ingredient) {
  assert(orders != NULL);
  assert(ingredient != NULL);
  assert(from_id > 0);
  assert(to_id > 0);

  if (from_id == to_id) {
    return INVALID_MOVE;
  }

  order_t *from_order = NULL;
  order_t *to_order = NULL;
  order_t *cur_order = orders;
  while (cur_order != NULL) {
    if (cur_order->order_id == from_id) {
      from_order = cur_order;
    }
    if (cur_order->order_id == to_id) {
      to_order = cur_order;
    }
    cur_order = cur_order->next;
  }

  if ((from_order == NULL) || (to_order == NULL)) {
    return NOT_FOUND;
  }

  ingredient_t *temp = to_order->ingredient_list;
  while (temp != NULL) {
    if (strcmp(ingredient->name, temp->name) == 0) {
      return INVALID_MOVE;
    }
    temp = temp->next;
  }

  ingredient_t *target_ingredient =
          unstock_ingredient(from_order, ingredient);

  if (target_ingredient == NULL) {
    ingredient_t* cur_ingredient = from_order->ingredient_list;
    while (cur_ingredient != NULL) {
      if (ingredient->name == cur_ingredient->name) {
        return INVALID_MOVE;
      }
      cur_ingredient = cur_ingredient->next;
    }
    return NOT_FOUND;
  }

  int result = stock_ingredient(to_order,
                                to_id, ingredient);

  return result;
} /* move_ingredient() */

/*
 * Prune all ingredients from the given source.
 */

ingredient_t *prune_source(order_t *orders, char *source) {
  assert(orders != NULL);
  assert(source != NULL);

  ingredient_t *ret_list = NULL;
  ingredient_t *ret_list_head = NULL;
  order_t *cur_order = orders;
  while (cur_order != NULL) {
    ingredient_t *cur_ingredient = cur_order->ingredient_list;

    while (cur_ingredient != NULL) {
      if (strcmp(source, cur_ingredient->source) == 0) {
        ingredient_t *next_ingredient = cur_ingredient->next;
        ingredient_t *removed = unstock_ingredient(
                cur_order, cur_ingredient);
        if (removed != NULL) {
          if (ret_list_head == NULL) {
            ret_list = removed;
            ret_list_head = ret_list;
          }
          else {
            ret_list->next = removed;
            ret_list = ret_list->next;
          }
        }
        cur_ingredient = next_ingredient;
      }
      else {
        cur_ingredient = cur_ingredient->next;
      }

    }

    cur_order = cur_order->next;
  }

  ingredient_t *cur = ret_list_head;
  while (cur != NULL) {
    cur = cur->next;
  }

  return ret_list_head;
} /* prune_source() */

/*
 * Find the order with the heaviest total weight of ingredients.
 */

order_t *find_heaviest_order(order_t *orders) {
  assert(orders != NULL);
  order_t *heaviest_order = NULL;
  double max_weight = -1.0;
  double cur_sum = 0.0;
  order_t *cur_order = orders;
  while (cur_order != NULL) {
    ingredient_t *cur_ingredient = cur_order->ingredient_list;
    cur_sum = 0.0;
    while (cur_ingredient != NULL) {
      cur_sum += cur_ingredient->weight;
      cur_ingredient = cur_ingredient->next;
    }

    if (cur_sum > max_weight) {
      max_weight = cur_sum;
      heaviest_order = cur_order;
    }
    cur_order = cur_order->next;
  }

  return heaviest_order;
} /* find_heaviest_order() */

/*
 * Delete an ingredient from the given order.
 */

int delete_ingredient(order_t *orders, int order_id, ingredient_t *ingredient) {
  assert(orders != NULL);
  assert(ingredient != NULL);
  assert(order_id > 0);

  order_t *cur_order = orders;
  while (cur_order != NULL) {
    if (cur_order->order_id == order_id) {
      break;
    }

    cur_order = cur_order->next;
  }

  if (cur_order == NULL) {
    return NOT_FOUND;
  }

  ingredient_t *cur_ingredient =
          unstock_ingredient(cur_order, ingredient);

  if (cur_ingredient == NULL) {
    return NOT_FOUND;
  }

  free(cur_ingredient->name);
  cur_ingredient->name = NULL;
  free(cur_ingredient->source);
  cur_ingredient->source = NULL;
  free(cur_ingredient);
  cur_ingredient = NULL;

  return SUCCESS;
} /* delete_ingredient() */

/*
 * Cook all orders that have all ingredients needed and remove it from orders.
 */

order_t *cook_orders(order_t *orders) {
  assert(orders != NULL);
  order_t *cur_order = orders;
  order_t *prev_order = NULL;
  while (cur_order != NULL) {
    if (cur_order->ingredients_needed == 0) {
      if (prev_order == NULL) {
        orders = cur_order->next;
      }
      else {
        prev_order->next = cur_order->next;
      }

      ingredient_t *cur_ingredient = cur_order->ingredient_list;
      while (cur_ingredient != NULL) {
        ingredient_t *next_ingredient = cur_ingredient->next;
        free(cur_ingredient->name);
        cur_ingredient->name = NULL;
        free(cur_ingredient->source);
        cur_ingredient->source = NULL;
        free(cur_ingredient);
        cur_ingredient = next_ingredient;
      }

      order_t *next_order = cur_order->next;
      free(cur_order->name);
      cur_order->name = NULL;
      free(cur_order);
      cur_order = next_order;

    }
    else {
      prev_order = cur_order;
      cur_order = cur_order->next;
    }
  }
  return orders;
} /* cook_orders() */

/*
 * Cancel an order with the given order_id.
 */

order_t *cancel_order(order_t *orders, int order_id) {
  assert(orders != NULL);
  assert(order_id > 0);
  order_t *cur_order = orders;
  order_t *prev_order = NULL;
  while (cur_order != NULL) {
    if (cur_order->order_id == order_id) {
      if (prev_order == NULL) {
        orders = cur_order->next;
      }
      else {
        prev_order->next = cur_order->next;
      }

      ingredient_t *cur_ingredient = cur_order->ingredient_list;
      while (cur_ingredient != NULL) {
        ingredient_t *next_ingredient = cur_ingredient->next;
        free(cur_ingredient->name);
        cur_ingredient->name = NULL;
        free(cur_ingredient->source);
        cur_ingredient->source = NULL;
        free(cur_ingredient);
        cur_ingredient = next_ingredient;
      }

      order_t *next_order = cur_order->next;
      free(cur_order->name);
      cur_order->name = NULL;
      free(cur_order);
      cur_order = next_order;

    }
    else {
      prev_order = cur_order;
      cur_order = cur_order->next;
    }
  }
  return orders;
} /* cancel_order() */
