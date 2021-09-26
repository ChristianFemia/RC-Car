#include <Arduino.h>
#include <ArduinoJson.h>
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

void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t *payload,
                      size_t length)
{
  StaticJsonDocument<200> doc;
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
    Serial.printf("[%u] Received text: %s\n", client_num, payload);

    deserializeJson(doc, payload);
    //Serial.print("X: "); 
    Serial.println(doc["x"].as<int>());
    // arcadeDrive(doc["x"], doc["y"], true);
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
void arcadeDrive(double xSpeed, double zRotation, boolean squareInputs)
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
}

void setup()
{
  // put your setup code here, to run once:

  // Start Serial port
  Serial.begin(115200);

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