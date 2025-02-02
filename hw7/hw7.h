#ifndef HW7_H
#define HW7_H

/* Constant definitions */

#define MAX_FILE_LEN   (50)
#define MAX_STOPS      (500)
#define MAX_NAME_SIZE  (50)
#define MAX_LOC_SIZE   (30)
#define MAX_BUF_SIZE   (600)
#define MAX_LINKS      (15)

typedef struct stop {
    char name[MAX_NAME_SIZE];
    char location[MAX_LOC_SIZE];
    char links[MAX_LINKS][MAX_NAME_SIZE];
    int bus_frequency[MAX_LINKS];
	struct stop *link_ptrs[MAX_LINKS];
} stop_t;

/* Error codes */

#define OK             ( 0)
#define FILE_READ_ERR  (-1)
#define BAD_RECORD     (-2)
#define TOO_MUCH_DATA  (-3)
#define NO_DATA        (-4)
#define NOT_FOUND      (-5)
#define PATH_IS_CYCLE  (-6)

/* Function prototypes */

int read_stops(char *);
int connect_stops(void);
int longest_min_freq_path(stop_t *);
int hubs_in_max_freq_path(stop_t *, char *);

/* Global variables */

extern int g_stop_count;
extern stop_t g_stop_array[MAX_STOPS];

#endif // HW7_H
