/*
 * OntarioSkills Competition Robot
 * -------------------------------
 * Differential drivetrain plus a motorised claw that grabs, lifts,
 * carries and releases boxes around a set course.
 *
 * The two things that took the longest to get right are both encoded
 * here as constants: GRIP_CLOSED (how hard the claw squeezes) and the
 * lift timing, because a claw that closes too gently drops the box
 * mid-lift and one that closes too hard stalls the servo.
 *
 * Board: Arduino Uno · Driver: L298N
 *
 * Castlebrooke Robotics Team — drivetrain and control logic by Jashan Multani
 */

#include <Servo.h>

// ---------------------------------------------------------------- pins
const uint8_t ENA = 5, IN1 = 6,  IN2 = 7;    // left drive
const uint8_t ENB = 11, IN3 = 8, IN4 = 12;   // right drive
const uint8_t CLAW_PIN = 9;
const uint8_t LIFT_PIN = 10;

/* ------------------------------------------------------------- tuning
 * GRIP_CLOSED was the fix for the claw dropping boxes mid-lift. The
 * first value let the box slip the moment the arm started moving; too
 * far the other way and the servo stalls and buzzes. This is the angle
 * that holds without straining.
 */
const uint8_t GRIP_OPEN    = 20;
const uint8_t GRIP_CLOSED  = 68;

const uint8_t LIFT_DOWN    = 25;
const uint8_t LIFT_CARRY   = 80;

/* Weight distribution was the other problem: under a heavy box the
 * whole robot tilted forward like it was about to face-plant. We
 * reinforced the chassis, and capped drive power while carrying so it
 * doesn't lurch on acceleration. */
const uint8_t SPEED_NORMAL  = 180;
const uint8_t SPEED_LOADED  = 120;   // while a box is in the claw
const uint8_t SPEED_TURN    = 150;

const uint16_t GRIP_SETTLE_MS = 400; // let the claw close BEFORE lifting
const uint16_t LIFT_STEP_MS   = 12;  // per degree — slow lift, less tilt

Servo claw, lift;
bool carrying = false;

void setup() {
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  claw.attach(CLAW_PIN); lift.attach(LIFT_PIN);
  claw.write(GRIP_OPEN); lift.write(LIFT_DOWN);
  Serial.begin(9600);
  delay(800);
}

// ------------------------------------------------------------- drive
void drive(int l, int r) {
  digitalWrite(IN1, l >= 0); digitalWrite(IN2, l < 0);
  digitalWrite(IN3, r >= 0); digitalWrite(IN4, r < 0);
  analogWrite(ENA, constrain(abs(l), 0, 255));
  analogWrite(ENB, constrain(abs(r), 0, 255));
}
void halt() { drive(0, 0); }

uint8_t cruise() { return carrying ? SPEED_LOADED : SPEED_NORMAL; }

void forward(uint16_t ms) { drive(cruise(), cruise()); delay(ms); halt(); }
void back(uint16_t ms)    { drive(-cruise(), -cruise()); delay(ms); halt(); }
void turnLeft(uint16_t ms)  { drive(-SPEED_TURN,  SPEED_TURN); delay(ms); halt(); }
void turnRight(uint16_t ms) { drive( SPEED_TURN, -SPEED_TURN); delay(ms); halt(); }

// --------------------------------------------------------------- claw
void sweep(Servo &s, uint8_t target, uint16_t stepMs) {
  int pos = s.read();
  int dir = (target > pos) ? 1 : -1;
  while (pos != target) { pos += dir; s.write(pos); delay(stepMs); }
}

void grab() {
  sweep(claw, GRIP_CLOSED, 6);
  delay(GRIP_SETTLE_MS);        // the fix: settle BEFORE taking the weight
  sweep(lift, LIFT_CARRY, LIFT_STEP_MS);
  carrying = true;
  Serial.println(F("carrying"));
}

void release() {
  sweep(lift, LIFT_DOWN, LIFT_STEP_MS);
  delay(150);
  sweep(claw, GRIP_OPEN, 6);
  carrying = false;
  Serial.println(F("released"));
}

// --------------------------------------------------- example routine
// A pick-and-place pass. Distances are timed for our chassis; on a
// different build every one of these numbers changes.
void pickAndPlace() {
  forward(900);
  grab();
  turnRight(420);
  forward(1200);
  release();
  back(600);
}

void loop() {
  pickAndPlace();
  delay(3000);
}
