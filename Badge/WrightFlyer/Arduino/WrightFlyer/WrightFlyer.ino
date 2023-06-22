/*
  Aerospace Village Def Con Badge for 2023
  Author: Dan Allen

  Idea: Celebrate the 120 year anniversary of first flight 12/7/1903 while at Def Con in 2023 (albeit a few months early).

  Design:
     - Spin 2 motors with attached laser cut propellers
     - backlight some aspects of the board with LEDs
     - Have a WiFi connection that people can login into the aircraft and make changes
        - Have a manual override switch that can disable the WiFi portion, use as a teaching aid to aircraft design and safety backups
        - Have a switch that can turn on/off the wifi server - save battery life and improve securty posture


*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "index_html.h"
#include "blueprint_html.h"

#define software_version 1.0

#define led_pin_R 4
#define led_pin_G 5
#define led_pin_B 6

#define motor_pin 10
#define motor_pin_backup 11

#define wifi_on_off_pin 12

#define GPIO_1 13
#define GPIO_2 14

// Motor on/off variables
#define motor_on_duration 5000
#define motor_off_duration 70000

boolean motorOn_user = true;


#include "led_functions.h"
#include "web_server_functions.h"


hw_timer_t *Timer0_Cfg = NULL;
volatile boolean motorOn_interrupt = false;



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



void setup() {
  Serial.begin(115200);
  delay(500); // allow time for the Serial line to get up to speed

  Serial.print("Software Version: ");
  Serial.println(software_version);

  //Setup and initialize all the LED pins
  pinMode(led_pin_R, OUTPUT);
  pinMode(led_pin_G, OUTPUT);
  pinMode(led_pin_B, OUTPUT);

  //Setup and initialize the motor pins
  pinMode(motor_pin, OUTPUT);
  pinMode(motor_pin_backup, OUTPUT);

  // GPIOs used for SAO interactions. More to do here maybe.
  pinMode(GPIO_1, OUTPUT);
  digitalWrite(GPIO_1, HIGH);
  pinMode(GPIO_2, OUTPUT);
  digitalWrite(GPIO_2, HIGH);

  //Timer setup to pulse modulate the motors correctly
  Timer0_Cfg = timerBegin(0, 240, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, false);
  timerAlarmWrite(Timer0_Cfg, 400000, true);
  timerAlarmEnable(Timer0_Cfg);

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
}


void loop() {

  //Service the client if connected
  process_client();

  //Check the Wifi switch for status
  check_wifi_state();

  //Set the center ring as it may have been changed
  set_Wright_color(color_ring_color);

}
