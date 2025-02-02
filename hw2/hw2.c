/* HW2 Template - Fall 2024 */

#include "hw2.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>


typedef struct dorm_data {
  int year;
  int month;
  int day;
  char dorm_name[MAX_NAME_LENGTH];
  int residents;
  float water;
  int electricity;
  int steam;
  int trash;
}

dorm_data_t;


/**
 * define is_valid_date
 */

bool is_valid_date(int year, int month, int day) {
  return (month >= 1) && (month <= 12) && (day >= 1) &&
         (day <= 30) && (year >= 0);
} /* is_valid_date() */

/*
 * define validate
 */

int validate(char *in_file) {
  FILE *file_ptr = fopen(in_file, "r");
  if (file_ptr == NULL) {
    return FILE_READ_ERR;
  }

  dorm_data_t data = {};
  bool entered = false;

  while (true) {
    int scan_status = fscanf(file_ptr, "%d-%d-%d,\"%49[^\"]\",%d\\%f,%d,%d,%d",
                             &data.year, &data.month, &data.day, data.dorm_name,
                             &data.residents, &data.water, &data.electricity,
                             &data.steam, &data.trash);

    if (scan_status == EOF) {
      if (!entered) {
        fclose(file_ptr);
        return NO_DATA_POINTS;
      }
      break;
    }
    else if (scan_status != 9) {
      fclose(file_ptr);
      return BAD_RECORD;
    }

    if ((data.residents < 0) || (data.water < 0) ||
        (data.electricity < 0) || (data.steam < 0) || (data.trash < 0)) {
      fclose(file_ptr);
      return BAD_RECORD;
    }

    if (!is_valid_date(data.year, data.month, data.day)) {
      fclose(file_ptr);
      return BAD_DATE;
    }

    entered = true;
  }

  fclose(file_ptr);
  return SUCCESS;
} /* validate() */


/*
 * define calculate_bill here
 */

double calculate_bill(char *in_file, char *dorm) {
  dorm_data_t data = {};
  double bill = 0;
  int status = validate(in_file);
  if (status != SUCCESS) {
    return status;
  }

  FILE *file_ptr = fopen(in_file, "r");
  if (file_ptr == NULL) {
    return FILE_READ_ERR;
  }

  int entries = 0;
  while (fscanf(file_ptr,
                "%d-%d-%d,\"%49[^\"]\",%d\\%f,%d,%d,%d",
                &data.year, &data.month, &data.day, data.dorm_name,
                &data.residents, &data.water, &data.electricity, &data.steam,
                &data.trash) != EOF) {
    if (strcmp(data.dorm_name, dorm) == 0) {
      bill += (1.065 * data.water) + (0.0984 * data.electricity) +
              (0.06 * data.steam) + (0.04 * data.trash);
      entries++;
    }
  }

  fclose(file_ptr);

  if (!entries) {
    return NO_DATA_POINTS;
  }
  return bill;
} /* calculate_bill() */

/*
 * define generate_monthly_water_report here
 */

int generate_monthly_water_report(
char *in_file, char *dorm, int year, char *out_file) {
  dorm_data_t data = {};
  int status = validate(in_file);
  if (status != SUCCESS) {
    return status;
  }

  if (year < 0) {
    return BAD_DATE;
  }

  FILE *in_file_ptr = fopen(in_file, "r");
  if (in_file_ptr == NULL) {
    return FILE_READ_ERR;
  }

  FILE *out_file_ptr = fopen(out_file, "w");
  if (out_file_ptr == NULL) {
    fclose(in_file_ptr);
    return FILE_WRITE_ERR;
  }

  float sum[12] = {.0f};
  int entries = 0;
  fprintf(out_file_ptr, "%s\n", dorm);
  while (fscanf(in_file_ptr,
                "%d-%d-%d,\"%49[^\"]\",%d\\%f,%d,%d,%d",
                &data.year, &data.month, &data.day, data.dorm_name,
                &data.residents, &data.water, &data.electricity,
                &data.steam,
                &data.trash) != EOF) {


    if ((strcmp(data.dorm_name, dorm) == 0) && (data.year == year)) {
      sum[data.month - 1] += data.water;
      entries++;
    }
  }

  for (int i = 0; i < 12; i++) {
    fprintf(out_file_ptr, "%02d:%.3f\n", i + 1, sum[i]);
  }

  fprintf(out_file_ptr, "%d\n", year);

  fclose(in_file_ptr);
  fclose(out_file_ptr);

  if (entries == 0) {
    return NO_DATA_POINTS;
  }

  return SUCCESS;
} /* generate_monthly_water_report() */

/*
 * define get_electricity_usage here
 */

int get_electricity_usage(char *in_file, char *dorm, int year) {
  dorm_data_t data = {};

  if (year < 0) {
    return BAD_DATE;
  }

  int status = validate(in_file);
  if (status != SUCCESS) {
    return status;
  }

  FILE *in_file_ptr = fopen(in_file, "r");
  if (in_file_ptr == NULL) {
    return FILE_READ_ERR;
  }

  int sum = 0;
  int entries = 0;
  while (fscanf(in_file_ptr,
                "%d-%d-%d,\"%49[^\"]\",%d\\%f,%d,%d,%d",
                &data.year, &data.month, &data.day, data.dorm_name,
                &data.residents, &data.water, &data.electricity,
                &data.steam,
                &data.trash) != EOF) {

    if ((strcmp(data.dorm_name, dorm) == 0) && (data.year == year)) {
      sum += data.electricity;
      entries++;
    }
  }
  fclose(in_file_ptr);

  if (entries == 0) {
    return NO_DATA_POINTS;
  }

  return sum;
} /* get_electricity_usage() */

/*
 * define get_average_resident_water_usage here
 */

double get_average_resident_water_usage(char *in_file, int month, int year) {
  dorm_data_t data = {};

  if (!is_valid_date(year, month, 1)) {
    return BAD_DATE;
  }

  int status = validate(in_file);
  if (status != SUCCESS) {
    return status;
  }
  FILE *in_file_ptr = fopen(in_file, "r");
  if (in_file_ptr == NULL) {
    return FILE_READ_ERR;
  }
  int entries = 0;
  double avg_sum = 0;

  while (fscanf(in_file_ptr,
                "%d-%d-%d,\"%49[^\"]\",%d\\%f,%d,%d,%d",
                &data.year, &data.month, &data.day, data.dorm_name,
                &data.residents, &data.water, &data.electricity,
                &data.steam,
                &data.trash) != EOF) {

    if ((data.month == month) && (data.year == year)) {
      if (data.residents == 0) {
        fclose(in_file_ptr);
        return NO_RESIDENTS;
      }
      avg_sum += (double) data.water / (double) data.residents;
      entries++;
    }
  }
  fclose(in_file_ptr);

  if (entries == 0) {
    return NO_DATA_POINTS;
  }

  return avg_sum / (double) entries;
} /* get_average_resident_water_usage() */