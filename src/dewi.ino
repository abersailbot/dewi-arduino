#include "compass.h"
#include <Servo.h>
#include <Wire.h>
#define DEBUG 1
Servo rudder;
Servo sail;

int windSensorPin = A2;

int offset; //this is for the wind sensor
char current_line[6]; // allocate some space for the string
void setup() {
  Serial.begin(9600);

  pinMode(windSensorPin, INPUT);

  rudder.attach(10, 1060, 1920);
  sail.attach(9, 1100, 1930);
  // set default positions
  rudder.writeMicroseconds(1500);
  sail.writeMicroseconds(1500);
  // initialize the I2C bus
  Wire.begin();
  // tell the client we've started
  Serial.println("{\"started\": true}");
}
void log_json_int(char* key, int value) {
  // write a single json key value pair
  if (DEBUG) {
    Serial.print("{\"");
    Serial.print(key);
    Serial.print("\": ");
    Serial.print(value);
    Serial.println("}");
  }
}
void log_json_float(char* key, float value) {
  // write a single json key value pair
  if (DEBUG) {
    Serial.print("{\"");
    Serial.print(key);
    Serial.print("\": ");
    Serial.print(value);
    Serial.println("}");
  }
}
void read_line(char* line) {
  // read characters from serial into line until a newline character
  char c;
  int index;
  for (index = 0; index < 5; index++) {
    // wait until there is a character
    while (Serial.available() == 0);
    // read a character
    c = Serial.read();
    if (c == '\n' || c== '\r') {
    	line[index]='\0';
      break;
    }
    else {
      line[index] = c;
    }
  }
  // terminate the string
  line[index+1] = '\0';
}
void set_rudder(int amount) {
  // set the rudder to amount
  amount = constrain(amount, 1060, 1920);
  rudder.writeMicroseconds(amount);
  log_json_int("rudder", amount);
}
void set_sail(int amount) {
  // set the sail to amount
  amount = constrain(amount, 1100, 1930);
  sail.writeMicroseconds(amount);
  log_json_int("sail", amount);
}
int get_amount(char* line) {
  // return the number in a string such as "r1200" as an int
  int amount;
  amount = (int) strtol(line + 1, NULL, 10);
  return amount;
}

float read_compass() {
  // read from the compass and output it
  if (Compass.poll_data()) { // Send error if false(?)
    log_json_float("compass", Compass.get_bearing());
    
  } else {
    
  }
}

int mod(int angle)
{
  if (angle > 359) {
    return mod(angle - 359);
  }
  else if (angle < 0) {
    return mod(359 - angle);
  }
  else {
    return angle;
  }
}


int readWindSensor() {
  int pulseLength = 0;
  int windAngle = 0;
  pulseLength = pulseIn(windSensorPin, HIGH, 2000);
  int magic = 29;
  windAngle = ((pulseLength * 10) / magic); // 29 is the magic number where pulse time of 1036 = 359
  windAngle = windAngle - offset;//Compensate for offset
  windAngle = mod(windAngle); // Wrap Arround
  log_json_int("wind", windAngle);
}

void set_offset(int amount) {
  offset = amount;

}



void loop() {
  if(DEBUG == 1){
    Serial.println("Running Debug in loop");
  }  
  read_line(current_line);
  if(DEBUG == 1){
    Serial.println(current_line);
  }
  switch (current_line[0]) {
    case 'c':
      read_compass();
      break;
    case 'w':
      readWindSensor();
      break;
    case 'r':
      set_rudder(get_amount(current_line));
      break;
    case 's':
      set_sail(get_amount(current_line));
      break;
    case 'o':
      set_offset(get_amount(current_line));
      break;
     
  }
}

