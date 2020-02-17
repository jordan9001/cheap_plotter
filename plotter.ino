#include <AccelStepper.h>
#include <MultiStepper.h>
#include <SPI.h>
#include <SD.h>

#include <stdint.h>

#include "plotter.h"

int32_t ablen = STEPSAB;
int32_t starta = STEPSA;
int32_t startb = STEPSB;

// len positions for the 3 relevent corners
// TODO give these good defaults
lencoord corner0 = CORNER0; // Top Left
lencoord corner1 = CORNER1; // Top Right
lencoord corner3 = CORNER3; // Bottom Left

ptcoord ptcorner0;
ptcoord ptcorner1;
ptcoord ptcorner3;

AccelStepper left(AccelStepper::HALF4WIRE, STEP_LEFT_1, STEP_LEFT_2, STEP_LEFT_3, STEP_LEFT_4);
AccelStepper right(AccelStepper::HALF4WIRE, STEP_RIGHT_1, STEP_RIGHT_2, STEP_RIGHT_3, STEP_RIGHT_4);
MultiStepper ctrl;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(LEFT_POT, INPUT);
  pinMode(RIGHT_POT, INPUT);

  initsteppers();
  initcoords();
  initcard();
}

void loop() {
  int stat = 0;

  stat = file_loop();

  if (stat == 1) {
    manual_loop();
  } else if (stat == 0) {
    // end, done
    gotol((lencoord){starta, startb});

    while (1) {
      delay(1000);
    }
  } else {
    delay(3000);
  }
}

#define READ_ITEM(var, f, max)   do {\
                              if (max != 0 && f.position() >= max) {\
                                Serial.println("Went over file size while trying to read " #var);\
                                goto END_CLOSE;\
                              }\
                              if (-1 == f.read(&var, sizeof(var))) {\
                                Serial.println("Ran out of file while reading " #var);\
                                goto END_CLOSE;\
                              }\
                            } while(0)

int file_loop() {
  int ret = -1;
  filestate fstate = NOSDCARD;
  File fp;
  uint32_t version = 0;
  uint32_t fsz = 0;
  uint32_t numlines = 0;
  uint32_t numpts = 0;
  uint16_t px = 0;
  uint16_t py = 0;
  pcoord p;

  uint32_t line = 0;
  uint32_t pn = 0;

  // wait until card is inserted
  if (digitalRead(CARD_IN) == LOW) {
    // card is not inserted, continue
    ret = 1;
    goto END;
  }

  if (!SD.begin(CARD_CS)) {
    Serial.println("Unable to begin SD");
    goto END;
  }

  fp = SD.open(FILENAME, FILE_READ);

  if (!fp) {
    Serial.println("Unable to find file by that name");
    goto END;
  }

  fstate = GETHEADER;

  while (fstate != DONE) {
    switch (fstate) {
    case GETHEADER:
      READ_ITEM(version, fp, fsz);
      if (version != FILEVERSION) {
        Serial.print("Bad file version! got version 0x");
        Serial.println(version, HEX);
        goto END_CLOSE;
      }

      READ_ITEM(fsz, fp, fsz);
      READ_ITEM(numlines, fp, fsz);

      Serial.print("Executing path with ");
      Serial.print(numlines);
      Serial.print(" lines. File size is 0x");
      Serial.println(fsz, HEX);

      fstate = GETLINE;
      break;
    case GETLINE:
      // Pen up
      setPressure(-1);

      if (line >= numlines) {
        fstate = DONE;
        break;
      }

      READ_ITEM(numpts, fp, fsz);
      line++;
      pn = 0;



      fstate = NEXTPOINT;
      break;
    case NEXTPOINT:
      if (pn >= numpts) {
        fstate = GETLINE;
        break;
      }

      READ_ITEM(px, fp, fsz);
      READ_ITEM(py, fp, fsz);
      p.x = px;
      p.y = py;
      pn++;

      Serial.print("line ");
      Serial.print(line);
      Serial.print("/");
      Serial.print(numlines);
      Serial.print("\tp ");
      Serial.print(pn);
      Serial.print("/");
      Serial.print(numpts);
      Serial.print("\t-\t(");
      Serial.print(p.x);
      Serial.print(",\t");
      Serial.print(p.y);
      Serial.println(")");

      // goto point
      gotop(p);

      if (pn == 1) {
        // Pen down
        setPressure(1);
      }
      break;
    default:
      Serial.println("Something bad happened in the state machine");
      goto END_CLOSE;
    }
  }

  if (fp.position() != fsz) {
    Serial.println("Uh oh: file size doesn't match our position");
  }

  Serial.println("Successfully handled the path");
  ret = 0;  

END_CLOSE:
  fp.close();

END:

  return ret;
}

int manual_loop() {
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
    
    l *= (MAXSPEED / POT_OFF);
    r *= (MAXSPEED / POT_OFF);

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

      
      Serial.print("ln (");
      Serial.print(ln.a);Serial.print(", ");
      Serial.print(ln.b);Serial.print(")\t p~ (");
      Serial.print(fakep.x);Serial.print(", ");
      Serial.print(fakep.y);Serial.println(")");
      

      next_print = millis() + 1200;
    }
  }

  left.runSpeed();
  right.runSpeed();

  return 0;
}

