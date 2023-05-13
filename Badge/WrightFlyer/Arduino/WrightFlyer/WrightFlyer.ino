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
#include "Wire.h"

#define led_pin_R 4
#define led_pin_G 5
#define led_pin_B 6

#define motor_pin 10
#define motor_pin_backup 11

#define wifi_on_off_pin 12

#define GPIO_1 13
#define GPIO_2 14

// Motor on/off variables
#define motor_on_duration 5000      // Was 2500
#define motor_off_duration 70000

// Set these to your desired credentials.
const char* ssid = "WrightFlyer";
const char* password = "bicycles";

boolean wifi_isOn = false;
boolean wifi_stateChanged = false;
boolean wifi_switch_prev_state = LOW;
boolean led_isOn = true;

IPAddress Ip(12, 12, 15, 59);
IPAddress NMask(255, 255, 255, 0);

WiFiServer server(80);

hw_timer_t *Timer0_Cfg = NULL;
volatile boolean motorOn_interrupt = false;
boolean motorOn_user = true;


void IRAM_ATTR Timer0_ISR() {
  if(motorOn_user == true){
    digitalWrite(motor_pin, motorOn_interrupt);
  }

  // Always write the motor_pin_backup value
  digitalWrite(motor_pin_backup, motorOn_interrupt);

  
    
  if(motorOn_interrupt == true){    
      timerAlarmWrite(Timer0_Cfg, motor_on_duration, true);    
  }else{
      timerAlarmWrite(Timer0_Cfg, motor_off_duration, true);
  }
  motorOn_interrupt = !motorOn_interrupt;    
  
  //Serial.println("Timer interrupt triggered");
}

void disableWiFi();
void enableWiFi();
void turnOnLEDs();
void turnOffLEDs();



void setup() {
  pinMode(led_pin_R, OUTPUT);
  pinMode(led_pin_G, OUTPUT);
  pinMode(led_pin_B, OUTPUT);

  
  pinMode(motor_pin, OUTPUT);
  pinMode(motor_pin_backup, OUTPUT);
  
  pinMode(wifi_on_off_pin, INPUT);
  digitalWrite(wifi_on_off_pin, LOW);

  pinMode(GPIO_1, OUTPUT);
  digitalWrite(GPIO_1, HIGH);
  pinMode(GPIO_2, OUTPUT);
  digitalWrite(GPIO_2, HIGH);

  Timer0_Cfg = timerBegin(0, 240, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 400000, true);
  timerAlarmEnable(Timer0_Cfg);

  Serial.begin(115200);
  
  enableWiFi();
  turnOnLEDs();

  Wire.begin(8, 9);   // More to follow on if/what we decide to do with I2C
   
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
            client.print("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.print("<!-- What is the significance of the ipv4 address of this webpage? Think you know... then talk to an Aerospace Village Staff Volunteer -->");
            client.print("<!-- V2FudCB0byBrbm93IG1vcmUgYWJvdXQgTk9UQU1TIGFuZCB0aGUgd2Vic2l0ZSBvdXRhZ2UgdGhhdCBtYWRlIHRoZSBuZXdzIHJlY2VudGx5PwoKTGVhcm4gbW9yZSBhYm91dCB0aGUgb3V0YWdlIGhlcmU6Cmh0dHBzOi8vd3d3LmZhYS5nb3YvbmV3c3Jvb20vZmFhLW5vdGFtLXN0YXRlbWVudAoKVGhlIE5PVEFNUyB3ZWJzaXRlIGl0c2VsZiBpcyBoZXJlOgpodHRwczovL3d3dy5ub3RhbXMuZmFhLmdvdi9kaW5zUXVlcnlXZWIv -->");
            client.print("<style>.button { background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; }");
            client.print("h1 { color: white; } p { color: white; } a { color: white; } ul { color:white;}");
            client.print("</style>");
            client.print("<title>Aerospace Village Badge 2023</title>");
            client.print("</head>");
            client.print("<body bgcolor=\"black\">");
            client.print("<h1>Aerospace Village</h1>");
            client.print("<ul><li>Build</li><li>Inspire</li><li>Promote</li></ul>");
            client.print("<p>Welcome to the Aerospace Village Badge for 2023. This badge has a number of Easter eggs, one of which you've found if you're reading this now.</p>");
            client.print("<p>Turn <a href=\"/H\" class=\"button\">ON</a> the LED.</p><br>");
            client.print("<p>Turn <a href=\"/L\" class=\"button\">OFF</a> the LED.</p><br>");
            client.print("<p><a href=\"/M\" class=\"button\">TOGGLE</a> the propellers.</p><br>");

            client.print("<p>Learn more about the <a href=\"https://aerospacevillage.org\">Aerospace Village</a></p>");            
            client.print("</body></html>");

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
          led_isOn = true;
          turnOnLEDs();                // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          led_isOn = false;
          turnOffLEDs();                // GET /L turns the LED off
        }

        if (currentLine.endsWith("GET /M")) {
          motorOn_user = !motorOn_user;                
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

void check_wifi_state(){
  int val = digitalRead(wifi_on_off_pin);
  
  if(val != wifi_switch_prev_state){
    Serial.println("WiFi pin state change");
    if(digitalRead(wifi_on_off_pin) == HIGH){
      //Turn On the Wifi
      enableWiFi();
    }else{
      //Turn Off the Wifi
      disableWiFi();
    }
    wifi_switch_prev_state = val;
  }
}

void loop() {

  process_client();

  check_wifi_state();
  
}


void disableWiFi(){
    wifi_isOn = false;
    Serial.println("Turning the Wifi OFF");
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
    Serial.println("WiFi is turned OFF");
}

void enableWiFi(){
    wifi_isOn = true;
    Serial.println("Configuring access point...");
    Serial.print("SSID: ");
    Serial.print(ssid);
    Serial.print(" Password: ");
    Serial.print(password);
    Serial.print("\n");
  
    // You can remove the password parameter if you want the AP to be open.
    //WiFi.mode(WIFI_AP);
    //Serial.println("Wifi Mode set");

    Serial.println("START WIFI");
    
    WiFi.softAP(ssid, password);
    Serial.println("SoftAP set");
  
    WiFi.softAPConfig(Ip, Ip, NMask);
    Serial.println("AP config set");
    
    server.begin();
    Serial.println("Server started");
}

void turnOnLEDs(){
  digitalWrite(led_pin_R, HIGH);
  digitalWrite(led_pin_G, HIGH);
  digitalWrite(led_pin_B, HIGH);
  
}

void turnOffLEDs(){
  digitalWrite(led_pin_R, LOW);
  digitalWrite(led_pin_G, LOW);
  digitalWrite(led_pin_B, LOW);

}
