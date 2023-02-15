/*
  Aerospace Village Def Con Badge for 2023
  Author: Dan Allen

  Idea: Celebrate the 120 year anniversary of first flight 12/7/1903 while at Def Con in 2023 (albeit a few months early).

  Design:
     - Spin 2 motors with attached laser cut propellers
     - backlight some aspects of the board with LEDs
     - Have a WiFi connection that people can login into the aircraft and make changes into EEPROM
        - Have a manual override switch that can disable the WiFi portion, use as a teaching aid to aircraft design
        - Have a switch that can turn on/off the wifi server - save battery life and improve securty posture

  
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define led_pin 9
#define motor_pin 10

// Set these to your desired credentials.
const char *ssid = "WrightFlyer";
const char *password = "bicycles";

IPAddress Ip(12, 12, 15, 59);
IPAddress NMask(255, 255, 255, 0);


WiFiServer server(80);

hw_timer_t *Timer0_Cfg = NULL;
volatile boolean motorOn_interrupt = false;
boolean motorOn_user = true;


void IRAM_ATTR Timer0_ISR() {
  if(motorOn_user == true){
    digitalWrite(motor_pin, motorOn_interrupt);
    if(motorOn_interrupt == true){    
      timerAlarmWrite(Timer0_Cfg, 10000, true);    
    }else{
      timerAlarmWrite(Timer0_Cfg, 70000, true);
    }
    motorOn_interrupt = !motorOn_interrupt;    
  }
  Serial.println("Timer interrupt triggered");
}

void setup() {
  pinMode(led_pin, OUTPUT);
  pinMode(motor_pin, OUTPUT);

  Timer0_Cfg = timerBegin(0, 240, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 400000, true);
  timerAlarmEnable(Timer0_Cfg);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(Ip, Ip, NMask);
  server.begin();

  Serial.println("Server started");
}

void process_client() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
            client.print("Click <a href=\"/M\">here</a> to turn ON the propellers.<br>");
            client.print("Click <a href=\"/m\">here</a> to turn OFF the propellers.<br>");
            client.print("</html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(led_pin, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(led_pin, LOW);                // GET /L turns the LED off
        }

        if (currentLine.endsWith("GET /M")) {
          motorOn_user = true;                
        }

        if (currentLine.endsWith("GET /m")) {
          motorOn_user = false;
          digitalWrite(motor_pin, motorOn_user);           
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void loop() {

  process_client();
  
}