void initcard() {
  pinMode(CARD_IN, INPUT);
}

void initcoords() {
  //TODO initialize the corners, ablen, ablensq, etc.
  //TODO use reusable setting functions, so things can be changed at runtime
  ptcorner0 = len2pt(corner0);
  ptcorner1 = len2pt(corner1);
  ptcorner3 = len2pt(corner3);

  Serial.print("Pt corners at (0) (");
  Serial.print(ptcorner0.x);
  Serial.print(", ");
  Serial.print(ptcorner0.y);
  Serial.print(" (1) (");
  Serial.print(ptcorner1.x);
  Serial.print(", ");
  Serial.print(ptcorner1.y);
  Serial.print(" (3) (");
  Serial.print(ptcorner3.x);
  Serial.print(", ");
  Serial.print(ptcorner3.y);
  Serial.println(")");
}

void initsteppers() {
  left.setSpeed(0);
  right.setSpeed(0);
  left.setMaxSpeed(MAXSPEED);
  right.setMaxSpeed(MAXSPEED);

  ctrl.addStepper(left);
  ctrl.addStepper(right);
}

void setPressure(int pressure) {
  //TODO actually support pressure
  //TODO actually lift pen with servo

  Serial.println((pressure < 0) ? "PEN UP" : "PEN DOWN");

  Serial.println("enter to continue");

  while (1) {
    while (!Serial.available()) {
      delay(600);
    }
    if (Serial.read() == '\n') {
      break;
    }
  }
}

void gotol(lencoord ln) {
  long dest[2];

  dest[0] = starta - ln.a;
  dest[1] = startb - ln.b;

  ctrl.moveTo(dest);

  //DEBUG
  /*
  Serial.println("BREAK");
  while (Serial.read() != '\n') {
    delay(600);
  }

  long next_print = 0;
  long t = 0;
  while(ctrl.run()) {
    t = millis();
    if (next_print <= t) {
      Serial.print("\t\t at ");
      Serial.print(starta - left.currentPosition());
      Serial.print(", ");
      Serial.print(startb - right.currentPosition());
      Serial.print("\t -> ");
      Serial.print(starta - dest[0]);
      Serial.print(", ");
      Serial.print(startb - dest[1]);
      Serial.println();
      
      next_print = t + 600;
    }
  }
  */
  ctrl.runSpeedToPosition();
}

void gotop(pcoord p) {
  lencoord ln = p2len(p);
  gotol(ln);
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
  int64_t a;
  int64_t b;
  int64_t ys = sq((int64_t)pt.y);
  a = sqrt(sq((int64_t)pt.x) + ys);
  b = sqrt(sq((int64_t)ablen - (int64_t)pt.x) + ys);
  
  ret.a = (int32_t)a;
  ret.b = (int32_t)b;
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
  ret.y += (p.y * (ptcorner3.y - ptcorner0.y)) / MAX_P;
  ret.y += (p.x * (ptcorner1.y - ptcorner0.y)) / MAX_P;
  
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
