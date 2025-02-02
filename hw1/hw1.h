#ifndef HW1_H
#define HW1_H

#define ARRAY_SIZE 40
#define MIN_SIZE (3)
extern int g_padovan_array[ARRAY_SIZE];

#define PADOVAN_ERROR (-1)
#define PADOVAN_CORRECT (0)

int compute_padovan_sequence(int, int, int, int);
int check_padovan_sequence(int);

#endif // HW1_H
