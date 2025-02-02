/* Name, hw5.c, CS 24000, Fall 2024
 */

/* Add any includes here */

#include "hw5.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>

typedef struct station_map_t {
  float total_mass;
  float max_mass;
  bool appear;
}

station_map_t;


/*
 * This function prints the item_t struct to the console.
 */

void print_item(item_t item) {
  printf("ID: %d\n", item.id);
  printf("Station Code: %c\n", item.station_code);
  printf("Name: %s\n", item.name);
  printf("Category: %s\n", item.category);
  printf("Status: %d\n", item.status);
  printf("Mass: %f\n", item.mass);
  printf("Manufacture Date: %d\n", item.manufacture_date);
} /* print_item() */

/*
 * This function check if the item is expired.
 */

bool is_expired(int date, int manufacture_date) {
  if (date == manufacture_date + 5) {
    return true;
  }
  int year = (date % 10000);
  int month = date / 1000000;
  int day = (date / 10000) % 100;
  int expired_year = (manufacture_date % 10000) + 5;
  int expired_month = manufacture_date / 1000000;
  int expired_day = (manufacture_date / 10000) % 100;
  return (year > expired_year) ||
         ((year == expired_year) && (month > expired_month)) ||
         ((year == expired_year) && (month == expired_month) &&
          (day >= expired_day));
} /* is_expired() */

/*
 * This function validate the date.
 */

bool validate_date(int date) {
  if (date < 0) {
    return false;
  }
  // MMDDYYYY
  if (date % 10000 < 1958) {
    return false;
  }
  int month = date / 1000000;
  if (month == 0) {
    return false;
  }
  int day = (date / 10000) % 100;
  if (day == 0) {
    return false;
  }

  return true;
} /* validate_date() */

/*
 * This function checks if the item is valid.
 */

bool is_valid(item_t item) {
  if (item.id < 0) {
    return false;
  }
  if ((item.station_code < 'a') || (item.station_code > 'z')) {
    return false;
  }
  if (strlen(item.name) > MAX_NAME_LEN - 1) {
    return false;
  }

  if (strlen(item.category) > MAX_NAME_LEN - 1) {
    return false;
  }

  if (!((item.status == AVAILABLE) || (item.status == IN_USE) ||
        (item.status == EXPIRED))) {
    return false;
  }

  if (item.mass < 0) {
    return false;
  }

  if (!validate_date(item.manufacture_date)) {
    return false;
  }

  return true;
} /* is_valid() */

/*
 * This function reads the item from the file.
 */

item_t read_item(FILE *file, int pos_n) {
  assert(file != NULL);
  assert(pos_n >= 0);
  item_t item = {0};
  rewind(file);
  if (fseek(file, pos_n * sizeof(item_t), SEEK_SET) != 0) {
    return BAD_ITEM;
  }
  if ((fread(&item, sizeof(item_t), 1, file) != 1)) {
    return BAD_ITEM;
  }
  return item;
} /* read_item() */


/*
 * This function writes the item to the file.
 */

