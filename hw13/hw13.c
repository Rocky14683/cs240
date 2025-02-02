/* Name, hw13.c, CS 24000, Spring 2024
 */

/* Add any includes here */

#include "hw13.h"

#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


/*
 * Read an objects from a file
 */

object *read_object(const char *filename) {
  assert(filename);
  FILE *file = fopen(filename, "r");
  if (!file) {
    return NULL;
  }

  object *obj = calloc(1, sizeof(object));
  assert(obj);

  char name[256] = {0};
  if (fscanf(file, "%255s", name) != 1) {
    free_object(obj);
    fclose(file);
    file = NULL;
    return NULL;
  }

  if (fscanf(file, "%d %d", &obj->num_points, &obj->num_polygons) != 2) {
    free_object(obj);
    fclose(file);
    file = NULL;
    return NULL;
  }


  obj->points = calloc(obj->num_points, sizeof(point));
  assert(obj->points);
  bool start_from_zero = false;
  for (int i = 0; i < obj->num_points; ++i) {
    int idx = 0;
    int ret = fscanf(file, "%d %lf %lf %lf", &idx, &obj->points[i].arr[0],
                     &obj->points[i].arr[1], &obj->points[i].arr[2]);
    if (i == 0) {
      if (idx == 0) {
        start_from_zero = true;
      }
      else if (idx == 1) {
        start_from_zero = false;
      }
      else {
        free_object(obj);
        fclose(file);
        file = NULL;
        return NULL;
      }
    }

    if (ret != 4) {
      free_object(obj);
      fclose(file);
      file = NULL;
      return NULL;
    }

    if (((idx != i + 1) && !(start_from_zero)) ||
        ((idx != i) && (start_from_zero))) {
      free_object(obj);
      fclose(file);
      file = NULL;
      return NULL;
    }
  }

  obj->polygons = calloc(obj->num_polygons, sizeof(polygon));
  assert(obj->polygons);
  for (int i = 0; i < obj->num_polygons; ++i) {
    char poly_name[256] = {0};
    char color_code[8] = {0};
    if (fscanf(file, "%255s %7s %d", poly_name, color_code,
               &obj->polygons[i].num_points) != 3) {
      free_object(obj);
      fclose(file);
      file = NULL;
      return NULL;
    }

    name_to_rgb(color_code, &obj->polygons[i].r, &obj->polygons[i].g,
                &obj->polygons[i].b);

    obj->polygons[i].point_arr = calloc(obj->polygons[i].num_points,
                                        sizeof(point *));
    assert(obj->polygons[i].point_arr);

    for (int j = 0; j < obj->polygons[i].num_points; ++j) {
      int index = 0;
      if (fscanf(file, "%d", &index) != 1) {
        free_object(obj);
        fclose(file);
        file = NULL;
        return NULL;
      }
      if (start_from_zero) {
        obj->polygons[i].point_arr[j] = &obj->points[index];
      }
      else {
        obj->polygons[i].point_arr[j] = &obj->points[index - 1];
      }
    }
  }

  fclose(file);
  file = NULL;
  return obj;
} /* read_object() */

/*
 * Free the object
 */

void free_object(object *obj) {
  if (!obj) {
    return;
  }

  if (obj->polygons) {
    for (int i = 0; i < obj->num_polygons; ++i) {
      if (obj->polygons[i].point_arr) {
        free(obj->polygons[i].point_arr);
        obj->polygons[i].point_arr = NULL;
      }
    }
    free(obj->polygons);
    obj->polygons = NULL;
  }

  free(obj->points);
  obj->points = NULL;
  free(obj);
  obj = NULL;
} /* free_object() */