#ifndef HW9_H
#define HW9_H

/* Error codes */
#define OK                    (0)
#define NO_SUCH_PLANET        (-1)
#define NO_SUCH_RESOURCE      (-2)

 
/* Structure Definitions */
typedef struct resource_struct {
  char *name;
  double volume;
  struct {
    int dollars;
    int cents;
  } value;
  struct resource_struct *prev_resource;
  struct resource_struct *next_resource;
} resource_t;

typedef struct planet_struct {
  char *name;
  int planet_id;
  struct resource_struct *resource_list;
  struct spaceship_struct *spaceship_list;
  struct planet_struct *prev_planet;
  struct planet_struct *next_planet;
} planet_t;

typedef struct spaceship_struct {
  char *name;
  int spaceship_id;
  double fuel_capacity;
  struct spaceship_struct *prev_ship;
  struct spaceship_struct *next_ship;
} spaceship_t;

/* Function prototypes */
void create_planet(char *, int, planet_t **);
void add_resource(planet_t *, char *, double, int, int);
void add_planet(planet_t **, planet_t *);
void destroy_planet(planet_t **);
int remove_resource(planet_t *, char *);
int move_spaceship(spaceship_t *, int, planet_t **);
int merge_resources(planet_t *, planet_t *);
int rotate_planets(planet_t **, int);
planet_t * find_planet(planet_t *, spaceship_t *);
void apocalypse(planet_t **);


#endif // HW9_H
