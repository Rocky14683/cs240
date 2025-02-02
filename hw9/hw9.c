/* Add any includes here */
#include "hw9.h"


#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * sort resources by name
 */

resource_t *sort_resource(resource_t *head) {
  if (head == NULL) {
    return head;
  }
  resource_t *sorted = NULL;
  resource_t *curr = head;

  while (curr != NULL) {

    resource_t *next = curr->next_resource;

    if ((!sorted) || (strcmp(sorted->name, curr->name) >= 0)) {
      curr->next_resource = sorted;

      if (sorted != NULL) sorted->prev_resource = curr;

      sorted = curr;
      sorted->prev_resource = NULL;

    }
    else {

      resource_t *current_sorted = sorted;

      while ((current_sorted->next_resource) &&
             (strcmp(current_sorted->next_resource->name, curr->name) < 0)) {
        current_sorted = current_sorted->next_resource;
      }

      curr->next_resource = current_sorted->next_resource;

      if (current_sorted->next_resource != NULL) {
        current_sorted->next_resource->prev_resource = curr;
      }

      current_sorted->next_resource = curr;
      curr->prev_resource = current_sorted;
    }

    curr = next;
  }

  return sorted;
} /* sort_resource() */

/*
 * create a new planet
 */

void create_planet(char *name, int id, planet_t **planet_ptr) {
  assert(name != NULL);
  assert(planet_ptr != NULL);
  assert(*planet_ptr == NULL);
  assert(id > 0);
  planet_t *new_planet = malloc(sizeof(planet_t));
  assert(new_planet != NULL);

  new_planet->name = malloc(strlen(name) + 1);
  assert(new_planet->name != NULL);
  strcpy(new_planet->name, name);

  new_planet->planet_id = id;

  new_planet->resource_list = NULL;
  new_planet->spaceship_list = NULL;

  new_planet->prev_planet = NULL;
  new_planet->next_planet = NULL;

  (*planet_ptr) = new_planet;
} /* create_planet() */

/*
 * add a resource to a planet
 */

void add_resource(planet_t *planet, char *name, double volume, int dollar,
                  int cents) {
  assert(planet != NULL);
  assert(name != NULL);
  assert(volume >= 0);
  assert(dollar >= 0);
  assert(cents >= 0);
  resource_t *new_resource = malloc(sizeof(resource_t));
  assert(new_resource != NULL);

  new_resource->name = malloc(strlen(name) + 1);
  assert(new_resource->name != NULL);
  strcpy(new_resource->name, name);

  new_resource->volume = volume;

  new_resource->value.dollars = dollar;
  new_resource->value.cents = cents;

  new_resource->prev_resource = NULL;
  new_resource->next_resource = planet->resource_list;

  if (planet->resource_list != NULL) {
    planet->resource_list->prev_resource = new_resource;
  }

  planet->resource_list = new_resource;
} /* add_resource() */

/*
 * add a planet to the list of planets
 */

void add_planet(planet_t **planet_list, planet_t *planet) {
  assert(planet_list != NULL);
  assert(planet != NULL);

  planet_t *cur = *planet_list;
  planet_t *prev = NULL;

  if (cur == NULL) {
    *planet_list = planet;
    return;
  }

  while (cur != NULL) {
    if (strcmp(cur->name, planet->name) >= 0) {
      break;
    }
    prev = cur;
    cur = cur->next_planet;
  }

  if ((!cur) && (prev)) {
    prev->next_planet = planet;
    planet->prev_planet = prev;
    return;
  }

  planet->prev_planet = cur->prev_planet;
  planet->next_planet = cur;

  if (cur->prev_planet != NULL) {
    cur->prev_planet->next_planet = planet;
  }
  else {
    *planet_list = planet;
  }

  cur->prev_planet = planet;
} /* add_planet() */

/*
 * destroy a planet and free all resources
 */

