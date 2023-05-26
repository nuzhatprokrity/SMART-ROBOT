#include <SoftwareSerial.h>
#include <AFMotor.h>

const int IRsensorLeft = A0; // Left IR Sensor
const int IRsensorRight = A1; // Right IR Sensor
const int echoPin = A2; // Echo Pin of Ultrasonic Sensor
const int trigPin = A3; // Trigger Pin of Ultrasonic Sensor
const int OperationModePin = A4; // Mode selection pin for the robot

SoftwareSerial BT_Serial(9, 10); // RX, TX

AF_DCMotor motorL(3, MOTOR12_1KHZ);
AF_DCMotor motorR(4, MOTOR12_1KHZ);


void setup() {
  Serial.begin(38400);
  BT_Serial.begin(9600);
  delay(1000);

  pinMode(IRsensorLeft, INPUT);
  pinMode(IRsensorRight, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(OperationModePin, INPUT_PULLUP);

  motorL.setSpeed(255);
  motorR.setSpeed(255);
}


void loop() {
  int modeStatus = analogRead(OperationModePin);
  Serial.println(modeStatus);

  if (modeStatus >= 0 && modeStatus <= 50) manualControl();
  else if (modeStatus >= 360 && modeStatus <= 420) voiceControl();
  else if (modeStatus >= 590 && modeStatus <= 650) obstacleAvoid();
  else if (modeStatus >= 780 && modeStatus <= 840) followObject();
  else robotStop();
}


void manualControl() { // manual control function
  char robotDirection;
  motorL.setSpeed(255);
  motorR.setSpeed(255);

  if (BT_Serial.available() > 0) {
    robotDirection = BT_Serial.read();
    Serial.write(robotDirection);
    Serial.println();
  }

  switch (robotDirection)
  {
    case 'F':
      robotForward();
      break;

    case 'B':
      robotBackward();
      break;

    case 'L':
      robotLeft();
      break;

    case 'R':
      robotRight();
      break;

    case 'G':
      robotSharpLeft();
      break;

    case 'I':
      robotSharpRight();
      break;

    case 'H':
      robotSharpRight();
      break;

    case 'J':
      robotSharpLeft();
      break;

    case 'S':
      robotStop();
      break;

    default:
      robotStop();
  }
}


void voiceControl() { // voice control function
  char voiceCommand = 0;
  motorL.setSpeed(255);
  motorR.setSpeed(255);

  if (BT_Serial.available() > 0) {
    voiceCommand = BT_Serial.read();
    Serial.write(voiceCommand);
    Serial.println();
  }

  int tempDistance = findDistance();
  int IRleftDistance = digitalRead(IRsensorLeft);
  int IRrightDistance = digitalRead(IRsensorRight);
  Serial.println(tempDistance);

  if ((tempDistance <= 15 || IRleftDistance == 0 || IRrightDistance == 0) && voiceCommand == 'W') robotStop();
  else {
    switch (voiceCommand)
    {
      case 'W':
        robotForward();
        delay(1500);
        robotStop();
        break;

      case 'Z':
        robotBackward();
        delay(1500);
        robotStop();
        break;

      case 'A':
        robotLeft();
        delay(600);
        robotStop();
        break;

      case 'D':
        robotRight();
        delay(600);
        robotStop();
        break;

      case 'Q':
        robotSharpLeft();
        delay(400);
        robotStop();
        break;

      case 'E':
        robotSharpRight();
        delay(400);
        robotStop();
        break;

      case 'X':
        robotStop();
        break;

      default:
        robotStop();
    }
  }
}


void obstacleAvoid() { // obstacle avoid function
  int obstacleDistance = 15;
  motorL.setSpeed(135); // 135 left
  motorR.setSpeed(120); // 120 right, because right motor has more speed

  int tempDistance = findDistance();
  int IRleftDistance = digitalRead(IRsensorLeft);
  int IRrightDistance = digitalRead(IRsensorRight);
  Serial.println(tempDistance);

  if (tempDistance > obstacleDistance && IRleftDistance == 1 && IRrightDistance == 1) {
    robotForward();
  }
  else if (tempDistance <= obstacleDistance && IRleftDistance == 0 && IRrightDistance == 0) {
    robotStop();
    delay(200);
    robotBackward();
    delay(400);
    robotStop();
    delay(200);
    robotSharpRight();
    delay(1000);
    robotStop();
    delay(200);
  }
  else if (tempDistance <= obstacleDistance || IRleftDistance == 0 ) {
    robotStop();
    delay(200);
    robotBackward();
    delay(400);
    robotStop();
    delay(200);
    robotSharpRight();
    delay(500);
    robotStop();
    delay(200);
  }
  else if (tempDistance <= obstacleDistance || IRrightDistance == 0 ) {
    robotStop();
    delay(200);
    robotBackward();
    delay(400);
    robotStop();
    delay(200);
    robotSharpLeft();
    delay(500);
    robotStop();
    delay(200);
  }
  else {
    robotStop();
    delay(200);
    robotBackward();
    delay(1000);
    robotStop();
    delay(200);
    robotSharpRight();
    delay(1000);
    robotStop();
    delay(200);
  }
}


void followObject() { // Object following function
  int objectDistanceMin = 7;
  int objectDistanceMax = 30;
  motorL.setSpeed(200); // 200 left
  motorR.setSpeed(178); // 178 right, because right motor has more speed

  int tempDistance = findDistance();
  int IRleftDistance = digitalRead(IRsensorLeft);
  int IRrightDistance = digitalRead(IRsensorRight);
  Serial.println(tempDistance);

  if (tempDistance > objectDistanceMin && tempDistance <= objectDistanceMax)robotForward();
  else if (tempDistance > objectDistanceMin && IRrightDistance == 1 && IRleftDistance == 0) robotLeft();
  else if (tempDistance > objectDistanceMin && IRleftDistance == 1 && IRrightDistance == 0) robotRight();
  else if (tempDistance <= objectDistanceMin || tempDistance >= objectDistanceMax) robotStop();
  else if (IRleftDistance == 1 && IRrightDistance == 1) robotStop();
  else robotStop();
}




void robotForward() {
  motorL.run(FORWARD);
  motorR.run(FORWARD);
}

void robotBackward() {
  motorL.run(BACKWARD);
  motorR.run(BACKWARD);
}

void robotLeft() {
  motorL.run(RELEASE);
  motorR.run(FORWARD);
}

void robotRight() {
  motorL.run(FORWARD);
  motorR.run(RELEASE);
}

void robotSharpLeft() {
  motorL.run(BACKWARD);
  motorR.run(FORWARD);
}

void robotSharpRight() {
  motorL.run(FORWARD);
  motorR.run(BACKWARD);
}

void robotStop() {
  motorL.run(RELEASE);
  motorR.run(RELEASE);
}




int findDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = (duration * .0343) / 2;
  delay(25);
  return distance;
}
