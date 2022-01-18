#include <ESP8266WiFi.h>
#include <espnow.h>
//START: MQTT related statements
#include <PubSubClient.h>
#include <Arduino_JSON.h>

const char ssid[] = "fouchey";
const char password[] = "";
const char SYSTEM_UUID[37] = "";
char pubTopicBase[] = "mqtt/sensor/";    
   

WiFiClient espClient;
PubSubClient client(espClient);
//END: MQTT related statements

uint8_t SENSOR_1_MAC_ADDR[] = {};
uint8_t SENSOR_2_MAC_ADDR[] = {};
uint8_t SENSOR_3_MAC_ADDR[] = {};

typedef struct temp_sensor {
  char systemUUID[37];
  char sensorId[2];
  char sensorType[10];
  float temp;
  float hum;
} temp_sensor;

temp_sensor tempSensor;

typedef struct motion_sensor {
  char systemUUID[37];
  char sensorId[3];
  char sensorType[10];
  char motionDedected[6];
} motion_sensor;

motion_sensor motionSensor;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  if(memcmp(mac_addr, SENSOR_1_MAC_ADDR, sizeof(mac_addr)) == 0 || memcmp(mac_addr, SENSOR_2_MAC_ADDR, sizeof(mac_addr)) == 0){
      handleTempSensorRequest(incomingData, mac_addr);
  }
  if(memcmp(mac_addr, SENSOR_3_MAC_ADDR, sizeof(mac_addr)) == 0){
      handleMotionSensorRequest(incomingData, mac_addr);
  }
}

void handleTempSensorRequest(uint8_t *incomingData, uint8_t * mac_addr) {
    memcpy(&tempSensor, incomingData, sizeof(tempSensor));
    if(memcmp(SYSTEM_UUID, tempSensor.systemUUID, sizeof(SYSTEM_UUID)) == 0){
      JSONVar tempPayload;
      tempPayload["sensorId"] = tempSensor.sensorId;
      tempPayload["sensorType"] = tempSensor.sensorType;
      tempPayload["temp"] = tempSensor.temp;
      tempPayload["hum"] = tempSensor.hum;
      tempPayload["macAddr"] = mac_addr;
      Serial.println(tempPayload);
    }
}

void handleMotionSensorRequest(uint8_t *incomingData, uint8_t * mac_addr) {
    memcpy(&motionSensor, incomingData, sizeof(motionSensor));
    if(memcmp(SYSTEM_UUID, motionSensor.systemUUID, sizeof(SYSTEM_UUID)) == 0){
      JSONVar motionPayload;
      motionPayload["sensorId"] = motionSensor.sensorId;
      motionPayload["sensorType"] = motionSensor.sensorType;
      motionPayload["motionDedected"] = motionSensor.motionDedected;
      motionPayload["macAddr"] = mac_addr;
      Serial.println(motionPayload);
    }
}
  
void setup() {
  uint8_t bootCounter = 0; 

  Serial.begin(115200);     //initialize serial port
  WiFi.mode(WIFI_AP_STA);  
  if(esp_now_init() != 0) {
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);   //this function will get called once all data is sent
}

void loop() {
}
