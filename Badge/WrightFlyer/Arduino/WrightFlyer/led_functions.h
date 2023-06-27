/*
 * Aerospace Village Def Con Badge for 2023
 * Author: @cybertestpilot
 * 
 * Functions for the LEDs
 */

typedef struct{
  short red;
  short green;
  short blue;
} RGB;

RGB color_array[] = {{125,0,0}, {0,125,0}, {0,0,125}, {125,125,0}, {0,125,125}, {125,0,125}, {125,90,0}};

RGB color_ring_color = {200,200,200};
volatile short color_index = 0;


void set_Wright_color(RGB value){
  analogWrite(led_pin_R, value.red);
  analogWrite(led_pin_G, value.green);
  analogWrite(led_pin_B, value.blue);
}