int write_item(FILE *file, item_t item, int pos_n) {
  assert(file != NULL);
  assert(pos_n >= 0);
  if (!is_valid(item)) {
    return MALFORMED_ITEM;
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  if (file_size < (pos_n + 1) * sizeof(item_t)) {
    return WRITE_ERR;
  }

  if (fseek(file, pos_n * sizeof(item_t), SEEK_SET)) {
    return WRITE_ERR;
  }

  if (fwrite(&item, sizeof(item_t), 1, file) != 1) {
    return WRITE_ERR;
  }

  return OK;
} /* write_item() */

/*
 * This function adds the item to the file.
 */

int add_item(FILE *file, item_t item) {
  assert(file != NULL);

  if (!is_valid(item)) {
    return MALFORMED_ITEM;
  }
  rewind(file);
  item_t target = {0};
  int pos = 0;
  while (fread(&target, sizeof(item_t), 1, file) == 1) {
    if (target.id == FREE_ID) {
      if (fseek(file, -1 * (int) sizeof(item_t), SEEK_CUR)) {
        return WRITE_ERR;
      }
      if (fwrite(&item, sizeof(item_t), 1, file) != 1) {
        return WRITE_ERR;
      }
      return pos;
    }
    pos++;
  }

  if (ferror(file)) {
    return WRITE_ERR;
  }

  return NO_SPACE;
} /* add_item() */


/*
 * This function decommission a category of items.
 */

int decommission_category(FILE *file, char *sub_str) {
  assert(file != NULL);
  assert(sub_str != NULL);
  item_t item = {0};
  int count = 0;
  rewind(file);
  while (fread(&item, sizeof(item_t), 1, file) == 1) {
    if (strstr(item.category, sub_str)) {
      if (item.id == FREE_ID) {
        continue;
      }
      item.id = FREE_ID;
      if (fseek(file, -1 * (int) sizeof(item_t), SEEK_CUR)) {
        return WRITE_ERR;
      }
      if (fwrite(&item, sizeof(item_t), 1, file) != 1) {
        return WRITE_ERR;
      }
      count++;
    }
  }

  if (ferror(file)) {
    return WRITE_ERR;
  }

  return count == 0 ? NO_ITEM : count;
} /* decommission_category() */


/*
 * This function finds the optimal route.
 */

int find_optimal_route(item_t item, char dest_station,
                       char routes[N_ROUTES][MAX_ROUTE_LEN]) {
  assert(routes != NULL);
  assert(is_valid(item));
  assert(item.station_code != dest_station);
  assert((dest_station >= 'a') && (dest_station <= 'z'));
  char start = item.station_code;
  int idx = -1;
  int shortest = MAX_ROUTE_LEN + 1;

  for (int i = 0; i < N_ROUTES; i++) {
    if (routes[i] == NULL) {
      break;
    }

    int start_index = -1;
    int dest_index = -1;

    for (int j = 0; (j < MAX_ROUTE_LEN) && (routes[i][j] != '\0'); j++) {
      if (routes[i][j] == start) {
        start_index = j;
      }
      if (routes[i][j] == dest_station) {
        dest_index = j;
        break;
      }
    }

    if ((start_index != -1) && (dest_index != -1)) {
      int stops = dest_index - start_index;
      if (stops < shortest) {
        shortest = stops;
        idx = i;
      }
    }
  }

  return idx == -1 ? NO_ROUTE : idx;
} /* find_optimal_route() */

/*
 * This function finds the item by id.
 */

item_t find_item_by_id(FILE *file, int id) {
  assert(file != NULL);
  assert(id >= 0);
  item_t item = {0};
  rewind(file);
  while (fread(&item, sizeof(item_t), 1, file) == 1) {
    if (item.id == id) {
      return item;
    }
  }
  return BAD_ITEM;
} /* find_item_by_id() */

/*
 * This function finds the expired items.
 */

int find_expired_items(FILE *file, int date) {
  assert(file != NULL);
  assert(validate_date(date));
  item_t item = {0};
  int count = 0;
  rewind(file);
  while (fread(&item, sizeof(item_t), 1, file) == 1) {
    if (!is_valid(item)) {
      continue;
    }
    if ((is_expired(date, item.manufacture_date)) &&
        (item.status == AVAILABLE)) {
      item.status = EXPIRED;
      count++;

      if (fseek(file, -1 * (int) sizeof(item_t), SEEK_CUR)) {
        return WRITE_ERR;
      }
      if (fwrite(&item, sizeof(item_t), 1, file) != 1) {
        return WRITE_ERR;
      }

    }
  }

  if (ferror(file)) {
    return WRITE_ERR;
  }

  return count;
} /* find_expired_items() */


/*
 * This function marks the item as used.
 */

int mark_used(FILE *file, int id) {
  assert(file != NULL);
  assert(id >= 0);
  item_t item = {0};
  rewind(file);
  while (fread(&item, sizeof(item_t), 1, file) == 1) {
    if (item.id == id) {
      item.status = IN_USE;
      if (fseek(file, -1 * (int) sizeof(item_t), SEEK_CUR)) {
        return WRITE_ERR;
      }
      if (fwrite(&item, sizeof(item_t), 1, file) != 1) {
        return WRITE_ERR;
      }
      return OK;
    }
  }

  if (ferror(file)) {
    return WRITE_ERR;
  }

  return NO_ITEM;
} /* mark_used() */

/*
 * This function balances the stations.
 */

float balance_stations(FILE *file) {
  assert(file != NULL);
  station_map_t station_map[26] = {0};
  item_t item = {0};
  rewind(file);
  while (fread(&item, sizeof(item_t), 1, file) == 1) {
    if (!is_valid(item)) {
      continue;
    }
    int idx = item.station_code - 'a';
    station_map[idx].appear = true;
    station_map[idx].total_mass += item.mass;
    if (item.mass > station_map[idx].max_mass) {
      station_map[idx].max_mass = item.mass;
    }
  }


  float max_mass = station_map[0].total_mass;
  int max_idx = 0;
  float min_mass = station_map[0].total_mass;
  int min_idx = 0;
  for (int i = 0; i < 26; i++) {
    if (!station_map[i].appear) {
      continue;
    }

    if (station_map[i].total_mass > max_mass) {
      max_mass = station_map[i].total_mass;
      max_idx = i;
    }
    if (station_map[i].total_mass < min_mass) {
      min_mass = station_map[i].total_mass;
      min_idx = i;
    }

  }

  if ((min_mass == max_mass) || (min_idx == max_idx)) {
    return ALREADY_BALANCED;
  }

  // step 2
  float diff = max_mass - min_mass;

  // step 3
  item_t restricted_max_mass_item = {0};
  long n = 0;
  long pos = -1;
  bool entered = false;
  rewind(file);
  while (fread(&item, sizeof(item_t), 1, file) == 1) {
    if (!is_valid(item)) {
      n++;
      continue;
    }
    if (item.station_code == (char) max_idx + 'a') {
      if ((item.mass < diff) && (restricted_max_mass_item.mass < item.mass)) {

        restricted_max_mass_item = item;
        pos = n * sizeof(item_t);
        entered = true;
      }
    }
    n++;
  }


  if (!entered) {
    return NO_ITEM;
  }

  // step 4
  restricted_max_mass_item.station_code = (char) (min_idx + 'a');
  if (fseek(file, pos, SEEK_SET)) {
    return WRITE_ERR;
  }
  if (fwrite(&restricted_max_mass_item, sizeof(item_t), 1, file) != 1) {
    return WRITE_ERR;
  }

  // step 5
  diff -= 2 * restricted_max_mass_item.mass;
  return diff;
} /* balance_stations() */