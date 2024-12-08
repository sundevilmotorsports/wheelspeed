#include <Arduino.h>
#include <queue>

#define LED 13
#define PIN 0
#define TEETH 49
//TIMEOUT in seconds
#define TIMEOUT 0.01

bool rising = false;
bool falling = false;
bool timerCheck = false;
int fallingTime = 0;
int timer = 0;

int delta = 0;
int epoch = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWriteFast(LED, !digitalReadFast(PIN));
}

void loop() {
  // We are measuring the time between the begining of one tooth (signals rising edge) to the begining of the next tooth (signals falling edge)
    // to do that we get the time when the signal goes from 1 -> 0 (falling edge/beginning of the tooth) and setting falling to true so we know we have started the cycle
    // wait until the signal goes from 0 -> 1 (rising edge/end of the tooth) then setting the rising value to true to signal we are half way through the cycle
    // we take the time when the signal goes from 1 -> 0 (falling edge/end of previous tooth begining of next tooth) 
    // we reset rising to false but not falling since the previous falling edge is both the end of previous tooth and begining of next tooth
    // find the time delta from the start of the cycle to the end and convert that into RPM
    
    // timerCheck is used to estimate when the object has stopped spinning, gets the last time a delta was calculated and measures that against current time
    // if the delta exceeds TIMEOUT then print the object is no longer rotating
  
  volatile bool currSignal = digitalReadFast(PIN);
  digitalWriteFast(LED, !currSignal);

  if(!falling && !rising && !currSignal){
    falling = true;
    fallingTime = micros();
  } else if (falling && !rising && currSignal){
    rising = true;
  } else if (falling && rising && !currSignal){
    delta = micros() - fallingTime;
    fallingTime = micros();
    rising = false;
    Serial.println("rpm: " + String(1 / (TEETH * (delta/(6 * pow10(7))))));
    timer = fallingTime;
    timerCheck = false;
  }

  if(micros() - timer > TIMEOUT * pow10(6) && !timerCheck){
    Serial.println("rpm: 0.0");
    timerCheck = true;
  }
}
