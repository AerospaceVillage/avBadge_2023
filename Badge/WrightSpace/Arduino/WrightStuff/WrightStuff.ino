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

#define threshold_1 20000
#define threshold_2 20000
#define threshold_3 20000
#define threshold_4 20000
#define threshold_5 20000


#define led1_R 12
#define led1_G 13
#define led1_B 14

#define led2_R 15
#define led2_G 16
#define led2_B 17       // Are these really switched in the schematic?

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

#include "led_functions.h"
#include "touch_functions.h"
#include "web_server_functions.h"


hw_timer_t *Timer0_Cfg = NULL;


void IRAM_ATTR Timer0_ISR() {
  // This function is called as freqently as the 
  twinkle_stars(twinkle_percentage);

  if(pulse == true){
    pulse_center();
  }
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
  

  Timer0_Cfg = timerBegin(0, 60, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, false);
  timerAlarmWrite(Timer0_Cfg, 10000, true);
  timerAlarmEnable(Timer0_Cfg);

  Wire.begin(8, 9);   // More to follow on if/what we decide to do with I2C 

  touchAttachInterrupt(touch1, process_touch1, threshold_1);
  touchAttachInterrupt(touch2, process_touch2, threshold_2);
  touchAttachInterrupt(touch3, process_touch3, threshold_3);
  touchAttachInterrupt(touch4, process_touch4, threshold_4);
  touchAttachInterrupt(touch5, process_touch5, threshold_5);

  set_center_color(color_array[color_index], scale);

  set_alien_color(color_array[color_index]);
  
}



void loop() {

  //Service the client if connected
  process_client();

  //Check the Wifi switch for status
  check_wifi_state();

  //Process the result of any button press
  service_touch_events();

  //Set the center ring as it may have been changed
  set_center_color(color_ring_color, scale);

  //Set the Alien LED as it may have changed
  set_alien_color(alien_color);

}
