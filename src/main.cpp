#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <rccar.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <math.h>

//Globals
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
char msg_buf[10];
char* in;
char* left;
char* right;
int leftValue;
int rightValue;
String leftValueString;
String rightValueString;

void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t *payload,
                      size_t length)
{
  // Figure out the type of WebSocket event
  switch (type)
  {

  // Client has disconnected
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", client_num);
    break;

  // New client has connected
  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(client_num);
    Serial.printf("[%u] Connection from ", client_num);
    Serial.println(ip.toString());
  }
  break;

  // Handle text messages from client
  case WStype_TEXT:

    // Print out raw message
   //Serial.printf("[%u] Received text: %s\n", client_num, payload);
      in = (char*) payload;
      left = strtok(in, ":");
      right = strtok(NULL, ":");
      leftValueString = (String)left;
      leftValue = leftValueString.toInt();
      rightValueString = (String)right;
      rightValue = rightValueString.toInt();
      arcadeDrive(leftValue, rightValue);
  

    break;

  // For everything else: do nothing
  case WStype_BIN:
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  default:
    break;
  }
}

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.css", "text/css");
}

// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}

void rightMotor(int motorSpeed)                       
{
  if (motorSpeed > 0)                                 
  {
    digitalWrite(rightMotorPinOne, HIGH);                         
    digitalWrite(rightMotorPinTwo, LOW);                          
  }
  else if (motorSpeed < 0)                            
  {
    digitalWrite(rightMotorPinOne, LOW);                          
    digitalWrite(rightMotorPinTwo, HIGH);                         
  }
  else                                                
  {
    digitalWrite(rightMotorPinOne, LOW);                          
    digitalWrite(rightMotorPinTwo, LOW);                          
  }
 ledcWrite(pwmChannelRight, motorSpeed);           
}

void leftMotor(int motorSpeed)                       
{
  if (motorSpeed > 0)                                 
  {
  //  Serial.println("Move Foward");
    digitalWrite(leftMotorPinOne, HIGH);                         
    digitalWrite(leftMotorPinTwo, LOW);                          
  }
  else if (motorSpeed < 0)                            
  {
    //Serial.println("Move Backwards");
    digitalWrite(leftMotorPinOne, LOW);                          
    digitalWrite(leftMotorPinTwo, HIGH);                         
  }
  else                                                
  {
    digitalWrite(leftMotorPinOne, LOW);                          
    digitalWrite(leftMotorPinTwo, LOW);                          
  }

  ledcWrite(pwmChannel, motorSpeed);                
}


void arcadeDrive(double xSpeed, double zRotation)
{

  //Define two variables for left and right speed
  double leftSpeed;
  double rightSpeed;
  //Define a max input for the motors
  double maxInput = copysign(max(abs(xSpeed), abs(zRotation)), xSpeed);

  /**
        * 
        * This set of code defines how the robot will turn
        * It applies power to each motor indivdually, 
        * and does some math to determine how much power it will need to turn
        */

  if (xSpeed >= 0.0)
  {
    // First quadrant, else second quadrant
    if (zRotation >= 0.0)
    {
      leftSpeed = maxInput;
      rightSpeed = xSpeed - zRotation;
    }
    else
    {
      leftSpeed = xSpeed + zRotation;
      rightSpeed = maxInput;
    }
  }
  else
  {
    // Third quadrant, else fourth quadrant
    if (zRotation >= 0.0)
    {
      leftSpeed = xSpeed + zRotation;
      rightSpeed = maxInput;
    }
    else
    {
      leftSpeed = maxInput;
      rightSpeed = xSpeed - zRotation;
    }
  }

  if(leftSpeed > 255){
    leftSpeed = 255;
  } else if (leftSpeed < -255 ){
    leftSpeed = -255;
  }
  if(rightSpeed > 255){
    rightSpeed = 255;
  } else if (rightSpeed < -255){
    rightSpeed = -255;
  }
  //Serial.println(leftSpeed);
  leftMotor(leftSpeed);
  rightMotor(rightSpeed);
}

void setup()
{
 // Start Serial port
  Serial.begin(115200);

  // put your setup code here, to run once:
Serial.println("PinMode Setup");
  pinMode(leftMotorPinOne, OUTPUT);
  pinMode(leftMotorPinTwo, OUTPUT);
  pinMode(leftMotorEnablePin, OUTPUT);
  
  pinMode(rightMotorPinOne, OUTPUT);
  pinMode(rightMotorPinTwo, OUTPUT);
  pinMode(rightMotorEnablePin, OUTPUT);

  ledcSetup(pwmChannel, freq, resolution);
  ledcSetup(pwmChannelRight, freq, resolution);

  ledcAttachPin(leftMotorEnablePin, pwmChannel);
  ledcAttachPin(rightMotorEnablePin, pwmChannelRight);

 

  // Make sure we can read the file system
  if (!SPIFFS.begin())
  {
    Serial.println("Error mounting SPIFFS");
    while (1)
      ;
  }


  // Start access point
  WiFi.softAP(ssid, password);

  // Print our IP address
  Serial.println();
  Serial.println("AP running");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // On HTTP request for style sheet, provide style.css
  server.on("/style.css", HTTP_GET, onCSSRequest);

  server.on("/virtualjoystick.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/virtualjoystick.js", "text/javascript"); });

  // Handle requests for pages that do not exist
  server.onNotFound(onPageNotFound);

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop()
{
  yield();
  webSocket.loop();

}