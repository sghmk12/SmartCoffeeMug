// Made by Sathira Katugaha in June of 2018 as a fun project!

#include <OneWire.h>
#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial = SoftwareSerial(255,A5);
Servo myServo;
OneWire ds(10);
boolean running = false;
boolean closed = false;