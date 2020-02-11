#include <AccelStepper.h>
#include <stdint.h>

#include "plotter.h"

int32_t ablen = STEPSAB;
int32_t starta = STEPSA;
int32_t startb = STEPSB;

// len positions for the 3 relevent corners
// TODO give these good defaults
lencoord corner0 = {18381, 38743}; // Top Left
lencoord corner1 = {35323, 17243}; // Top Right
lencoord corner3 = {37499, 52646}; // Bottom Left

ptcoord ptcorner0;
ptcoord ptcorner1;
ptcoord ptcorner3;

AccelStepper left(AccelStepper::HALF4WIRE, STEP_LEFT_1, STEP_LEFT_2, STEP_LEFT_3, STEP_LEFT_4);
AccelStepper right(AccelStepper::HALF4WIRE, STEP_RIGHT_1, STEP_RIGHT_2, STEP_RIGHT_3, STEP_RIGHT_4);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(LEFT_POT, INPUT);
  pinMode(RIGHT_POT, INPUT);

  left.setSpeed(0);
  right.setSpeed(0);
  left.setMaxSpeed(900);
  right.setMaxSpeed(900);
  
  initcoords();
}

void loop() {
  manual_loop();
}

void manual_loop() {
  // put your main code here, to run repeatedly:
  static unsigned long next_read = 0;
  static unsigned long next_print = 0;
  static int l = 0;
  static int r = 0;

  if (next_read <= millis()) {
    l = analogRead(LEFT_POT) - POT_OFF;
    r = analogRead(RIGHT_POT) - POT_OFF;

    if (l < POT_DEAD && l > -POT_DEAD) {
      l = 0;
    }

    if (r < POT_DEAD && r > -POT_DEAD) {
      r = 0;
    }
    
    l *= (MAX_SPEED / POT_OFF);
    r *= (MAX_SPEED / POT_OFF);

    left.setSpeed(-l);
    right.setSpeed(r);

    next_read = millis() + 100;

    if (next_print <= millis()) {
      lencoord ln;
      ptcoord pt;
      pcoord fakep;
      ln.a = starta - left.currentPosition();
      ln.b = startb - right.currentPosition();
      pt = len2pt(ln);
      fakep = fake_pt2p(pt);

      
      //Serial.print(l);Serial.print(" ");
      //Serial.print(r);Serial.print("\t");
      //Serial.print(ln.a);Serial.print(" ");
      //Serial.print(ln.b);Serial.print("\t");
      Serial.print(fakep.x);Serial.print(" ");
      Serial.print(fakep.y);Serial.println();
      

      next_print = millis() + 100;
    }
  }

  

  left.runSpeed();
  right.runSpeed();
}



// helper functions for going to/from different coordinate spaces


void initcoords() {
  //TODO initialize the corners, ablen, ablensq, etc.
  //TODO use reusable setting functions, so things can be changed at runtime
  ptcorner0 = len2pt(corner0);
  ptcorner1 = len2pt(corner1);
  ptcorner3 = len2pt(corner3);
}

ptcoord len2pt(lencoord ln) {
  // given we know the length of the distance between the steppers, and the length of the belts to the pen
  // workout the cartesian point
  // the lengths of the belts are the hypotenuses
  ptcoord ret;

  // we have to be careful not to get int overflow here, use 64 bit ints for intermediates
  int64_t lasq;
  int64_t y;
  int64_t x;
  
  lasq = sq((int64_t)ln.a);
  
  x = lasq - sq((int64_t)ln.b);
  x /= (((int64_t)ablen));
  x += ((int64_t)ablen);
  x /= 2;
  y = sqrt(lasq - sq(x));
  
  ret.x = x;
  ret.y = y;

  return ret;
}

lencoord pt2len(ptcoord pt) {
  // A is at 0,0 and B is at ablen, 0
  lencoord ret;
  int32_t ys = sq(pt.y);
  ret.a = sqrt(sq(pt.x) + ys);
  ret.b = sqrt(sq(ablen - pt.x) + ys);
  
  return ret;
}

ptcoord p2pt(pcoord p) {
  // linearly interpolate
  // can handle rotated parallelagrams even
  // maybe we should just treat as a rectangle and save computation?
  ptcoord ret;

  ret.x = ptcorner0.x;
  ret.x += (p.x * (ptcorner1.x - ptcorner0.x)) / MAX_P;
  ret.x += (p.y * (ptcorner3.x - ptcorner0.x)) / MAX_P;
  
  ret.y = ptcorner0.y;
  ret.x += (p.y * (ptcorner3.y - ptcorner0.y)) / MAX_P;
  ret.x += (p.x * (ptcorner1.y - ptcorner0.y)) / MAX_P;
  
  return ret;
}

pcoord fake_pt2p(ptcoord pt) {
  pcoord ret;

  // doing it for real would be intense...
  // we only need this for readouts anyways
  // so here we just assume a alligned rectangle
  // which we don't do in p2pt
  ret.x = pt.x - ptcorner0.x;
  ret.x = ret.x * MAX_P / (ptcorner1.x - ptcorner0.x);

  ret.y = pt.y - ptcorner0.y;
  ret.y = ret.y * MAX_P / (ptcorner3.y - ptcorner3.x);
  
  return ret;
}

lencoord p2len(pcoord p) {
  return pt2len(p2pt(p));
}
