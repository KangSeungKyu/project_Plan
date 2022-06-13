#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <SPIFFS.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <Hash.h>
  #include <FS.h>
#endif
#include <ESPAsyncWebServer.h>
#include "kp_dfplayer_mini.h"

#define VOLUME 30

AsyncWebServer server(80);
kp_dfplayer_mini mp3(Serial2);

const char* ssid = "TP-LINK_1B78";
const char* password = "april1204";

const char* PARAM_STRING = "inputString";
const char* PARAM_INT = "inputInt";
const char* PARAM_FLOAT = "inputFloat";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
  <html>
  <head>
    <title>ESP Input Form</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
      <script>
        function submitMessage() {
          alert("Saved value to ESP SPIFFS");
          setTimeout(function(){ document.location.reload(false); }, 500);   
      }
      </script>
    <style>
    button{
      width: 120px;
      height: 40px;
      color: #fff;
      background-color: #004fff;
      font-size: 12px;
      border: 2px solid #ffffff;
      left:50%;
      top:50%;  
      display: inline;
      margin:auto;
      display:block;
    }
    button:focus {
      outline:0;
    }
    button:hover {
      background: rgba(0,79,255,0.9);
      cursor: pointer;
    }
    h1 {
      color:black;
      text-align:center;
    }
  </style>
  </head>
  <body>
  <h1>VOICE</h1>
  <form action="/get" target="hidden-form">
    inputInt (current value %inputInt%): <input type="number " name="inputInt">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <iframe style="display:none" name="hidden-form"></iframe>

  <button type="botton02" onclick="location.href='http://192.168.43.177/projectFile'">BACK TO</button>
</body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  file.close();
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

String processor(const String& var){
  if(var == "inputInt"){
    return readFile(SPIFFS, "/inputInt.txt");
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);
  mp3.set_volume(VOLUME);
  delay(1000);
  
  #ifdef ESP32
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #else
    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam(PARAM_INT)) {
      inputMessage = request->getParam(PARAM_INT)->value();
      writeFile(SPIFFS, "/inputInt.txt", inputMessage.c_str());
    } else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {  
  int yourInputInt = readFile(SPIFFS, "/inputInt.txt").toInt();
  Serial.print("*** Your inputInt: ");
  Serial.println(yourInputInt);
  delay(5000);

  if ( Serial.read() ){
    int value = yourInputInt;
    if ( ( value > 0 ) && ( value <= 3 ) ){
      mp3.set_volume(VOLUME);
      mp3.play_mp3(1, value);    
    }
  }
}
