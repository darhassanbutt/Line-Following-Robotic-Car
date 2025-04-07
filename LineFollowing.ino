#include <Servo.h>

// Motor control pins
#define rightMotorPin 7
#define leftMotorPin 8
#define rightReversePin 12
#define leftReversePin 13

// Ultrasonic sensor pins
#define trigPin A1
#define echoPin A2

// Infrared sensor pins
#define irRightPin A5
#define irLeftPin A4

// Mode and power switch pins
#define modeSwitchPin 2
#define powerSwitchPin A3

// Servo motor
Servo cameraServo;

// Movement control variable
int forwardDelay = 0;

void setup() {
  Serial.begin(9600);

  pinMode(rightMotorPin, OUTPUT);
  pinMode(leftMotorPin, OUTPUT);
  pinMode(rightReversePin, OUTPUT);
  pinMode(leftReversePin, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(modeSwitchPin, INPUT);
  pinMode(powerSwitchPin, INPUT);
  pinMode(irLeftPin, INPUT);
  pinMode(irRightPin, INPUT);

  cameraServo.attach(10);
  cameraServo.write(93);  // Center position
  delay(1000);
}

void loop() {
  bool isPoweredOn = digitalRead(powerSwitchPin);
  bool isInfraredMode = digitalRead(modeSwitchPin);

  if (isPoweredOn) {
    if (isInfraredMode)
      handleInfraredMode();
    else
      handleUltrasonicMode();
  } else {
    forwardDelay = 0;
  }
}

void driveForward() {
  digitalWrite(rightMotorPin, HIGH);
  digitalWrite(leftMotorPin, HIGH);
  delay(forwardDelay);

  digitalWrite(rightMotorPin, LOW);
  digitalWrite(leftMotorPin, LOW);
  delay(200 - forwardDelay);

  if (forwardDelay < 200)
    forwardDelay += 10;
}

void driveBackward() {
  digitalWrite(rightReversePin, HIGH);
  digitalWrite(leftReversePin, HIGH);
  delay(500);

  digitalWrite(rightReversePin, LOW);
  digitalWrite(leftReversePin, LOW);
  delay(500);
}

void turnRight(int delayTime) {
  digitalWrite(rightMotorPin, LOW);
  digitalWrite(leftMotorPin, HIGH);
  delay(delayTime);

  digitalWrite(rightMotorPin, LOW);
  digitalWrite(leftMotorPin, LOW);
  delay(delayTime);
}

void turnLeft(int delayTime) {
  digitalWrite(rightMotorPin, HIGH);
  digitalWrite(leftMotorPin, LOW);
  delay(delayTime);

  digitalWrite(rightMotorPin, LOW);
  digitalWrite(leftMotorPin, LOW);
  delay(delayTime);
}

void stopMotors() {
  digitalWrite(rightMotorPin, LOW);
  digitalWrite(leftMotorPin, LOW);
  digitalWrite(rightReversePin, LOW);
  digitalWrite(leftReversePin, LOW);
  delay(500);
  forwardDelay = 0;
}

void handleInfraredMode() {
  bool isRightClear = digitalRead(irRightPin);
  bool isLeftClear = digitalRead(irLeftPin);

  if (!isRightClear && !isLeftClear)
    driveForward();
  else if (!isRightClear && isLeftClear)
    turnLeft(200);
  else if (isRightClear && !isLeftClear)
    turnRight(200);
  else
    stopMotors();
}

int measureDistance() {
  long duration;
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("\nDistance: ");
  Serial.print(distance);
  Serial.print(" cm");

  return distance;
}

int checkRightDistance() {
  cameraServo.write(170);
  delay(1000);
  int distance = measureDistance();
  cameraServo.write(93);
  delay(100);
  return distance;
}

int checkLeftDistance() {
  cameraServo.write(10);
  delay(1000);
  int distance = measureDistance();
  cameraServo.write(93);
  delay(100);
  return distance;
}

void handleUltrasonicMode() {
  int frontDistance = measureDistance();

  if (frontDistance <= 15) {
    stopMotors();
    driveBackward();
    stopMotors();

    int leftDistance = checkLeftDistance();
    delay(200);
    int rightDistance = checkRightDistance();
    delay(200);

    if (rightDistance > leftDistance)
      turnRight(500);
    else
      turnLeft(500);

    stopMotors();
  } else {
    driveForward();
  }
}
