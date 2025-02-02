/* Add any includes here */
#include "hw1.h"
#include <stdio.h>
#include <stdlib.h>

/* Define any global variables or arrays here */
int g_padovan_array[ARRAY_SIZE] = {0};


/*
 * Define swap()
 */

void swap(int *num_a, int *num_b) {

  int temp = *num_a;
  *num_a = *num_b;
  *num_b = temp;

} /* swap() */

/*
 * Define compute_padovan_sequence()
 */

int compute_padovan_sequence(int init_1, int init_2, int init_3, int limit) {

  if ((3 > limit) || (limit > ARRAY_SIZE)) {
    return PADOVAN_ERROR;
  }

  if ((init_1 <= 0) || (init_2 <= 0) || (init_3 <= 0)) {
    return PADOVAN_ERROR;
  }

  if (init_1 > init_3) {
    swap(&init_1, &init_3);
  }

  if (init_1 > init_2) {
    swap(&init_1, &init_2);
  }

  if (init_2 > init_3) {
    swap(&init_2, &init_3);
  }


  g_padovan_array[0] = init_1;
  g_padovan_array[1] = init_2;
  g_padovan_array[2] = init_3;

  for (int i = 3; i < limit; i++) {
    g_padovan_array[i] = g_padovan_array[i - 2] + g_padovan_array[i - 3];
  }

  return PADOVAN_CORRECT;

} /* compute_padovan_sequence() */

/*
 * Define check_padovan_sequence()
 */

int check_padovan_sequence(int limit) {

  if ((3 > limit) || (limit > ARRAY_SIZE)) {
    return PADOVAN_ERROR;
  }

  if ((g_padovan_array[0] <= 0) || (g_padovan_array[1] <= 0) ||
      (g_padovan_array[2] <= 0)) {
    return PADOVAN_ERROR;
  }

  if ((g_padovan_array[0] > g_padovan_array[1]) ||
      (g_padovan_array[0] > g_padovan_array[2]) ||
      (g_padovan_array[1] > g_padovan_array[2])) {
    return PADOVAN_ERROR;
  }

  for (int i = 3; i < limit; i++) {
    if (g_padovan_array[i] != g_padovan_array[i - 2] + g_padovan_array[i - 3]) {
      return i;
    }
  }

  return PADOVAN_CORRECT;

} /* check_padovan_sequence() */
