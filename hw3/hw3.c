/*
 * Homework 3 Template
 * CS 240 - Fall 2024
 *
 */

/* Add any includes here */
#include "hw3.h"

#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int g_mission_count = 0;
char g_astronaut_and_mission[MAX_MISSIONS][NUM_MISSION_COLS][MAX_NAME_LEN] =
        {0};
char g_equipment[MAX_MISSIONS][NUM_COLS_IN_PAIR][MAX_NAME_LEN] = {0};
float g_op_hours[MAX_MISSIONS][NUM_COLS_IN_PAIR] = {0};
char g_mission_dates[MAX_MISSIONS][NUM_COLS_IN_PAIR][MAX_DATE_LEN] = {0};
int g_experiments[MAX_MISSIONS][NUM_COLS_IN_PAIR] = {0};

typedef struct date_pair_t {
  int start[3];
  int end[3];
}

date_pair_t;


typedef struct hash_table_t {
  char* name;
  int total_time;
}

hash_table_t;

/*
 * parse date string into year, month, and day, return 0 if successful
 */

int parse_date(const char *date, int *year, int *month, int *day) {
  if (sscanf(date, "%4d-%2d-%2d", year, month, day) != 3) {
    return BAD_DATA;
  }
  return 0;
} /* parse_date() */

/*
 * Validate date string, return true if valid
 */

bool validate_date(const char *date) {
  int year = 0;
  int month = 0;
  int day = 0;

  if (parse_date(date, &year, &month, &day) != 0) {
    return false;
  }

  if ((year < MIN_YEAR) || (year > MAX_YEAR) ||
      (month < 1) || (month > 12) ||
      (day < 1) || (day > 30)) {
    return false;
  }

  return true;
} /* validate_date() */

/*
 * Calculate duration between two dates, return the duration in hours
 */

int calculate_duration(const char *start_date, const char *end_date) {
  int start_year = 0;
  int start_month = 0;
  int start_day = 0;
  int end_year = 0;
  int end_month = 0;
  int end_day = 0;

  if (parse_date(start_date, &start_year, &start_month,
                 &start_day) != 0) {
    return BAD_DATA;
  }

  if (parse_date(end_date, &end_year, &end_month,
                 &end_day) != 0) {
    return BAD_DATA;
  }

  return (end_year - start_year) * 12 * 30 * 24 +
         (end_month - start_month) * 30 * 24 +
         (end_day - start_day) * 24;
} /* calculate_duration() */

/*
 * Compare equipment pairs, return true if they are the same
 */

bool compare_equipment_pairs(
const char exp_pair1[2][MAX_NAME_LEN], const char exp_pair2[2][MAX_NAME_LEN]) {

  return (strcmp(exp_pair1[0], exp_pair2[0]) == 0) &&
         (strcmp(exp_pair1[1], exp_pair2[1]) == 0);
} /* compare_equipment_pairs() */


/*
 * clean up the global variables buffer
 */

void cleanup() {
  g_mission_count = 0;
  memset(g_astronaut_and_mission, 0, sizeof(g_astronaut_and_mission));
  memset(g_equipment, 0, sizeof(g_equipment));
  memset(g_op_hours, 0, sizeof(g_op_hours));
  memset(g_mission_dates, 0, sizeof(g_mission_dates));
  memset(g_experiments, 0, sizeof(g_experiments));
} /* cleanup() */

/*
 * popular_equipment_pair but specific to an astronaut,
 * return the index of the equipment pair
 */

