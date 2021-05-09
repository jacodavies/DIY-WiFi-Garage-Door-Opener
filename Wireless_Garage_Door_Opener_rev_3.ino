// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


// Replace with your network credentials
const char* ssid     = "replace-with-network-name";
const char* password = "replace-with-network-password";

// Set web server port number to 8080
WiFiServer server(8080);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state

String output7State = "Off";
String output6State = "Off";

// Assign output variables to GPIO pins
const int output7 = 0;
const int output6 = 2;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  // Initialize the output variables as outputs
  //********** CHANGE PIN FUNCTION  TO GPIO **********
  //GPIO 1 (TX) swap the pin to a GPIO.
  pinMode(1, FUNCTION_3); 
  //GPIO 3 (RX) swap the pin to a GPIO.
  pinMode(3, FUNCTION_3);
  pinMode(output7, OUTPUT);
  pinMode(output6, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output7, LOW);
  digitalWrite(output6, LOW);

  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  // Start web server
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /7/on") >= 0) {
              output7State = "On";
              digitalWrite(output7, HIGH);
            } else if (header.indexOf("GET /7/off") >= 0) {
              output7State = "Off";
              digitalWrite(output7, LOW);
            } else if (header.indexOf("GET /6/on") >= 0) {
              output6State = "On";
              digitalWrite(output6, HIGH);
            } else if (header.indexOf("GET /6/off") >= 0) {
              output6State = "Off";
              digitalWrite(output6, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { background-color: #ffffff; font-family: Futura; display: inline-block; margin: 10px auto; text-align: center;}");
            client.println(".button { background-color: #ff2525; border: none; color: #ffff99; padding: 7px 60px;");
            client.println("text-decoration: none; font-size: 50px; margin: 0px; cursor: wait;}");
            client.println(".button2 {background-color: #25ff25;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Garage Door Opener</h1>");
            
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { background-color: #ffffff; font-family: Futura; display: inline-block; margin: 10px auto; text-align: center;}");
            client.println(".button5 { background-color: #33dec5; border: none; color: #ffff99; padding: 7px 60px;");
            client.println("text-decoration: none; font-size: 50px; margin: 0px; cursor: wait;}");
            client.println(".button6 {background-color: #eeeeee;}</style></head>");
            
            // Display current state, and ON/OFF buttons for GPIO 1  
           // client.println("<p>The Garage Light is " + output6State + "</p>");
            // If the output6State is off, it displays the ON button       
            if (output6State=="Off") {
              client.println("<p><a href=\"/6/on\"><button class=\"button5\">Door</button></a></p>");
            } else {
              
              client.print("<HEAD>");
              client.print("<meta http-equiv=\"refresh\" content=\"0;url=/6/off\">");
              client.print("</head>");
              
              delay(50);
            }
            
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { background-color: #ffffff; font-family: Futura; display: inline-block; margin: 10px auto; text-align: center;}");
            client.println(".button7 { background-color: #2fd2eb; border: none; color: #ffff99; padding: 7px 60px;");
            client.println("text-decoration: none; font-size: 50px; margin: 0px; cursor: wait;}");
            client.println(".button8 {background-color: #eeeeee;}</style></head>");
            
            // Display current state, and ON/OFF buttons for GPIO 3  
           // client.println("<p>The Garage Door is " + output7State + "</p>");
            // If the output7State is off, it displays the ON button       
          /*  if (output7State=="Off") {
              client.println("<p><a href=\"/7/on\"><button class=\"button7\">Light</button></a></p>");
            } else {
              
              client.print("<HEAD>");
              client.print("<meta http-equiv=\"refresh\" content=\"0;url=/7/off\">");
              client.print("</head>");
              
              delay(50);
            }  */
               
            client.println("<img src=\"https://media.istockphoto.com/vectors/wifi-icon-vector-id504816042?k=6&m=504816042&s=612x612&w=0&h=eO80eKD0CkoHpVHMx1GEJTL1rBMb1jno_X0UewddvO8=\" alt=\"No Internet\" height=\"17\" width=\"23\">");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
}
