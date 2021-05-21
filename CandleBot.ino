#include <Ultrasonic.h>

Ultrasonic ultrasonicR(9,8);

boolean infraredL, infraredR, infraredF;
int intensityF, intensityB;

int distanceR;
int diff = 0;

int threshold = 960; //intensity at which the light sensor triggers
int ambientLight = 600;

int candle = 0; //number of candles put out

int AA = 6;   //pins 5,6,10, 11 are the only pins that can output 
int AB = 5;   //an 'analog' signal, meaning that you can adjust the
int BA = 11;  //strength of the the signal (google 'post-width modulation')
int BB = 10;

int biasM = 20; //for right motor
int spd = 180;  //adjust the speed of the motors
                //you can input values 0-255, but I found that
                //128 is the lowest you can go to make the motors move
                
int hitWall = 0; //number of times the robot hits the wall

int minLight = 300;
int maxLight = 920;

int frontTrigger = 2;    //min distance the sensors trigger

unsigned long previousRightTurn = 0;
unsigned long previousLeftTurn = 0;
unsigned long previousReverse = 0;

void setup() 
{
  pinMode(A4, OUTPUT);
  
  pinMode(AA, OUTPUT);
  pinMode(AB, OUTPUT);
  pinMode(BA, OUTPUT);
  pinMode(BB, OUTPUT);

  digitalWrite(A0, HIGH);
  digitalWrite(A5, HIGH);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
}

void loop() 
{
//  intensityF = 1023 - analogRead(A0);
//  intensityB = 1023 - analogRead(A5);
//  Serial.print("F: ");
//  Serial.print(intensityF);
//  Serial.print(" DIFF:");
//  Serial.print(abs(intensityF - intensityB));
//  Serial.print(" B: ");
//  Serial.println(intensityB);
//
//  if (hitWall == 0) { //drive out
//    driveOut();
//  } else if (hitWall < 1) { //go to corner
//    avoidLine();
//    avoidBuildings();
//    avoidLight();
//  } else { //traverse
//    avoidLine();
//    avoidBuildings();
//    followLight();
//    if (candle != 7) {
//      extinguish();
//    } else {
//      turnLeftThenExtinguish();
//    }
//  }

testLight();

}

void followLight() {
  intensityF = 1023 - analogRead(A0);
  intensityB = 1023 - analogRead(A5);

  int difference = abs(intensityF - intensityB);

  if (intensityF > 600 && intensityF > intensityB  && difference > 10) {
    forward();
  }
  else if (intensityB > 600 && intensityB > intensityF  && difference > 10) {
    reverse();
  }
  else if (intensityB > 600 && intensityF > 600 && difference < 10){
    turnOnFan();
  }
}

