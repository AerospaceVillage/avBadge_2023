/*
 * All the Wright Stuff Web Server and Client Functions
 */

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

// Function Calls
void process_custom_color(String requestBody);



void process_client() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    //Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    String requestBody = "";
    
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor

        
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {            

            while (client.available()) {
              requestBody += (char)client.read();
            }

            if (requestBody.length()) {
              Serial.print("Request body =");
              Serial.println(requestBody);
              Serial.println();
            }

            // Process/handle the POST requests as evidenced by the requestBody string
            if(requestBody.indexOf("color=%23") > 0){
              // Process "favcolor" but that alone will be index == 0 so search for substring of
              process_custom_color(requestBody);
            }else if(requestBody.indexOf("winkle") > 0){
              Serial.println("Twinkle Pressed");
              twinkle_percentage = requestBody.substring(8, 2).toInt();
            }

            

            
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println(); // Finish the header on the newline


            char index_html[5000];            
            sprintf(index_html, index_html_code, color_index); 

            client.println(index_html);

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

        

        

        if (currentLine.endsWith("GET /C")) {
          Serial.println("Change Color button pressed");
          color_index += 1;
          if(color_index > sizeof(color_array)/sizeof(RGB)){
            color_index = 0;
          }
          color_ring_color = color_array[color_index];      
        }

        if (currentLine.endsWith("GET /A")) {
          alienFound = !alienFound; 
          Serial.println("Toggle Alien button pressed");
        }

        if (currentLine.endsWith("GET /P")) {
          Serial.println("Pulse button pressed");
          pulse = !pulse; 
        }
      }

      
    }
    // close the connection:
    client.stop();
    //Serial.println("Client Disconnected.");
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

    WiFi.setSleep(false);
  
    WiFi.softAPConfig(Ip, Ip, NMask);
    Serial.println("AP config set");
    
    server.begin();
    Serial.println("Server started");
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


void process_custom_color(String requestBody){
  Serial.println("Found a favcolor!!!");
  //Serial.println(currentLine);          
  int _start = requestBody.indexOf("favcolor=%23") + 12;

  String tmp = requestBody.substring(_start, _start+7);
  char fav_color_array[7];
  tmp.toCharArray(fav_color_array, 7);
  long favColor = strtol(fav_color_array , NULL, 16);

  short r = favColor >> 16;
  short g = favColor >> 8 & 0xff;
  short b = favColor & 0xff;

  RGB x = {r,g,b};
  color_ring_color = x;  

  alien_color = x;  
}
