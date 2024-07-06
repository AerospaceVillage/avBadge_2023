/*
 * Functions and Definitions for the LEDs
 */
#define led1_R 12
#define led1_G 13
#define led1_B 14

#define led2_R 15
#define led2_G 16
#define led2_B 17       // Are these really switched in the datasheet?

#define led1 19
#define led2 20
#define led3 21
#define led4 33
#define led5 34
#define led6 35
#define led7 36


typedef struct{
  short red;
  short green;
  short blue;
} RGB;
 

// Function Definitions
void setup_LEDs();
void twinkle_stars(short percetage);
void pulse_center();
void led2_color(RGB rgb, float scale);
void alien_LED_isOn(boolean on_off);
void set_alien_color(RGB value);
void set_center_color(RGB value, float scale);


short white_leds [7] = {led1, led2, led3, led4, led5, led6, led7};

RGB color_array[] = {{255,0,0}, {0,255,0}, {0,0,255}, {125,125,0}, {0,125,125}, {125,0,125}, {255,255,255}};

RGB color_ring_color = color_array[0];
RGB alien_color = RGB {255,0,255};
volatile short color_index = 0;

volatile boolean pulse = false;
volatile float scale = 1;
volatile short scale_dir = -1;

volatile boolean alienFound = false;
volatile short percentage = 92;

void setup_LEDs(){
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

  set_center_color(color_array[color_index], scale);
}

void twinkle_stars(short percent){
  for(int i=0; i<7; i++){
    digitalWrite(white_leds[i], HIGH);
  }

  if(random(0, 100) > percent){    // Pick a percentange of times to turn a LED off
    digitalWrite(white_leds[random(0,7)], LOW);     
  }  
}

void pulse_center(){
  scale -= (.01 * scale_dir);

  if(scale <= 0){
    scale_dir = scale_dir * -1;
    scale = 0;
  }

  if(scale >= 1){
    scale_dir = scale_dir * -1;
    scale = 1;
  }
}


void set_center_color(RGB value, float scale){
  analogWrite(led2_R, (short)(value.red*scale));
  analogWrite(led2_G, (short)(value.green*scale));
  analogWrite(led2_B, (short)(value.blue*scale));
}

void set_alien_color(RGB value){
  if(alienFound == true){
    analogWrite(led1_R, value.red);
    analogWrite(led1_G, value.green);
    analogWrite(led1_B, value.blue);
  }else{
    analogWrite(led1_R, 0);
    analogWrite(led1_G, 0);
    analogWrite(led1_B, 0);
  }
}