int popular_equipment_pair_used_by(char *name) {
  if (g_mission_count == 0) {
    return NO_DATA_POINTS;
  }

  // equipment pairs list
  char pairs[MAX_MISSIONS][2][MAX_NAME_LEN] = {0};
  // times, first index
  int pair_count[MAX_MISSIONS][2] = {0};
  int unique_pairs = 0;

  for (int i = 0; i < g_mission_count; i++) {
    bool found = false;
    if (strcmp(g_astronaut_and_mission[i][0], name) != 0) {
      continue;
    }
    for (int j = 0; j < unique_pairs; j++) {
      if (strcmp(g_astronaut_and_mission[j][0], name) != 0) {
        continue;
      }
      if (compare_equipment_pairs(g_equipment[i],
                                  pairs[j])) {
        pair_count[j][0]++;
        found = true;
      }
    }
    if (!found) {
      memcpy(&pairs[unique_pairs], g_equipment[i],
             sizeof(g_equipment[i]));
      pair_count[unique_pairs][0]++;
      pair_count[unique_pairs][1] = i;
      unique_pairs++;
    }
  }

  if (unique_pairs == 1) {
    return pair_count[0][1];
  }

  int max_count = -1;
  int max_index = -1;
  bool multiple = false;

  for (int i = 0; i < unique_pairs; i++) {
    if (pair_count[i][0] > max_count) {
      max_count = pair_count[i][0];
      max_index = i;
      multiple = false;
    }
    else if (pair_count[i][0] == max_count) {
      multiple = true;
    }
  }

  if (multiple) {
    return MULTIPLE_PAIRS;
  }

  return pair_count[max_index][1];
} /* popular_equipment_pair_used_by() */

/*
 * read the file and parse the data into the global buffers
 * return the mission count
 */

int read_tables(char *file_name) {
  cleanup();
  FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    return FILE_READ_ERR;
  }
  int scan_result = 0;
  char line[2048] = {0};

  char temp_astronaut_and_mission[NUM_MISSION_COLS][MAX_NAME_LEN] = {0};
  char temp_equipment[NUM_COLS_IN_PAIR][MAX_NAME_LEN] = {0};
  float temp_op_hours[NUM_COLS_IN_PAIR] = {0};
  char temp_mission_dates[NUM_COLS_IN_PAIR][MAX_DATE_LEN] = {0};
  int temp_experiments[NUM_COLS_IN_PAIR] = {0};

  bool entered = false;
  while (fscanf(file, "%2047[^\n]\n", line) != EOF) {
    entered = true;
    scan_result = sscanf(line,
                         "%99[^|]|%99[^|]|%99[^*]*%f|%99[^*]*%f|"
                         "%10[^|]|%10[^|]|%d|%d|%99[^|]|",
                         temp_astronaut_and_mission[0],
                         temp_astronaut_and_mission[1],
                         temp_equipment[0],
                         &temp_op_hours[0],
                         temp_equipment[1],
                         &temp_op_hours[1],
                         temp_mission_dates[0],
                         temp_mission_dates[1],
                         &temp_experiments[0],
                         &temp_experiments[1],
                         temp_astronaut_and_mission[2]);


    if (g_mission_count >= MAX_MISSIONS) {
      fclose(file);
      return OUT_OF_BOUNDS;
    }

    if ((temp_op_hours[0] < 0) || (temp_op_hours[1] < 0)) {
      fclose(file);
      return BAD_DATA;
    }

    if ((temp_experiments[0] < 0) ||
        (temp_experiments[1] < 0) ||
        (temp_experiments[1] > temp_experiments[0])) {
      fclose(file);
      return BAD_DATA;
    }

    if ((scan_result != 11) ||
        (!validate_date(temp_mission_dates[0])) ||
        (!validate_date(temp_mission_dates[1]))) {

      fclose(file);
      return BAD_DATA;
    }

    memcpy(&g_astronaut_and_mission[g_mission_count],
           temp_astronaut_and_mission, sizeof(temp_astronaut_and_mission));
    memcpy(&g_equipment[g_mission_count], temp_equipment,
           sizeof(temp_equipment));
    memcpy(&g_op_hours[g_mission_count], temp_op_hours,
           sizeof(temp_op_hours));

    memcpy(&g_mission_dates[g_mission_count], temp_mission_dates,
           sizeof(temp_mission_dates));
    memcpy(&g_experiments[g_mission_count], temp_experiments,
           sizeof(temp_experiments));
    g_mission_count++;
  }

  fclose(file);
  if (!entered) {
    return NO_DATA_POINTS;
  }

  return g_mission_count;
} /* read_tables() */


/*
 * get the total time of an astronaut, return the total time
 */

