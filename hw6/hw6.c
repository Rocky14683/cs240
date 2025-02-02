/* Add any includes  */

#include "hw6.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Define any global variables  */

int g_stop_count = 0;
stop_t g_stop_array[MAX_STOPS] = {0};

/*
 * validate buffer(1 line)
 */

bool validate_buffer(char *buffer) {
  assert(buffer != NULL);
  if (strchr(buffer, '\n') == NULL) {
    return false;
  }
  int count = 0;
  for (int i = 0; buffer[i] != '\0'; i++) {
    if (buffer[i] == '$') {
      count++;
    }
  }

  if (count != 3) {
    return false;
  }

  return true;
} /* validate_buffer() */


/*
 * tokenize buffer and store in dest
 */

bool tokenize_buffer(char dest[4][MAX_BUF_SIZE], char *tokens) {
  assert(tokens != NULL);
  assert(dest != NULL);
  const char separator[] = "$";

  char *token = strtok(tokens, separator);
  for (int i = 0; i < 4; i++) {
    if ((token == NULL) || (isspace(token[0])) || (strlen(token) == 0)) {
      if (i >= 2) {
        // i: 2, 3
        break;
      }
      else {
        // i: 0, 1
        return false;
      }
    }
    else {
      strncpy(dest[i], token, MAX_BUF_SIZE - 1);
      dest[i][strlen(dest[i])] = '\0';
      if ((i == 0) && (strlen(dest[0]) >= MAX_NAME_SIZE)) {
        return false;
      }
      token = strtok(NULL, separator);
    }
  }
  return true;
} /* tokenize_buffer() */


/*
 * tokenize links and store in dest
 */

bool get_links(char dest[MAX_LINKS][MAX_NAME_SIZE], int *lk_count, char *tok) {
  assert(tok != NULL);
  assert(dest != NULL);
  assert(lk_count != NULL);
  const char separator[] = ">";

  char *token = strtok(tok, separator);
  if ((token == NULL) || (strlen(token) == 0)) {
    strcpy(dest[0], "");
  }
  int i = 0;
  for (i = 0; token != NULL; i++) {
    if (i >= MAX_LINKS) {
      return false;
    }
    if ((token[0] == '\0') || (strlen(token) == 0)) {
      i--;
      break;
    }
    if (strlen(token) > MAX_NAME_SIZE) {
      return false;
    }
    strcpy(dest[i], token);
    token = strtok(NULL, separator);
  }
  *lk_count = i;
  return true;
} /* get_links() */


/*
 * tokenize frequency and store in dest
 */

bool get_freq(int dest[MAX_LINKS], int *freq_count, char *tokens) {
  assert(tokens != NULL);
  assert(dest != NULL);
  assert(freq_count != NULL);

  const char separator[] = "+";
  char *token = strtok(tokens, separator);
  if ((token == NULL) || (strlen(token) == 0)) {
    dest[0] = 0;
  }

  int i = 0;
  for (i = 0; token != NULL; i++) {
    if (i >= MAX_LINKS) {
      return false;
    }
    if ((token[0] == '\0') || (strlen(token) == 0)) {
      i--;
      break;
    }
    dest[i] = atoi(token);
    if (dest[i] <= 0) {
      return false;
    }
    token = strtok(NULL, separator);
  }
  *freq_count = i;
  return true;
} /* get_freq() */

/*
 * reset global variables
 */

void reset_globals() {
  g_stop_count = 0;
  memset(g_stop_array, 0, sizeof(g_stop_array));
} /* reset_globals() */


/*
 * read the file and store the data in the global array and return the size
 */

