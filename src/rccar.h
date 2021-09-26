#include "Arduino.h"
#include <ArduinoJSON.h>
#include <AsyncWebSocket.h>

#pragma once
#ifndef RCCAR_H_
#define RCCAR_H_


const char *ssid = "ESP32-RC_CAR-AP";
const char *password = "TEJ3M12021";
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;
const int led_pin = 15;


//void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length);
void onIndexRequest(AsyncWebServerRequest *request);
void onCSSRequest(AsyncWebServerRequest *request);
void onJSRequest(AsyncWebServerRequest *request);
void arcadeDrive(double xSpeed, double zRotation, boolean squareInputs);
#endif 