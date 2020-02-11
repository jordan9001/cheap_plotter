
#define LEFT_POT        A7
#define RIGHT_POT       A6
#define POT_OFF         45
#define POT_DEAD        5

#define STEP_LEFT_1     0
#define STEP_LEFT_2     2
#define STEP_LEFT_3     1
#define STEP_LEFT_4     3
#define STEP_RIGHT_1     4
#define STEP_RIGHT_2     6
#define STEP_RIGHT_3     5
#define STEP_RIGHT_4     7

#define MAX_SPEED       900

#define STEPSPERIN	2610
#define STEPSAB		48348
#define STEPSA		30015
#define STEPSB		30015

// coordinates based on band length
typedef struct {
  int32_t a;
  int32_t b;
} lencoord;

// cartesian coords in step size units
// we denote proportional cartesian with p, vs pt being in step size units
typedef struct {
  int32_t x;
  int32_t y;
} ptcoord;

#define MAX_P   10000
// How it comes in from the file as a proportion of MAX_P
typedef struct {
  int32_t x;
  int32_t y;
} pcoord;

