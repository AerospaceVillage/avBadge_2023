/*
 * All the Wright Stuff Web Server and Client Functions
 */

 #define wifi_on_off_pin 18

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
boolean with_cookie = true;

// Function Calls
void process_custom_color(String requestBody);
void send_header(WiFiClient client, boolean withCookie);

#include "index_html.h"
#include "wrightX_html.h"



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
          if (currentLine.indexOf("/wrightX.html") > 0){
              Serial.println("WrightX link called");
              send_header(client, false);
              client.println(wrigthX_html_code);
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

            // Process/handle the POST requests as evidenced by the requestBody string
            if(requestBody.indexOf("favcolor=%23") > 0){
              process_custom_color(requestBody);
            }else if(requestBody.indexOf("GPIO_1") > 0){
              Serial.println("GPIO 1 Toggled");
              GPIO_1_isOn = !GPIO_1_isOn;
              digitalWrite(GPIO_1, GPIO_1_isOn);
            }else if(requestBody.indexOf("GPIO_2") > 0){
              Serial.println("GPIO 2 Toggled");
              GPIO_2_isOn = !GPIO_2_isOn;
              digitalWrite(GPIO_2, GPIO_2_isOn);
            }

            //Possibly set a cookie
            send_header(client, with_cookie);

            client.println(index_html_code);

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

void setup_WiFi(){
  // Wifi Pin configuration
  pinMode(wifi_on_off_pin, INPUT);


  // Setup the WiFi based upon the setting of the switch
  if (digitalRead(wifi_on_off_pin) == HIGH) {
    wifi_switch_prev_state = LOW;
    enableWiFi();
  } else {
    wifi_switch_prev_state = HIGH;
    disableWiFi();
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

  Serial.print("Color: {"); Serial.print(r); Serial.print(','); Serial.print(g); Serial.print(','); Serial.print(b); Serial.println('}');
}


void send_header(WiFiClient client, boolean withCookie){
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  if(withCookie == true){
    client.println("Set-Cookie:aerospacevillage=Still need last years badge? Find @cybertestpilot and inform him \"The Eagle Has Landed\"");
    with_cookie = false;
  }  
  client.println(); // Finish the header on the newline
}
