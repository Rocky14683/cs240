#ifndef HW3_H
#define HW3_H

/* Constant definitions */
#define MAX_MISSIONS          (500)
#define MAX_NAME_LEN          (100)
#define MIN_YEAR              (1980)
#define MAX_YEAR              (1989)
#define MAX_DATE_LEN          (11)
#define NUM_MISSION_COLS      (3)
#define NUM_COLS_IN_PAIR      (2)

/* Error codes */
#define OK                   (0)
#define FILE_READ_ERR        (-1)
#define FILE_WRITE_ERR       (-2)
#define OUT_OF_BOUNDS        (-3)
#define NO_DATA_POINTS       (-4)
#define BAD_DATA             (-5)
#define BAD_ROW              (-6)
#define NO_SUCH_ASTRONAUT    (-7)
#define MULTIPLE_PAIRS       (-8)
#define NO_SUCH_TYPE         (-9)

/* Function prototypes */
int read_tables(char *);
int get_total_time(char *);
int highest_success_mission();
int popular_equipment_pair();
float avg_mission_duration(char *);
char * longest_mission_astronaut();
float calc_efficiency_score(char *);
int generate_report(char *, char *);

/* Global variables */
extern int g_mission_count;
extern char g_astronaut_and_mission[MAX_MISSIONS][NUM_MISSION_COLS][MAX_NAME_LEN];
extern char g_equipment[MAX_MISSIONS][NUM_COLS_IN_PAIR][MAX_NAME_LEN];
extern float g_op_hours[MAX_MISSIONS][NUM_COLS_IN_PAIR];
extern char g_mission_dates[MAX_MISSIONS][NUM_COLS_IN_PAIR][MAX_DATE_LEN];
extern int g_experiments[MAX_MISSIONS][NUM_COLS_IN_PAIR];

#endif // HW3_H