int get_total_time(char *name) {
  if (g_mission_count == 0) {
    return NO_DATA_POINTS;
  }

  int total_time = 0.0f;
  bool found = false;
  for (int i = 0; i < g_mission_count; i++) {
    if (strcmp(g_astronaut_and_mission[i][0], name) == 0) {
      found = true;

      int time = 0;
      if ((time = calculate_duration(
              g_mission_dates[i][0],
              g_mission_dates[i][1])) == BAD_DATA) {
        return BAD_DATA;
      }

      total_time += time;
    }
  }

  if ((total_time == 0.0f) || (!found)) {
    return NO_SUCH_ASTRONAUT;
  }

  return total_time;
} /* get_total_time() */

/*
 * get the highest success rate mission, return the index
 */

int highest_success_mission() {
  if (g_mission_count == 0) {
    return NO_DATA_POINTS;
  }

  int highest_idx = -1;
  double highest_rate = 0.0;

  for (int i = 0; i < g_mission_count; i++) {
    if (g_experiments[i][0] == 0) {
      continue;
    }

    double rate = (double) g_experiments[i][1] / g_experiments[i][0];
    if (rate > highest_rate) {
      highest_rate = rate;
      highest_idx = i;
    }
  }

  return highest_idx;
} /* highest_success_mission() */

/*
 * get the most popular equipment pair, return the index of the pair
 */

int popular_equipment_pair() {
  if (g_mission_count == 0) {
    return NO_DATA_POINTS;
  }

  // checked equipment pairs list
  char pairs[MAX_MISSIONS][2][MAX_NAME_LEN] = {0};
  // times, first index
  int pair_count[MAX_MISSIONS][2] = {0};
  int unique_pairs = 0;

  for (int i = 0; i < g_mission_count; i++) {
    bool found = false;
    for (int j = 0; j < unique_pairs; j++) {
      if (compare_equipment_pairs(g_equipment[i],
                                  pairs[j])) {
        pair_count[j][0]++;
        found = true;
      }
    }
    if (!found) {
      memcpy(&pairs[unique_pairs], g_equipment[i],
             sizeof(g_equipment[i]));
      pair_count[unique_pairs][0]++;
      pair_count[unique_pairs][1] = i;
      unique_pairs++;
    }
  }

  if (unique_pairs == 1) {
    return pair_count[0][1];
  }

  int max_count = -1;
  int max_index = -1;
  bool multiple = false;

  for (int i = 0; i < unique_pairs; i++) {
    if (pair_count[i][0] > max_count) {
      max_count = pair_count[i][0];
      max_index = i;
      multiple = false;
    }
    else if (pair_count[i][0] == max_count) {
      multiple = true;
    }
  }

  if (multiple) {
    return MULTIPLE_PAIRS;
  }

  return pair_count[max_index][1];
} /* popular_equipment_pair() */

/*
 * get the average mission duration of a mission type, return the average
 */

float avg_mission_duration(char *mission_type) {
  if (g_mission_count == 0) {
    return NO_DATA_POINTS;
  }

  int total_duration = 0;
  int mission_count = 0;

  for (int i = 0; i < g_mission_count; i++) {
    if (strcmp(g_astronaut_and_mission[i][2], mission_type) == 0) {

      int time = 0;
      if ((time = calculate_duration(
              g_mission_dates[i][0],
              g_mission_dates[i][1])) == BAD_DATA) {
        return BAD_DATA;
      }

      total_duration += time;
      mission_count++;
    }
  }

  if (mission_count == 0) {
    return NO_SUCH_TYPE;
  }

  return (float) total_duration / mission_count;
} /* avg_mission_duration() */

/*
 * get the astronaut with the longest mission, return the name of the astronaut
 */

char *longest_mission_astronaut() {
  if (g_mission_count == 0) {
    return NULL;
  }

  int longest_time = 0;
  char *longest_astronaut = NULL;
  hash_table_t items[MAX_MISSIONS] = {0};
  int astronaut_count = 0;
  for (int i = 0; i < g_mission_count; i++) {
    char* name = g_astronaut_and_mission[i][0];
    int time = 0;

    if ((time = calculate_duration(g_mission_dates[i][0],
                                   g_mission_dates[i][1]))
        == BAD_DATA) {
      return (char *) BAD_DATA;
    }

    bool found = false;
    for (int j = 0; j < astronaut_count; j++) {
      if (strcmp(name, items[j].name) == 0) {
        items[j].total_time += time;
        found = true;
        break;
      }
    }

    if (!found) {
      items[astronaut_count].name = name;
      items[astronaut_count].total_time += time;
      astronaut_count++;
    }
  }

  for (int i = 0; i < astronaut_count; i++) {
    if (items[i].total_time > longest_time) {
      longest_time = items[i].total_time;
      longest_astronaut = items[i].name;
    }
  }

  return longest_astronaut;
} /* longest_mission_astronaut() */

