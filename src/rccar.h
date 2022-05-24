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

const int leftMotorMainPinOne = 27; 
const int leftMotorMainPinTwo = 16;
const int leftMotorMainEnablePin = 14;

const int leftMotorFollowerPinOne = 17; 
const int leftMotorFollowerPinTwo = 25;
const int leftMotorFollowerEnablePin = 26;


const int rightMotorMainPinOne = 4; 
const int rightMotorMainPinTwo = 35; 
const int rightMotorMainEnablePin = 2;


const int rightMotorFollowerPinOne = 5; 
const int rightMotorFollowerPinTwo = 13; 
const int rightMotorFollowerEnablePin = 12;

// Setting PWM properties
const int freq = 30000;
const int pwmChannelLeft = 0;
const int pwmChannelLeftFollower = 1;

const int pwmChannelRight =2;
const int pwmChannelRightFollower =3;
const int resolution = 8;

//void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length);
void onIndexRequest(AsyncWebServerRequest *request);
void onCSSRequest(AsyncWebServerRequest *request);
void onJSRequest(AsyncWebServerRequest *request);
void arcadeDrive(double xSpeed, double zRotation);
void leftMotor(int motorSpeed);
void rightMotor(int motorSpeed);
#endif 