void destroy_planet(planet_t **planet_ptr) {
  assert(planet_ptr);
  assert(*planet_ptr);
  printf("in destroy planet\n");

  planet_t *cur = (*planet_ptr);

  if ((*planet_ptr)->prev_planet != NULL) {
    (*planet_ptr)->prev_planet->next_planet = cur->next_planet;
  }

  if ((*planet_ptr)->next_planet != NULL) {
    (*planet_ptr)->next_planet->prev_planet = cur->prev_planet;
  }

  planet_t *prev = (*planet_ptr)->prev_planet;
  if (!prev) {
    *planet_ptr = cur->next_planet;
  }
  while (prev) {
    (*planet_ptr) = prev;
    prev = prev->prev_planet;
  }


  free(cur->name);
  cur->name = NULL;
  resource_t *cur_resource = cur->resource_list;
  while (cur_resource) {
    resource_t *next_resource = cur_resource->next_resource;
    free(cur_resource->name);
    cur_resource->name = NULL;
    free(cur_resource);
    cur_resource = NULL;
    cur_resource = next_resource;
  }

  spaceship_t *cur_spaceship = cur->spaceship_list;
  while (cur_spaceship) {
    spaceship_t *next_spaceship = cur_spaceship->next_ship;
    free(cur_spaceship->name);
    cur_spaceship->name = NULL;
    free(cur_spaceship);
    cur_spaceship = NULL;
    cur_spaceship = next_spaceship;
  }

  free(cur);
  cur = NULL;
} /* destroy_planet() */

/*
 * remove a resource from a planet
 */

int remove_resource(planet_t *planet, char *target_name) {
  assert(planet);
  assert(target_name);

  resource_t *cur = planet->resource_list;
  while (cur) {
    if (strcmp(cur->name, target_name) == 0) {
      if (cur->prev_resource) {
        cur->prev_resource->next_resource = cur->next_resource;
      }
      else {
        planet->resource_list = cur->next_resource;
      }

      if (cur->next_resource) {
        cur->next_resource->prev_resource = cur->prev_resource;
      }

      free(cur->name);
      cur->name = NULL;
      free(cur);
      cur = NULL;
      return OK;
    }
    cur = cur->next_resource;
  }

  return NO_SUCH_RESOURCE;
} /* remove_resource() */

/*
 * move a spaceship to a planet
 */

int move_spaceship(spaceship_t *spaceship, int id, planet_t **planets) {
  assert(spaceship != NULL);
  assert(id > 0);
  assert(planets != NULL);
  assert(*planets != NULL);

  planet_t *dest = NULL;

  planet_t *head = *planets;
  while (head->prev_planet) {
    head = head->prev_planet;
  }

  planet_t *cur_planet = head;

  while (cur_planet) {
    if (cur_planet->planet_id == id) {
      dest = cur_planet;
      break;
    }
    cur_planet = cur_planet->next_planet;
  }

  if (!dest) {
    return NO_SUCH_PLANET;
  }

  spaceship_t *prev_ship = spaceship->prev_ship;
  spaceship_t *next_ship = spaceship->next_ship;

  if (prev_ship) {
    prev_ship->next_ship = next_ship;
  }
  else {
    planet_t *found = find_planet(*planets, spaceship);
    if (found) {
      found->spaceship_list = next_ship;
    }
  }

  if (next_ship) {
    next_ship->prev_ship = prev_ship;
  }

  spaceship_t *cur_spaceship = dest->spaceship_list;
  spaceship_t *prev_spaceship = NULL;
  while (cur_spaceship) {
    if ((spaceship->name[0] < cur_spaceship->name[0]) ||
        ((spaceship->name[0] == cur_spaceship->name[0]) &&
        (spaceship->spaceship_id < cur_spaceship->spaceship_id))) {
      break;
    }
    prev_spaceship = cur_spaceship;
    cur_spaceship = cur_spaceship->next_ship;
  }

  spaceship->prev_ship = prev_spaceship;
  spaceship->next_ship = cur_spaceship;


  if (prev_spaceship) {
    prev_spaceship->next_ship = spaceship;
  }
  else {
    dest->spaceship_list = spaceship;
  }

  if (cur_spaceship) {
    cur_spaceship->prev_ship = spaceship;
  }

  (*planets) = dest;

  return OK;
} /* move_spaceship() */

