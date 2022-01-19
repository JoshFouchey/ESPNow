#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>

constexpr char WIFI_SSID[] = "";
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i = 0; i < n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}

uint8_t ap_mac[] = {};
// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {};

typedef struct struct_message {
  char systemUUID[37];
  char sensorId[3];
  char sensorType[10];
  char motionDedected[6];
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }
  else {
    Serial.println("Delivery fail");
  }
  //15 mins
  ESP.deepSleep(0);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  sendMotionData();
}

void loop() {
}

void sendMotionData() {
  strcpy(myData.systemUUID, "75a8c399-10e6-4311-b8f7-481fd2a2a14e");
  strcpy(myData.sensorId, "3");
  strcpy(myData.motionDedected, "true");
  strcpy(myData.sensorType, "motion");

  Serial.println(myData.sensorId);
  Serial.println(myData.motionDedected);
  Serial.println(myData.sensorType);
  Serial.println(myData.systemUUID);

  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
}
