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

#define touch1 1
#define touch2 2
#define touch3 3
#define touch4 4
#define touch5 5

#define threshold 80000

#define led1_R 12
#define led1_G 13
#define led1_B 14

#define led2_R 15
#define led2_G 17
#define led2_B 16

#define led1 19
#define led2 20
#define led3 21
#define led4 33
#define led5 34
#define led6 35

#define wifi_on_off_pin 18

#define GPIO_1 10
#define GPIO_2 11

// Set these to your desired credentials.
const char* ssid = "WrightStuff";
const char* password = "1SmallStep";

boolean wifi_isOn = true;
boolean wifi_stateChanged = false;
boolean wifi_switch_prev_state = LOW;



volatile boolean led_isRed = false;
volatile short sat_color = 1;

IPAddress Ip(7, 20, 19, 69);
IPAddress NMask(255, 255, 255, 0);

WiFiServer server(80);

hw_timer_t *Timer0_Cfg = NULL;

void disableWiFi();
void enableWiFi();
void turnOnLEDs();
void turnOffLEDs();
void led2_Red();
void led2_Green();
void led2_Blue();
void led2_Yellow();
void twinkle_stars();


void IRAM_ATTR Timer0_ISR() {
  // maybe use for PWM all the star LEDs
  twinkle_stars();
}



void process_touch5(){
  sat_color = 1;
}

void process_touch4(){
  sat_color = 2;
}

void process_touch3(){
  sat_color = 3;
}

void process_touch2(){
  sat_color = 4;
}



void setup() {
  pinMode(led1_R, OUTPUT);
  pinMode(led1_G, OUTPUT);
  pinMode(led1_B, OUTPUT);

  pinMode(led2_R, OUTPUT);
  pinMode(led2_G, OUTPUT);
  pinMode(led2_B, OUTPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);
  
  
  //pinMode(wifi_on_off_pin, INPUT);
  //digitalWrite(wifi_on_off_pin, LOW);

  pinMode(GPIO_1, OUTPUT);
  digitalWrite(GPIO_1, HIGH);
  pinMode(GPIO_2, OUTPUT);
  digitalWrite(GPIO_2, HIGH);

  Timer0_Cfg = timerBegin(0, 240, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 100000, true);
  timerAlarmEnable(Timer0_Cfg);

  Serial.begin(115200);
  
  enableWiFi();
  

  Wire.begin(8, 9);   // More to follow on if/what we decide to do with I2C

  turnOnLEDs();
  led2_Blue();
  twinkle_stars();

  touchAttachInterrupt(touch5, process_touch5, threshold);
  touchAttachInterrupt(touch4, process_touch4, threshold);
  touchAttachInterrupt(touch3, process_touch3, threshold);
  touchAttachInterrupt(touch2, process_touch2, threshold);
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
            client.print("<title>The Wright Stuff</title>");
            client.print("</head>");
            client.print("<body bgcolor=\"black\">");
            client.print("<h1>Aerospace Village</h1>");
            client.print("<ul><li>Build</li><li>Inspire</li><li>Promote</li></ul>");
            client.print("<p>Welcome to the Aerospace Village Badge for 2023. This badge has a number of Easter eggs, one of which you've found if you're reading this now.</p>");
            client.print("<p>Turn <a href=\"/H\" class=\"button\">ON</a> the LED.</p><br>");
            client.print("<p>Turn <a href=\"/L\" class=\"button\">OFF</a> the LED.</p><br>");
            client.print("<p>Turn <a href=\"/M\" class=\"button\">ON</a> the propellers.</p><br>");
            client.print("<p>Turn <a href=\"/m\" class=\"button\">OFF</a> the propellers.</p><br>");

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
          turnOnLEDs();                // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          turnOffLEDs();                // GET /L turns the LED off
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

  //check_wifi_state();
  if(wifi_isOn == true){
    //turnOnLEDs();
  }else{
    //turnOffLEDs();
  }

  
  /*
  Serial.println("\n\n");

  int touchValue = touchRead(touch2);
  Serial.print("Touch 2: ");
  Serial.println(touchValue);

  touchValue = touchRead(touch3);
  Serial.print("Touch 3: ");
  Serial.println(touchValue);

  touchValue = touchRead(touch4);
  Serial.print("Touch 4: ");
  Serial.println(touchValue);
  */
  
  //int touchValue = touchRead(touch5);
  //Serial.print("Touch 5: ");
  //Serial.println(touchValue);


  //if(touchRead(touch5) > 150000 || touchRead(touch4) > 150000 || touchRead(touch3) > 150000){
  //  led_isRed = !led_isRed;
  //}

  if(sat_color == 1){
    led2_Red();
  }else if(sat_color == 2){
    led2_Green();
  }else if(sat_color == 3){
    led2_Blue();
  }else{
    led2_Yellow();
  }

  //twinkle_stars();
  //delay(150);

  //turnOffLEDs();

  //touchValue = touchRead(touch3);
  //Serial.println(touchValue);
  //delay(250);
  
  
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
  digitalWrite(led1_R, HIGH);
  digitalWrite(led1_G, HIGH);
  digitalWrite(led1_B, HIGH);

  digitalWrite(led2_R, HIGH);
  digitalWrite(led2_G, HIGH);
  digitalWrite(led2_B, HIGH);

  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led4, HIGH);
  digitalWrite(led5, HIGH);
  digitalWrite(led6, HIGH);
}

void turnOffLEDs(){
  digitalWrite(led1_R, LOW);
  digitalWrite(led1_G, LOW);
  digitalWrite(led1_B, LOW);

  digitalWrite(led2_R, HIGH);
  digitalWrite(led2_G, HIGH);
  digitalWrite(led2_B, HIGH);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
  digitalWrite(led6, LOW);  
}


void led2_Red(){
  digitalWrite(led2_R, HIGH);
  digitalWrite(led2_G, LOW);
  digitalWrite(led2_B, LOW);
}

void led2_Green(){
  digitalWrite(led2_R, LOW);
  digitalWrite(led2_G, HIGH);
  digitalWrite(led2_B, LOW);
}

void led2_Blue(){
  digitalWrite(led2_R, LOW);
  digitalWrite(led2_G, LOW);
  digitalWrite(led2_B, HIGH);
}

void led2_Yellow(){
  digitalWrite(led2_R, HIGH);
  digitalWrite(led2_G, LOW);
  digitalWrite(led2_B, HIGH);
}

void twinkle_stars(){

  short min_val = 20;
  short max_val = 60;
  
  analogWrite(led1, random(min_val,max_val));
  analogWrite(led2, random(min_val,max_val));
  analogWrite(led3, random(min_val,max_val));
  analogWrite(led4, random(min_val,max_val));
  analogWrite(led5, random(min_val,max_val));
  analogWrite(led6, random(min_val,max_val));
}