int read_stops(char *in_file) {
  assert(in_file != NULL);
  reset_globals();
  FILE *file = fopen(in_file, "r");
  if (file == NULL) {
    return FILE_READ_ERR;
  }

  char buf[MAX_BUF_SIZE] = {0};
  bool entered = false;

  // name$location$>link1>link2...>linkN$+freq1+freq2...+freqN
  while (true) {
    if (fgets(buf, MAX_BUF_SIZE, file) != NULL) {
      entered = true;
      if (g_stop_count >= MAX_STOPS) {
        fclose(file);
        file = NULL;
        return TOO_MUCH_DATA;
      }

      if (!validate_buffer(buf)) {
        fclose(file);
        file = NULL;
        return BAD_RECORD;
      }


      char category[4][MAX_BUF_SIZE] = {0};

      if (!tokenize_buffer(category, buf)) {
        fclose(file);
        file = NULL;
        return BAD_RECORD;
      }

      stop_t stop = {0};


      int freq_count = 0;
      int link_count = 0;

      if (!get_links(stop.links, &link_count, category[2])) {
        fclose(file);
        file = NULL;
        return BAD_RECORD;
      }

      if (!get_freq(stop.bus_frequency, &freq_count, category[3])) {
        fclose(file);
        file = NULL;
        return BAD_RECORD;
      }

      if (freq_count != link_count) {
        fclose(file);
        file = NULL;
        return BAD_RECORD;
      }

      for (int i = 0; i < MAX_STOPS; i++) {
        if (g_stop_array[i].name[0] == '\0') {
          break;
        }
        if (strcmp(g_stop_array[i].name, category[0]) == 0) {
          fclose(file);
          file = NULL;
          return DUPLICATE_NAMES;
        }
      }

      strcpy(stop.name, category[0]);
      strncpy(stop.location, category[1], MAX_LOC_SIZE - 1);
      stop.location[strlen(stop.location)] = '\0';

      g_stop_array[g_stop_count] = stop;
      g_stop_count++;
    }
    else {
      if (ferror(file)) {
        fclose(file);
        file = NULL;
        return FILE_READ_ERR;
      }
      fclose(file);
      file = NULL;
      break;
    }
  }

  if (!entered) {
    return NO_DATA;
  }

  for (int i = 0; i < g_stop_count; i++) {
    for (int j = 0; j < MAX_LINKS; j++) {
      if (g_stop_array[i].links[j][0] == '\0') {
        break;
      }
      bool found = false;
      for (int k = 0; k < g_stop_count; k++) {
        if (strcmp(g_stop_array[i].links[j], g_stop_array[k].name) == 0) {
          found = true;
          break;
        }
      }
      if (!found) {
        return NOT_FOUND;
      }
    }
  }

  return g_stop_count;
} /* read_stops() */


/*
 * find hubs and return the numbers of hubs
 */

int find_hubs() {
  if (g_stop_count == 0) {
    return NO_DATA;
  }

  int count = 0;
  for (int i = 0; i < g_stop_count; i++) {
    stop_t stop = g_stop_array[i];
    for (int j = 0; j < g_stop_count; j++) {
      if ((i != j) && (strcmp(stop.name, g_stop_array[j].name) == 0)) {
        return DUPLICATE_NAMES;
      }
    }

    int buses = 0;

    int outgoing_stops = 0;
    for (int x = 0; x < MAX_LINKS; x++) {
      if (stop.links[x][0] == '\0') {
        break;
      }
      buses -= stop.bus_frequency[x];
      outgoing_stops++;
    }

    // 3 ontgoing stops
    if (outgoing_stops < 3) {
      continue;
    }

    // loop thru links
    // loop thru all stops' links and find equal to current stop
    for (int stop_idx = 0; stop_idx < g_stop_count; stop_idx++) {
      for (int link_idx = 0; link_idx < MAX_LINKS; link_idx++) {
        if (g_stop_array[stop_idx].links[link_idx][0] == '\0') {
          break;
        }
        if (strcmp(stop.name, g_stop_array[stop_idx].links[link_idx]) == 0) {
          buses += g_stop_array[stop_idx].bus_frequency[link_idx];
        }
      }
    }

    // leaving the stop each hour is equal to the total number of buses arriving
    if (buses != 0) {
      continue;
    }
    // contain “Central”
    if (!((strncmp(stop.location, "Central", 7) == 0) ||
          (strcmp(stop.location + strlen(stop.location) - 7, "Central") ==
           0))) {
      continue;
    }
    printf("success: %s, %s\n", stop.name, stop.location);
    count++;
  }

  return count == 0 ? NOT_FOUND : count;
} /* find_hubs() */