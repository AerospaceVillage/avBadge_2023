/*
 * Functions for the LEDs
 */


void twinkle_stars(short percent){
  for(int i=0; i<7; i++){
    digitalWrite(white_leds[i], HIGH);
  }

  if(random(0, 100) > percent){    // Pick a percentange of times to turn a LED off
    digitalWrite(white_leds[random(0,7)], LOW);     
  }  
}

void pulse_center(){
  scale -= (.005 * scale_dir);

  if(scale <= 0){
    scale_dir = scale_dir * -1;
    scale = 0;
  }

  if(scale >= 1){
    scale_dir = scale_dir * -1;
    scale = 1;
  }  
}


void led2_color(RGB value, float scale){
  analogWrite(led2_R, (short)(value.red*scale));
  analogWrite(led2_G, (short)(value.green*scale));
  analogWrite(led2_B, (short)(value.blue*scale));
}

void alien_LED(boolean on_off){
  digitalWrite(led1_R, on_off);
  digitalWrite(led1_G, on_off);
  digitalWrite(led1_B, on_off);
}