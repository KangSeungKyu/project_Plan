#include <esp_now.h>
#include <WiFi.h>

// 수신지 맥 주소
uint8_t broadcastAddress[] = {0x78, 0x21, 0x84, 0x7C, 0xC8, 0x90};
// 터치센서 연결
#define SENSOR_PIN 35

// Station모드 연결
const char* wifi_network_ssid = "TP-LINK_1B78";
const char* wifi_network_password =  "april1204";

// AP모드 연결
const char *soft_ap_ssid = "MyESP32AP";
const char *soft_ap_password = "testpassword";

// 전달할 데이터 타입과 변수 선언
typedef struct struct_message {
  int b;
} struct_message;

// 전달할 데이터 myData
struct_message myData;

// pearInfo변수 선언
esp_now_peer_info_t peerInfo;

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

// 데이터가 제대로 전달 되었는지 확인하기
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// 터치센서의 상태변수 선언
int lastState = LOW;
int currentState;  

void setup() {
  // Serial 모니터 속도 선언
  Serial.begin(115200);
  
  // 터치센서 활성화
  pinMode(SENSOR_PIN, INPUT);
  
  // WiFi 초기화
  initWiFi();
  
  // esp_now 초기화
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // 데이터가 잘 보내졌는지 확인하기
  esp_now_register_send_cb(OnDataSent);
  
  // 데이터가 잘 보내졌는지 확인하기
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // 수신 ESP32 등록
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  
  // currentState를 터치센서와 연결
  currentState = digitalRead(SENSOR_PIN);

  // myData.b의 데이터 값 150
  myData.b = 150;

  // 터치센서가 눌러지면 데이터가 전달됨
  if (lastState == LOW && currentState == HIGH){
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    lastState = currentState;
    delay(1000);
  }
}

void loop() {
}