/*
 * calculate the efficiency score of an astronaut, return the score
 */

float calc_efficiency_score(char *astronaut_name) {
  if (g_mission_count == 0) {
    return NO_DATA_POINTS;
  }
  float highest_efficiency_score = 0.0f;
  bool found = false;
  for (int i = 0; i < g_mission_count; i++) {
    if (strcmp(g_astronaut_and_mission[i][0], astronaut_name) == 0) {
      found = true;
      float rate = (float) g_experiments[i][1] / g_experiments[i][0] * 100.f;
      float factor = 0;
      if (rate < 20) {
        factor = 0.5;
      }
      else if (rate < 50) {
        factor = 1.0;
      }
      else if (rate < 80) {
        factor = 1.5;
      }
      else {
        factor = 2.0;
      }

      float efficiency_score = (float) g_experiments[i][1] /
                               (g_op_hours[i][0] + g_op_hours[i][1]) * factor;

      highest_efficiency_score = fmax(efficiency_score,
                                      highest_efficiency_score);
    }
  }

  return found ? highest_efficiency_score : NO_SUCH_ASTRONAUT;
} /* calc_efficiency_score() */

/*
 * generate a report for an astronaut, return the status or error code
 */

int generate_report(char *out_file, char *astronaut_name) {
  if (g_mission_count == 0) {
    return NO_DATA_POINTS;
  }

  FILE *file = fopen(out_file, "w");
  if (file == NULL) {
    return FILE_WRITE_ERR;
  }

  bool found = false;
  int total_missions = 0;
  char buffer[2048] = {'\0'};
  for (int i = 0; i < g_mission_count; i++) {
    if (strcmp(g_astronaut_and_mission[i][0], astronaut_name) == 0) {
      found = true;
      total_missions++;
      sprintf(buffer,
              "%s:\n"
              "Equipment 1 and operational hours -> %s | %f\n"
              "Equipment 2 and operational hours -> %s | %f\n"
              "Mission Type -> %s\n"
              "Mission Start Date -> %s\n"
              "Duration of the mission in hours -> %d\n",
              g_astronaut_and_mission[i][1],
              g_equipment[i][0], g_op_hours[i][0],
              g_equipment[i][1], g_op_hours[i][1],
              g_astronaut_and_mission[i][2],
              g_mission_dates[i][0],
              calculate_duration(g_mission_dates[i][0],
                                 g_mission_dates[i][1]));

      fprintf(file, "%s", buffer);
    }
  }

  if ((!found) || (total_missions == 0)) {
    fclose(file);
    return NO_SUCH_ASTRONAUT;
  }

  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer,
          "Overall Summary:\n"
          "Total missions led by %s -> %d\n"
          "Total mission time of %s -> %d\n"
          "Maximum efficiency score of %s -> %f\n",
          astronaut_name, total_missions,
          astronaut_name, get_total_time(astronaut_name),
          astronaut_name, calc_efficiency_score(astronaut_name));
  fprintf(file,
          "%s", buffer);
  memset(buffer, 0, sizeof(buffer));
  int equip_idx = popular_equipment_pair_used_by(astronaut_name);
  if (equip_idx == MULTIPLE_PAIRS) {
    sprintf(buffer,
            "Most popular equipment pair used by %s -> multiple pairs\n",
            astronaut_name);

  }
  else {
    sprintf(buffer,
            "Most popular equipment pair used by %s -> %s,%s\n",
            astronaut_name,
            g_equipment[equip_idx][0],
            g_equipment[equip_idx][1]);
  }

  fprintf(file,
          "%s", buffer);

  fclose(file);

  return OK;
} /* generate_report() */