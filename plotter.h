
#define LEFT_POT        A7
#define RIGHT_POT       A6
#define POT_OFF         45
#define POT_DEAD        5

#define CARD_IN         17
#define CARD_CS         10
#define CARD_MOSI       11
#define CARD_MISO       12
#define CARD_CLK        13

#define FILENAME        "path.bin"
#define FILEVERSION     1

#define STEP_LEFT_1     0
#define STEP_LEFT_2     2
#define STEP_LEFT_3     1
#define STEP_LEFT_4     3
#define STEP_RIGHT_1     4
#define STEP_RIGHT_2     6
#define STEP_RIGHT_3     5
#define STEP_RIGHT_4     7

#define MAXSPEED       600

#define STEPSPERIN	2610
#define STEPSAB		48348
#define STEPSA		30015
#define STEPSB		30015

//#define CORNER0   {18381, 38743}
//#define CORNER1   {35323, 17243}
//#define CORNER3   {37499, 52646}

//#define CORNER0   {20519, 36838}
//#define CORNER1   {31684, 24220}
//#define CORNER3   {37534, 49658}

#define CORNER0   {22640, 36112}
#define CORNER1   {34892, 23743}
#define CORNER3   {36829, 46482}

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

typedef enum {
  NOSDCARD,
  GETHEADER,
  GETLINE,
  NEXTPOINT,
  DONE,
} filestate;