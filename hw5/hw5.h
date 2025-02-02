#ifndef HW5_H
#define HW5_H

#include <stdio.h>

/* Constant definitions */
#define MAX_NAME_LEN    (20)
#define N_ROUTES        (15)
#define MAX_ROUTE_LEN   (10)

/* Error codes */
#define OK                 (-1)
#define NO_ITEM            (-2)
#define WRITE_ERR          (-3)
#define NO_SPACE           (-4)
#define MALFORMED_ITEM     (-5)
#define BAD_ID             (-6)
#define FREE_ID            (-7)
#define ALREADY_BALANCED   (-8)
#define NO_ROUTE           (-9)

#define BAD_ITEM ((item_t) {\
  .id = BAD_ID, \
})


/* Structure Declarations */
enum status_t{
    AVAILABLE,
    IN_USE,
    EXPIRED
};

typedef struct {
    int id;
    char station_code;
    char name[MAX_NAME_LEN];
    char category[MAX_NAME_LEN];
    enum status_t status;
    float mass;
    int manufacture_date;
} item_t;

/* Function prototypes */
item_t read_item(FILE *, int);
int write_item(FILE *, item_t, int);
int add_item(FILE *, item_t);
int decommission_category(FILE *, char *);
int find_optimal_route(item_t, char, char[N_ROUTES][MAX_ROUTE_LEN]);
item_t find_item_by_id(FILE *, int);
int find_expired_items(FILE *, int);
int mark_used(FILE *, int);
float balance_stations(FILE *);

#endif // HW5_H
