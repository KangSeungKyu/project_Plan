#include <WiFi.h>
#include <HTTPClient.h>

#define POWER_PIN  25 
#define SIGNAL_PIN 35 
#define SENSOR_MIN 0
#define SENSOR_MAX 521

int value = 0; 
int level = 0; 

const char WIFI_SSID[] = "TP-LINK_1B78";
const char WIFI_PASSWORD[] = "april1204";

String HOST_NAME = "http://192.168.0.101";
String PATH_NAME   = "/insert_temp.php";
// String queryString = "?resval=11";

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  /*
  HTTPClient http;

  http.begin(HOST_NAME + PATH_NAME + queryString); //HTTP
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  */
  pinMode(POWER_PIN, OUTPUT);   
  digitalWrite(POWER_PIN, LOW); 
}

void loop() {
  digitalWrite(POWER_PIN, HIGH);  
  delay(10);                      
  value = analogRead(SIGNAL_PIN); 
  digitalWrite(POWER_PIN, LOW);   

  level = map(value, SENSOR_MIN, SENSOR_MAX, 0, 4); 
  Serial.print("Water level: ");
  Serial.println(level);
  delay(5000);
  String water_level = (String)level;
  String queryString = "?resval="+water_level;

  HTTPClient http;

  http.begin(HOST_NAME + PATH_NAME + queryString); //HTTP
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) {
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
