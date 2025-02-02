/*
 * Homework 7 Template
 * CS 240, Fall 2024
 */


#include "hw7.h"

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
        break;
      }
      else {
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
          return BAD_RECORD;
        }
      }

      strcpy(stop.name, category[0]);
      strncpy(stop.location, category[1], MAX_LOC_SIZE - 1);
      stop.location[strlen(stop.location)] = '\0';
      stop.link_ptrs[0] = NULL;

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
        return NO_DATA;
      }
    }
  }
  return g_stop_count;
} /* read_stops() */

/*
 * link stops together by populating the link_ptrs array of each bus stop.
 */

int connect_stops(void) {
  if (g_stop_count == 0) {
    return NO_DATA;
  }

  for (int i = 0; i < g_stop_count; i++) {
    for (int j = 0; j < MAX_LINKS; j++) {
      if (g_stop_array[i].links[j][0] == '\0') {
        g_stop_array[i].link_ptrs[j] = NULL;
      }
      else {

        bool found = false;
        for (int k = 0; k < g_stop_count; k++) {
          if (strcmp(g_stop_array[i].links[j], g_stop_array[k].name) == 0) {
            g_stop_array[i].link_ptrs[j] = &g_stop_array[k];
            found = true;
            break;
          }

        }

        if (!found) {
          return NOT_FOUND;
        }
      }
    }
  }

  return OK;
} /* connect_stops() */

/*
 * find the longest minimum-frequency path through the bus network, starting
 * at the given stop.
 */

int longest_min_freq_path(stop_t *stop_ptr) {
  assert(stop_ptr != NULL);
  stop_t *current = stop_ptr;
  int min_freq = current->bus_frequency[0];
  int visited_count = 0;
  stop_t *visited[MAX_STOPS] = {0};

  while ((current != NULL) && (visited_count < MAX_STOPS)) {

    for (int i = 0; i < visited_count; i++) {
      if (visited[i] == current) {
        return PATH_IS_CYCLE;
      }
    }

    visited[visited_count] = current;
    visited_count++;
    bool found = false;
    stop_t *next = NULL;
    for (int i = 0; i < MAX_LINKS; i++) {
      if (current->link_ptrs[i] != NULL) {
        if ((!found) || (current->bus_frequency[i] < min_freq)) {
          min_freq = current->bus_frequency[i];
          next = current->link_ptrs[i];
          found = true;
        }
      }
    }

    if (!found) {
      break;
    }

    current = next;
  }

  return visited_count;
} /* longest_min_freq_path() */

/*
 *  locate all hubs along the maximum-frequency path starting from the given
 *  bus stop
 */

int hubs_in_max_freq_path(stop_t *stop_ptr, char *hub_name) {
  assert(stop_ptr != NULL);
  assert(hub_name != NULL);

  strcpy(hub_name, "");
  int max_freq = -1;
  int visited_count = 0;
  int hub_count = 0;
  stop_t *current = stop_ptr;
  stop_t *visited[MAX_STOPS] = {0};
  while ((current != NULL) && (visited_count < MAX_STOPS)) {

    for (int i = 0; i < visited_count; i++) {
      if (visited[i] == current) {
        return PATH_IS_CYCLE;
      }
    }

    visited[visited_count] = current;
    visited_count++;

    if (strstr(current->location, "Central") != NULL) {
      strcat(hub_name, current->name);
      strcat(hub_name, "\n");
      hub_count++;
    }

    bool found = false;
    stop_t *next = NULL;
    max_freq = -1;
    for (int i = 0; i < MAX_LINKS; i++) {
      if (current->link_ptrs[i] != NULL) {
        if (current->bus_frequency[i] >= max_freq) {
          max_freq = current->bus_frequency[i];
          next = current->link_ptrs[i];
          found = true;
        }
      }
    }

    if (!found) {
      break;
    }

    current = next;
  }

  if (strlen(hub_name) > 0) {
    hub_name[strlen(hub_name) - 1] = '\0';
  }

  return hub_count;
} /* hubs_in_max_freq_path() */



