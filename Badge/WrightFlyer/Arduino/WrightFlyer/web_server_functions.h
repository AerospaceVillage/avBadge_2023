/*
 * All the Wright Stuff Web Server and Client Functions
 */

 // The WiFi Credentials
const char* ssid = "WrightFlyer";
const char* password = "bicycles";

// The webserver specifications, ipv4
IPAddress Ip(12, 12, 15, 59);     // Time in seconds of each of the first 4 flights made on 12/3/1903 - https://en.wikipedia.org/wiki/Wright_Flyer#Flight_trials_at_Kitty_Hawk
IPAddress NMask(255, 255, 255, 0);

WiFiServer server(80);

boolean wifi_isOn = true;
boolean wifi_stateChanged = false;
boolean wifi_switch_prev_state = LOW;
boolean with_cookie = true;

// Function Calls
void process_custom_color(String requestBody);
void send_header(WiFiClient client, boolean withCookie);



void process_client() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    //Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    String requestBody = "Request Body=";
    
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor

        
        if (c == '\n') {                    // if the byte is a newline character

          //Have to process GET requests before possible processing of the requestBody (i.e. POST methods)
          if (currentLine.indexOf("/blueprints") > 0){
              Serial.println("Blueprints link called");
              send_header(client, false);
              client.println(blueprint_html_code);
              client.println();
              
              // break out of the while loop:
              break;
          }

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {            

            while (client.available()) {
              requestBody += (char)client.read();
            }

            
            if (requestBody.length() > 13) {
              Serial.println(requestBody);
            }
            

            // Process/handle the POST requests as evidenced by the requestBody string
            if(requestBody.indexOf("favcolor=%23") > 0){
              process_custom_color(requestBody);
            }else if(requestBody.indexOf("motor") > 0){
              motorOn_user = !motorOn_user;
            }            
            
            //Possibly set a cookie
            send_header(client, with_cookie);
    
            client.println(index_html_code);
            //client.println(index_html);

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
  Serial.println("Processing a favcolor!!!");
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

  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
}

void send_header(WiFiClient client, boolean withCookie){
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  if(withCookie == true){
    client.println("Set-Cookie:aerospacevillage=@cybertestpilot thanks you for your participation and support of the Aerospace Village. Let me know what you thought of the challenges!");
    with_cookie = false;
  }  
  client.println(); // Finish the header on the newline
}
