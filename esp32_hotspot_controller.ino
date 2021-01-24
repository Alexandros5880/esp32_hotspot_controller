#include "SPIFFS.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"


const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";


// Fixed IP
IPAddress Ip(192, 168, 1, 1);
IPAddress NMask(255, 255, 255, 0);


// Set web server port number to 80
AsyncWebServer server(80);




int button = 23;
int pressed_mill = 0;
bool pressed = false;

// Pins
int pin_1 = 27;
int pin_2 = 26;
int pin_3 = 32;
int pin_4 = 33;





const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Controll Center</title>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
    <style>
      .switch {
        position: relative;
        display: inline-block;
        width: 60px;
        height: 34px;
      }
      .switch input { 
        opacity: 0;
        width: 0;
        height: 0;
      }
      .slider {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #ccc;
        -webkit-transition: .4s;
        transition: .4s;
      }
      .slider:before {
        position: absolute;
        content: "";
        height: 26px;
        width: 26px;
        left: 4px;
        bottom: 4px;
        background-color: white;
        -webkit-transition: .4s;
        transition: .4s;
      }
      input:checked + .slider {
        background-color: #00af26;
      }
      input:focus + .slider {
        box-shadow: 0 0 1px #00af26;
      }
      input:checked + .slider:before {
        -webkit-transform: translateX(26px);
        -ms-transform: translateX(26px);
        transform: translateX(26px);
      }
      .slider {
        border-radius: 34px;
      }
      .slider:before {
        border-radius: 50%;
      }
      /*
      #save_button:before {
        background-color: none;
      }
      #save_button:hover {
        background-color: rgb(73, 133, 73);
      }
      #save_button:focus {
        background-color: rgb(27, 83, 27);
      }
      */
      </style>
  </head>
  <body>
      <h2><b>Reverse Route</b></h2>
      <label class="switch">
        <input type="checkbox" id="reverse" name="reverse" >
        <span class="slider"></span>
      </label>
      <br/><br/>
      <label for="outputone"><p style="font-size: 21px;">Delay pin 2:</p></label>
      <input type="number" style="font-size: 21px;" id="outputone" name="outputone" />
      <br/><br/>
      <label for="outputtow"><p style="font-size: 21px;">Delay pin 3:</p></label>
      <input type="number" style="font-size: 21px;" id="outputtow" name="outputtow" />
      <br/><br/>
      <label for="outputthree"><p style="font-size: 21px;">Delay pin 4:</p></label>
      <input type="number" style="font-size: 21px;" id="outputthree" name="outputthree" />
      <br/><br/>
      <br/>
      <div>
        <button id="save_button" style="font-size: 21px; border-radius: 15%;" name="save" onclick="SaveRequest()">Save</button>
        <button id="on_off" style="font-size: 21px; border-radius: 15%;" name="on_off" onclick="OnOff()">ON/OFF</button>
        <script>
          function SaveRequest() {
            let reverse_val = document.getElementById("reverse").checked;
            let outputone_val = document.getElementById("outputone").value;
            let outputtow_val = document.getElementById("outputtow").value;
            let outputthree_val = document.getElementById("outputthree").value;
            var url = "/save?reverse="+reverse_val+"&outputone="+outputone_val+"&outputtow="+outputtow_val+"&outputthree="+outputthree_val; // http://192.168.1.1:80
            var client = new XMLHttpRequest();
            client.open("GET", url, true);
            client.send(null); 
          }
          function OnOff() {
            var url = "/run"; // http://192.168.1.1:80
            var client = new XMLHttpRequest();
            client.open("GET", url, true);
            client.send(null); 
          }
        </script>
      </div>
      <br/><br/>
      <br/>
  </body>
</html>
)rawliteral";


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}





// Variable to saves the results from html switchs and labels
boolean checkbox_rev = false;
int val_1 = 0, val_2 = 0, val_3 = 0;






void pins_on() {
  digitalWrite(pin_1,HIGH);
  delay(val_1);
  digitalWrite(pin_2,HIGH);
  delay(val_2);
  digitalWrite(pin_3,HIGH);
  delay(val_3);
  digitalWrite(pin_4,HIGH);
}

void pins_off() {
  digitalWrite(pin_1,LOW);
  delay(val_1);
  digitalWrite(pin_2,LOW);
  delay(val_2);
  digitalWrite(pin_3,LOW);
  delay(val_3);
  digitalWrite(pin_4,LOW);
}

void pins_on_reverse() {
  digitalWrite(pin_4,HIGH);
  delay(val_1);
  digitalWrite(pin_3,HIGH);
  delay(val_2);
  digitalWrite(pin_2,HIGH);
  delay(val_3);
  digitalWrite(pin_1,HIGH);
}

void pins_off_reverse() {
  digitalWrite(pin_4,LOW);
  delay(val_1);
  digitalWrite(pin_3,LOW);
  delay(val_2);
  digitalWrite(pin_2,LOW);
  delay(val_3);
  digitalWrite(pin_1,LOW);
}



// Start Proccess 
void start_proccess() {
  pressed = !pressed;
  // On Pins
  if (pressed) {
    if (! checkbox_rev) {
      pins_on();
    } else {
      pins_on_reverse();
    }
  }
  // Off Pins
  else {
    if (! checkbox_rev) {
      pins_off();
    } else {
      pins_off_reverse();
    }
  }
}








// Create WIFI HotSpot
void create_wifi_hotspot() {
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  delay(100);
  // Seting Fixed IP
  WiFi.softAPConfig(Ip, Ip, NMask);
  // Fixed IP
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}


// Start The Server
void start_server() {
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", MAIN_page);
  });
  // When Click Save Button
  server.on("/save", HTTP_GET, [] (AsyncWebServerRequest *request) {
    int paramsNr = request->params();
    for ( int i = 0; i < paramsNr; i++ ) {
      AsyncWebParameter* p = request->getParam(i);
      if ( p->name() == "reverse" )     { checkbox_rev = (p->value() == "true"? true : false); }
      if ( p->name() == "outputone" )   { val_1 = p->value().toInt(); }
      if ( p->name() == "outputtow" )   { val_2 = p->value().toInt(); }
      if ( p->name() == "outputthree" ) { val_3 = p->value().toInt(); }
    }
    Serial.println(String(checkbox_rev) + "  " + String(val_1) + "  " + String(val_2) + "  " + String(val_3));
    request->send(200, "text/html", MAIN_page);
  });
  // When Click Save Button
  server.on("/run", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.println("Run Proccess.");
    start_proccess();
    request->send(200, "text/html", MAIN_page);
  });
  // On Error
  server.onNotFound(notFound);
  // Start
  server.begin();
}

















void setup() {
  Serial.begin(115200);
  pinMode(button,INPUT);
  pinMode(pin_1,OUTPUT);
  pinMode(pin_2,OUTPUT);
  pinMode(pin_3,OUTPUT);
  pinMode(pin_4,OUTPUT);
  create_wifi_hotspot();
  start_server();
}







void loop(){

  // If reverce is checked
  if (digitalRead(button)) {
    pressed_mill++;
    if (pressed_mill == 1000) {pressed_mill = 2;}
    delay(150);
  } else {
    pressed_mill = 0;
  }
  // If Pin GPIO 23 == HIGHT
  if ( (pressed_mill == 1) ) {
    start_proccess();
  }
  
}