void turnOnFan() {
  halt();
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(A4, HIGH);
  delay(5000);
  intensityF = 1023 - analogRead(A0);
  intensityB = 1023 - analogRead(A5);
  while (intensityF > 600 || intensityB > 600) //keep fan on until light is put out
  {
    delay(5000);
    intensityF = 1023 - analogRead(A0);
    intensityB = 1023 - analogRead(A5);
  }
  digitalWrite(A4, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  veerRight();
  wait(400);
  candle++;
}

void turnLeftThenExtinguish() {
  
  intensityF = 1023 - analogRead(A0);
  intensityB = 1023 - analogRead(A5);

  if (intensityB > threshold) {
    veerLeft();
    wait(100);
    intensityF = 1023 - analogRead(A0);
    while (intensityF < threshold) {
      wait(100);
      intensityF = 1023 - analogRead(A0);
    }
    forward();
    wait(300);
    turnOnFan();
    forward();
  }
}

void extinguish() {

  intensityF = 1023 - analogRead(A0);
  intensityB = 1023 - analogRead(A5);

  int difference = abs(intensityF - intensityB);

  if (intensityB > minLight && intensityF > minLight && difference < 200){
    turnOnFan();
  }
  
//  if (intensityF > 600 && intensityF > intensityB  && difference > 10) {
//    forward();
//  }
//  else if (intensityB > threshold - 300 && intensityB > intensityF  && difference > 10) {
//    reverse();
//  }
//  else if (intensityB > threshold - 300 && intensityF > 600 && difference < 10){
//    turnOnFan();
//  }
}

void avoidLight() {
   intensityF = 1023 - analogRead(A0);
   if (intensityF > maxLight) {
    veerRight();
    wait(100);
   }

void avoidBuildings() {
  distanceR = ultrasonicR.distanceRead(CM);
  if (distanceR <= frontTrigger && distanceR != 0)
  {
    turnLeft();
    wait(100);
  } else {
    forward();
  }
}

void driveOut() 
{
  infraredL = (digitalRead(3) == HIGH);
  infraredR = (digitalRead(2) == HIGH);
  
  if (infraredL || infraredR) {
    reverse();
    wait(600);
    turnRight();
    wait(1100);
    hitWall++;
  } else {
    forward();
  }
}

void avoidLine()
{
  
  infraredL = (digitalRead(3) == HIGH);
  infraredR = (digitalRead(2) == HIGH);
  infraredF = (digitalRead(7) == HIGH);

  if (infraredF) {
    previousReverse = millis();
    diff = min(labs(previousRightTurn - previousReverse), labs(previousLeftTurn - previousReverse));

    if (diff < 2000 && diff > 0) {
      reverse();
      wait(500);
      veerRightReverse();
      wait(900);
    } else {
      veerRightReverse();
      wait(1400);
    }
    previousRightTurn = 0;
    previousLeftTurn = 0;
    previousReverse = 0;
    hitWall++;
  }
  else if (infraredL && !infraredR && !infraredF)
  {
    previousRightTurn = millis();
    veerRight();
  }
  else if (infraredR && !infraredL && !infraredF)
  {
    previousLeftTurn = millis();
    veerLeft();
  }
  else
  {
    forward();
  }

  // in the case that the robot is approaching a wall at a small angle,
  // if both sensors trigger within 1 sec of each other, we have hit a wall
//  diff = labs(previousRightTurn - previousLeftTurn);
//  
//  if (diff < 2000 && diff > 0) { 
//    reverse();
//    wait(900);
//    veerRightReverse();
//    wait(1400);
//    previousRightTurn = 0;
//    previousLeftTurn = 0;
//    hitWall++;
//  }
}

void testLight() {

  intensityF = 1023 - analogRead(A0);
  intensityB = 1023 - analogRead(A5);

  int difference = abs(intensityF - intensityB);
  
  Serial.print("F: ");
  Serial.print(intensityF);
  Serial.print(" DIFF: ");
  Serial.print(difference);
  Serial.print(" B: ");
  Serial.println(intensityB);
  
}

void testUltrasonic() 
{
  distanceR = ultrasonicR.distanceRead(CM);
  Serial.println(distanceR);
}

void forward()
{
  analogWrite(AA, spd);
  analogWrite(AB, 0);
  analogWrite(BA, spd);
  analogWrite(BB, 0); 
}

void reverse()  
{
  analogWrite(AA, 0);
  analogWrite(AB, spd);
  analogWrite(BA, 0);
  analogWrite(BB, spd);
}

void turnRight()
{
  analogWrite(AA, 0);
  analogWrite(AB, spd);
  analogWrite(BA, spd);
  analogWrite(BB, 0);
}

void  veerRight()
{
  analogWrite(AA, spd);
  analogWrite(AB, spd);  
  analogWrite(BA, spd);
  analogWrite(BB, 0);
}

void  turnLeft()
{
  analogWrite(AA, spd);
  analogWrite(AB, 0);
  analogWrite(BA, 0);
  analogWrite(BB, spd); 
}

void  veerLeft()
{
  analogWrite(AA, spd + biasM);
  analogWrite(AB, 0);  
  analogWrite(BA, spd + biasM);
  analogWrite(BB, spd + biasM);
}

void veerLeftReverse()
{
  analogWrite(AA, 0);
  analogWrite(AB, spd + biasM);  
  analogWrite(BA, spd + biasM);
  analogWrite(BB, spd + biasM);
}

void veerRightReverse()
{
  analogWrite(AA, spd);
  analogWrite(AB, spd);  
  analogWrite(BA, 0);
  analogWrite(BB, spd);
}

void  halt()
{
  analogWrite(AA, 0);
  analogWrite(AB, 0);
  analogWrite(BA, 0);
  analogWrite(BB, 0);  
}

//boolean lineDetects() {
//  return   (digitalRead(3) == HIGH) && (digitalRead(2) == HIGH);
//}
//
void wait(int del)
{     
    for (int i = 0; i < (del / 100); i++) {
        delay(100);
//        if (lineDetects) {
//          break;
//        }
    }
}


