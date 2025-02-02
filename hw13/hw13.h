#ifndef HW13_H
#define HW13_H

#include "graphics.h"
#include "object.h"

object *read_object(const char *filename);
void free_object(object *obj);

#endif // HW13_H