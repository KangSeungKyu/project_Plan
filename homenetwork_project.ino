// 라이브러리
#include <esp_now.h>
#include <WiFi.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ESP32Servo.h"

// 서보모터와 각도
Servo myservo;
int angle = 0;

//와이파이 연결
const char* wifi_network_ssid = "TP-LINK_1B78";
const char* wifi_network_password =  "april1204";

const char *soft_ap_ssid = "MyESP32AP";
const char *soft_ap_password = "testpassword";

void initWiFi() {
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  WiFi.begin(wifi_network_ssid, wifi_network_password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  Serial.print("ESP32 IP as soft AP: ");
  Serial.println(WiFi.softAPIP());

  Serial.print("ESP32 IP on the WiFi network: ");
  Serial.println(WiFi.localIP());
}

// 웹 서버 포트 80으로 연결
AsyncWebServer server(80);

// 서보모터 움직임
void servoON() {
  for (int angle = 0; angle <= 60; angle++) {
    myservo.write(angle);
    delay(10);
  }
  delay(1000);

  for (int angle = 60; angle >= 0; angle--) {
    myservo.write(angle);
    delay(10);
  }
  delay(1000);

  myservo.write(0);
  delay(3000);
}

// 수신 받을 데이터 유형
typedef struct struct_message {
  int b;
} struct_message;

// 수신 받을 데이터
struct_message myData;

// 시리얼 모니터에 보이는 데이터의 값
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.println();
}

// 웹 메인
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Web server</title>
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
  <h1>Always</h1>
  
  </br>
  
  <button type="botton01" onclick="location.href='http://192.168.43.177/servoon'">FOOD</button>
  <button type="botton01" onclick="location.href='http://192.168.43.177/wateron'">WATER</button>
  <button type="botton02" onclick="location.href='http://192.168.43.177/voice'">VOICE</button>
  <button type="botton03" onclick="location.href='http://192.168.43.164:8080/stream/video.mjpeg '">Realtime</button>
  
<script>

</script>
</body>
</html>)rawliteral";

// FOOD 페이지
const char index_html2[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Web server</title>
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
  <h1>Always</h1>
  
  </br>
  
  <button type="botton01" onclick="location.href='http://192.168.43.177/servoon'">FOOD ON</button>
  <button type="botton01" onclick="location.href='http://192.168.43.177/projectFile'">BACK TO</button>
  
<script>

</script>
</body>
</html>)rawliteral";

void setup()
{
  Serial.begin(115200);

  initWiFi();

  server.on("/projectFile", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", index_html);
  });

  server.on("/servoon", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", index_html2);
    servoON();
  });

  server.begin();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  myservo.attach(27);
  myservo.write(angle);
  delay(2000);

  if (myservo.attach(27) == false) {
    myservo.attach(27);
  }
}

void loop() {
  if (myData.b) {
    servoON();
    myservo.detach();
    delay(1000);
    ESP.restart();
  }
}
