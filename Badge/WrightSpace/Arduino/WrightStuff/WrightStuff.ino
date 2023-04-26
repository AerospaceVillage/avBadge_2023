/*
  Aerospace Village Def Con Badge for 2023
  Author: Dan Allen
  Title: The Wright Stuff

  Idea: Enjoy the nostalgia of 120 years of avaition with a heavy space theme

  Design:
     - Artowrk by flysurreal.com
     - Twinkle some starts and other LEDs
     - Have a WiFi connection that people can login into the spacefract and make changes into EEPROM
        - Have a manual override switch that can enable/disable the WiFi for battery saving
     - Utilize the touch capacitance of the ESP32-S2 MCU fro some user based interactions
     - Be very verbose/spammy on the Serial line for user intaction options.
     - Perhaps use I2C in a creative way?  
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "Wire.h"
#include "index_html.h"

#define touch1 1
#define touch2 2
#define touch3 3
#define touch4 4
#define touch5 5

#define threshold_1 60000
#define threshold_2 60000
#define threshold_3 60000
#define threshold_4 20000
#define threshold_5 20000


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
#define led7 36

#define wifi_on_off_pin 18

#define GPIO_1 10
#define GPIO_2 11

// The WiFi Credentials
const char* ssid = "WrightStuff";
const char* password = "1SmallStep";

// The webserver specifications, ipv4
IPAddress Ip(7, 20, 19, 69);  // Date of first moon landing
IPAddress NMask(255, 255, 255, 0);
WiFiServer server(80);


boolean wifi_isOn = true;
boolean wifi_stateChanged = false;
boolean wifi_switch_prev_state = LOW;



volatile boolean led_isRed = false;
volatile short sat_color = 1;


hw_timer_t *Timer0_Cfg = NULL;

// Function definitions
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


void process_touch1(){
  sat_color = 1;
}

void process_touch2(){
  sat_color = 4;
}

void process_touch3(){
  sat_color = 3;
}

void process_touch4(){
  sat_color = 2;
}

void process_touch5(){
  sat_color = 1;
}




void setup() {

  Serial.begin(115200);
  delay(500); // allow time for the Serial line to get up to speed  

  
  // Setup and initialize all the LED pins for this board
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
  pinMode(led7, OUTPUT);
  
  // GPIOs used for SAO interactions. More to do here maybe.
  pinMode(GPIO_1, OUTPUT);
  digitalWrite(GPIO_1, HIGH);
  pinMode(GPIO_2, OUTPUT);
  digitalWrite(GPIO_2, HIGH);

  // Wifi Pin configuration
  pinMode(wifi_on_off_pin, INPUT);

  // Setup the WiFi based upon the setting of the switch 
  if(digitalRead(wifi_on_off_pin) == HIGH){
    wifi_switch_prev_state = LOW;
    enableWiFi();
  }else{
    wifi_switch_prev_state = HIGH;
    disableWiFi();
  }

  Timer0_Cfg = timerBegin(0, 240, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 100000, true);
  timerAlarmEnable(Timer0_Cfg);

  Wire.begin(8, 9);   // More to follow on if/what we decide to do with I2C

  turnOnLEDs();
  led2_Blue();
  twinkle_stars();

  touchAttachInterrupt(touch1, process_touch1, threshold_1);
  touchAttachInterrupt(touch2, process_touch2, threshold_2);
  touchAttachInterrupt(touch3, process_touch3, threshold_3);
  touchAttachInterrupt(touch4, process_touch4, threshold_4);
  touchAttachInterrupt(touch5, process_touch5, threshold_5);
  
  
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
            client.println(); // Finish the header on the newline

            client.println(index_html_code);

            

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

  check_wifi_state();
  
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
  digitalWrite(led7, HIGH);
}

void turnOffLEDs(){
  digitalWrite(led1_R, LOW);
  digitalWrite(led1_G, LOW);
  digitalWrite(led1_B, LOW);

  digitalWrite(led2_R, LOW);
  digitalWrite(led2_G, LOW);
  digitalWrite(led2_B, LOW);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
  digitalWrite(led6, LOW);  
  digitalWrite(led7, LOW);  
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
  analogWrite(led7, random(min_val,max_val));
}
