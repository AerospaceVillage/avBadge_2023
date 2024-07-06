/*
  Aerospace Village Def Con Badge for 2023
  Author: Dan Allen
  Title: The Wright Stuff

  Idea: Enjoy the nostalgia of 120 years of avaition with a heavy space theme

  Design:
     - Artowrk by flysurreal.com
     - Twinkle some starts and other LEDs
     - Have a WiFi connection that people can login into the spacecraft and make changes
        - Have a manual override switch that can enable/disable the WiFi for battery saving
     - Utilize the touch capacitance of the ESP32-S2 MCU for some user based interactions
     - Be very verbose/spammy on the Serial line for user intaction options should they connect low level
     - Perhaps use I2C in a creative way?
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "Wire.h"


#define GPIO_1 10
#define GPIO_2 11

boolean GPIO_1_isOn = false;
boolean GPIO_2_isOn = false;

#define FLUX_ADR 0x70
volatile boolean scan_i2c = false;
volatile boolean flux_capacitor_present = false;
volatile boolean flux_play_track = true;
volatile short flux_track = 0x7;
#define flux_track_limit 11
int flux_print_count = 0;

void flux_change_track();
void flux_stop();

boolean demo_mode = false;
int demo_mode_counter = 0;
int demo_mode_limit = 500;

#include "led_functions.h"
#include "touch_functions.h"
#include "web_server_functions.h"




hw_timer_t *Timer0_Cfg = NULL;
hw_timer_t *Timer1_Cfg = NULL;
hw_timer_t *Timer2_Cfg = NULL;

void IRAM_ATTR Timer0_ISR() {
  twinkle_stars(percentage);  
}

void IRAM_ATTR Timer1_ISR() {
  // This function is called to trigger an inspection to look for the Flux SAO
  scan_i2c = !scan_i2c;
}

void IRAM_ATTR Timer2_ISR() {
  if (pulse == true) {
    pulse_center();
  }
}



void setup() {

  Serial.begin(115200);
  delay(500); // allow time for the Serial line to get up to speed

  setup_LEDs();
  setup_WiFi();
  setup_touch_interface();

  // GPIOs used for SAO interactions. More to do here maybe.
  pinMode(GPIO_1, OUTPUT);
  digitalWrite(GPIO_1, GPIO_1_isOn);
  pinMode(GPIO_2, OUTPUT);
  digitalWrite(GPIO_2, GPIO_2_isOn);

  //Setup the Timers to be used to twinkle the LEDs and scan I2C
  Timer0_Cfg = timerBegin(0, 80, true);
  timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 10000, true);
  timerAlarmEnable(Timer0_Cfg);

  Timer1_Cfg = timerBegin(1, 8000, true);
  timerAttachInterrupt(Timer1_Cfg, &Timer1_ISR, true);
  timerAlarmWrite(Timer1_Cfg, 10000, true);
  timerAlarmEnable(Timer1_Cfg);

  Timer2_Cfg = timerBegin(2, 540, true);
  timerAttachInterrupt(Timer2_Cfg, &Timer2_ISR, true);
  timerAlarmWrite(Timer2_Cfg, 8000, true);
  timerAlarmEnable(Timer2_Cfg);

  //Setup I2C on the SAO pins to do stuff
  Wire.setPins(8, 9);
  Wire.begin();
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

  if(scan_i2c == true){
    //Serial.println("Scanning for Flux Capacitor on I2C");
    scan_i2c = false;

    Wire.beginTransmission (FLUX_ADR);
    if(Wire.endTransmission() == 0){
      flux_capacitor_present = true; //Yeah we found a connected Flux!
      flux_print_count++;

      if(flux_print_count >= 50){
        Serial.println("Oh my goodness... is a Flux Capacitor attached?");
        flux_print_count = 0;
      }
    }else{
      flux_capacitor_present = false; //Bummer... but maybe something else is connected?
    }


    if(flux_capacitor_present == true){
      if(flux_play_track == true){
        Wire.beginTransmission(FLUX_ADR); // Address 'F' in ASCII for Flux
        Wire.write(0x81); //Play command... or the Play command (0x01) with 0x80
        Wire.write(flux_track); //Top Gun song be default!
        Wire.endTransmission(true);
        //Serial.println("You're in for a treat listening to this sick song!");
        
        //As a way to slow down the send command, just look for it again
        flux_capacitor_present = false;
      }
    }    
  }

  if(demo_mode == true){
    demo_mode_counter+= 1;
    if(demo_mode_counter >= demo_mode_limit){
      percentage = random(0,20);
      demo_mode_counter = 0;
      color_index += 1;
      if(color_index > sizeof(color_array)/sizeof(RGB)){
        color_index = 0;
      }
      color_ring_color = color_array[color_index];
    }    
  }
}

void flux_stop(){
  Serial.println("Flux stop being sent");
  //Regarxless of connection or not, presence or not... send the STOP command... 0x82 and an extra padding byte
  Wire.beginTransmission(FLUX_ADR); // Address 'F' in ASCII for Flux
  Wire.write(0x82); //Stop command... OR with 0x80
  Wire.write(0x82); // Fill the buffer up
  Wire.endTransmission(true);
}

void flux_change_track(){
  Serial.println("Flux change detection being processed.");
  flux_stop();
  if(flux_capacitor_present == true){
    Wire.beginTransmission(FLUX_ADR); // Address 'F' in ASCII for Flux
    Wire.write(0x81); //Play command... OR the Play command (0x01) with 0x80
    Wire.write(flux_track);
    Wire.endTransmission(true);
          
    //As a way to slow down the send command, just look for it again
    flux_capacitor_present = false;
  }
}
