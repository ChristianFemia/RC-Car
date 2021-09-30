#include "Arduino.h"
#include <AsyncWebSocket.h>

#pragma once
#ifndef RCCAR_H_
#define RCCAR_H_


const char *ssid = "ESP32-RC_CAR-AP";
const char *password = "TEJ3M12021";
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;

const int leftMotorPinOne = 27; 
const int leftMotorPinTwo = 16;
const int leftMotorEnablePin = 14;
const int rightMotorPinOne = 17; 
const int rightMotorPinTwo = 25; 
const int rightMotorEnablePin = 26;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int pwmChannelRight =1;
const int resolution = 8;

//void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length);
void onIndexRequest(AsyncWebServerRequest *request);
void onCSSRequest(AsyncWebServerRequest *request);
void onJSRequest(AsyncWebServerRequest *request);
void arcadeDrive(double xSpeed, double zRotation);
void leftMotor(int motorSpeed);
void rightMotor(int motorSpeed);
#endif 