/*
 * merge resources from one planet to another
 */

int merge_resources(planet_t *target, planet_t *add) {
  assert(target);
  assert(add);

  resource_t *target_cur = target->resource_list;
  resource_t *prev_cur = NULL;

  while (target_cur) {
    prev_cur = target_cur;
    target_cur = target_cur->next_resource;
  }

  if (prev_cur) {
    prev_cur->next_resource = add->resource_list;
  }
  else {
    target->resource_list = add->resource_list;
  }
  target_cur = target->resource_list;

  while (target_cur) {
    target_cur = target_cur->next_resource;
  }

  target_cur = sort_resource(target->resource_list);
  target->resource_list = target_cur;

  while (target_cur) {
    target_cur = target_cur->next_resource;
  }

  target_cur = target->resource_list;

  while (target_cur) {
    if ((target_cur->next_resource) &&
        (strcmp(target_cur->name, target_cur->next_resource->name) == 0)) {
      target_cur->volume += target_cur->next_resource->volume;
      target_cur->value.dollars += target_cur->next_resource->value.dollars;
      target_cur->value.cents += target_cur->next_resource->value.cents;
      resource_t *target_del = target_cur->next_resource;

      target_cur->next_resource = target_del->next_resource;


      if (target_del->next_resource) {
        target_del->next_resource->prev_resource = target_cur;
      }

      target_del->next_resource = NULL;
      target_del->prev_resource = NULL;
      free(target_del->name);
      target_del->name = NULL;
      free(target_del);
      target_del = NULL;
    }
    else {
      target_cur = target_cur->next_resource;
    }
  }

  target_cur = target->resource_list;
  while (target_cur) {
    target_cur = target_cur->next_resource;
  }

  add->resource_list = NULL;

  return OK;
} /* merge_resources() */

/*
 * rotate planet list
 */

int rotate_planets(planet_t **planet_ptr, int num_rotations) {
  assert(planet_ptr);

  if ((num_rotations == 0) || (!*planet_ptr)) {
    return OK;
  }

  int len = 1;
  planet_t *cur = *planet_ptr;
  while (cur->next_planet) {
    len++;
    cur = cur->next_planet;
  }

  num_rotations = num_rotations % len;
  if (num_rotations < 0) {
    num_rotations += len;
  }

  if (num_rotations == 0) {
    return OK;
  }

  cur->next_planet = *planet_ptr;
  (*planet_ptr)->prev_planet = cur;

  for (int i = 0; i < len - num_rotations; i++) {
    cur = cur->next_planet;
  }

  *planet_ptr = cur->next_planet;
  (*planet_ptr)->prev_planet = NULL;
  cur->next_planet = NULL;

  return OK;
} /* rotate_planets() */

/*
 * find the planet that a spaceship is on
 */

planet_t *find_planet(planet_t *planet, spaceship_t *ship) {
  assert(ship);
  assert(planet);

  spaceship_t *prev = ship;
  spaceship_t *head = ship;
  while (prev) {
    head = prev;
    prev = prev->prev_ship;
  }

  planet_t *cur_planet = planet;
  while (cur_planet) {
    if (cur_planet->spaceship_list == head) {
      return cur_planet;
    }
    cur_planet = cur_planet->next_planet;
  }

  return NULL;
} /* find_planet() */

/*
 * destroy all planets
 */

void apocalypse(planet_t **node) {
  assert(node);
  assert(*node);

  planet_t *prev = *node;
  planet_t *head = *node;
  while (prev) {
    head = prev;
    prev = prev->prev_planet;
  }

  while (head) {
    destroy_planet(&head);
  }

  (*node) = head;
} /* apocalypse() */
