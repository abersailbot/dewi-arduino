#include <CMPS10.h>
#include <Servo.h>
#include <Wire.h>

#define DEBUG 1

Servo rudder;
Servo sail;
CMPS10 cmps10; // compass

char current_line[6]; // allocate some space for the string

void setup() {
    Serial.begin(9600);

    rudder.attach(10, 1060, 1920);
    sail.attach(9, 1050, 1930);

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
        if (c == '\n') {
            break;
        } else {
            line[index] = c;
        }
    }
    // terminate the string
    line[index] = '\0';
}

void set_rudder(int amount) {
    // set the rudder to amount
    amount = constrain(amount, 1060, 1920);
    rudder.writeMicroseconds(amount);
    log_json_int("rudder", amount);
}

void set_sail(int amount) {
    // set the sail to amount
    amount = constrain(amount, 1050, 1930);
    sail.writeMicroseconds(amount);
    log_json_int("sail", amount);
}

int get_amount(char* line) {
    // return the number in a string such as "r1200" as an int
    int amount;
    amount = (int) strtol(line+1, NULL, 10);
    return amount;
}

float read_compass() {
    // read from the compass and output it
    log_json_float("compass", cmps10.bearing());
}

void loop() {
    read_line(current_line);
    switch (current_line[0]){
        case 'c':
            read_compass();
            break;
        case 'r':
            set_rudder(get_amount(current_line));
            break;
        case 's':
            set_sail(get_amount(current_line));
            break;
    }
}
