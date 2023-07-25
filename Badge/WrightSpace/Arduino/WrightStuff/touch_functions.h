/*
 * Functions to register all the touch point functions internally numbered 1-5
 */

#define touch1 1
#define touch2 2
#define touch3 3
#define touch4 4
#define touch5 5

#define threshold_1 20000     // These need to perhaps be set/tuned to the actual fabricated board once completed.
#define threshold_2 20000
#define threshold_3 20000
#define threshold_4 20000
#define threshold_5 20000

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
	    // Reset all the things.
	    percentage = 100;
	    color_index = 0;
      color_ring_color = color_array[color_index];
      alien_color = color_array[color_index];
	    scale = 1;
	    pulse = false;
      alienFound = false;
	}

	else if(touch_sum == 2){
		pulse = true;
	}

	else if(touch_sum == 18){
		// Means both touch 2 & 5 pressed
		alienFound = true;
    Serial.println("Do you believe in aliens?");
	}else if(touch_sum == 24){
		// Means both touch 4 & 5 pressed
		alienFound = false;
	}


	else if(touch5_released == true){
		color_index += 1;
		if(color_index > sizeof(color_array)/sizeof(RGB)){
		  color_index = 0;
		}

		color_ring_color = color_array[color_index];
		touch5_released = false;
	}
}

void setup_touch_interface(){
  touchAttachInterrupt(touch1, process_touch1, threshold_1);
  touchAttachInterrupt(touch2, process_touch2, threshold_2);
  touchAttachInterrupt(touch3, process_touch3, threshold_3);
  touchAttachInterrupt(touch4, process_touch4, threshold_4);
  touchAttachInterrupt(touch5, process_touch5, threshold_5);
}
