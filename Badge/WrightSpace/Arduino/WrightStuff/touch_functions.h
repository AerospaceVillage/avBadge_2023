/*
 * Functions to register all the touch point functions
 */

volatile short touch_sum = 0;
volatile boolean color_lock = false;
volatile boolean touch5_released = false;


void process_touch1(){
  // The XOR in the interrupt helps reset the function when the touch sensor is released
  touch_sum = touch_sum ^ 1;
}

void process_touch2(){
  
  touch_sum = touch_sum ^ 2;
}

void process_touch3(){
  
  touch_sum = touch_sum ^ 4;
  if(percentage >= 80){
    percentage -= 1;
  }
  
}

void process_touch4(){
    
  touch_sum = touch_sum ^ 8;
  if(percentage <= 100){
    percentage += 1;
  }
}

void process_touch5(){
  
  touch_sum = touch_sum ^ 16;
  color_lock = !color_lock;

  if( (touch_sum & 16) == 0 and color_lock == false){
    touch5_released = true;
  }else{
    touch5_released = false;
  }
}


void service_touch_events(){
	if(touch_sum == 1){
	    // Reset all the things that can be toyed with
	    percentage = 100;
	    color_index = 0;
      color_ring_color = color_array[color_index];
      alien_color = color_array[color_index];
	    scale = 1;
	    pulse = false;
      alienFound = false;
	}


	if(touch_sum == 2){
		pulse = true;
	}

	if(touch_sum == 3){
		// Means both touch 1 & 2 pressed
		alienFound = true;
	}else if(touch_sum == 9){
		// Means both touch 1 & 4 pressed
		alienFound = false;
	}


	if(touch5_released == true){
		color_index += 1;
		if(color_index > sizeof(color_array)/sizeof(RGB)){
		  color_index = 0;
		}

		color_ring_color = color_array[color_index];

		touch5_released = false;
	}
}
