//라이브러리 선언
#include <esp_now.h>
#include <WiFi.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// 핀 선언
#define PIN_IN3  27
#define PIN_IN4  26
#define PIN_ENB  14

// Station모드 연결
const char* wifi_network_ssid = "TP-LINK_1B78";
const char* wifi_network_password =  "april1204";

// AP모드 연결
const char *soft_ap_ssid = "MyESP32AP";
const char *soft_ap_password = "testpassword";

// WiFi 초기화
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

// 전달할 데이터 타입과 변수 선언
typedef struct struct_message {
  int b;
} struct_message;

// 전달할 데이터 myData
struct_message myData;

// 시리얼 모니터에 보이는 데이터의 값
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.println();
}

// 워터펌프 작동 함수
void pumpON() {
  analogWrite(PIN_ENB, 150);
  digitalWrite(PIN_IN3, HIGH); 
  digitalWrite(PIN_IN4, LOW);
  delay(10000);

  analogWrite(PIN_ENB, 0);
  digitalWrite(PIN_IN3, LOW); 
  digitalWrite(PIN_IN4, LOW);
  delay(3000);
}

// 워터펌프 작동 페이지
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
  
  <button type="botton01" onclick="location.href='http://192.168.43.177/wateron'">WATER ON</button>
  <button type="botton01" onclick="location.href='http://192.168.43.177/projectFile'">BACK TO</button>
  
<script>

</script>
</body>
</html>)rawliteral";

void setup() {
  //시리얼 모니털 시작
  Serial.begin(115200);

  // 와이파이 초기화
  initWiFi();

  server.on("/wateron", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", index_html);
    pumpON();
  });

  server.begin();

  //esp_now 시작
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // 데이터 수신지
  esp_now_register_recv_cb(OnDataRecv);

  // 워터펌프 핀 연결
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_ENB, OUTPUT);
}

void loop() {
  // 버튼이 눌리면 워터펌프가 작동되고, esp32가 재시작 된다. 
  if (myData.b) {
    pumpON();
    ESP.restart();
  }
}
