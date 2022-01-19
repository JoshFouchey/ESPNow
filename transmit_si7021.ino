#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>

#define si7021Addr 0x40
#define SDA 4
#define SCL 5
constexpr char WIFI_SSID[] = "";
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}

unsigned char ap_mac[18] = {};  
// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {};

typedef struct struct_message {
  char systemUUID[37];
  char sensorId[2];
  char sensorType[10];
  float temp;
  float hum;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
  //15 mins
  ESP.deepSleep(900000000); 
}
void getSiData(unsigned int *_ret_data, byte _i2c_command);
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Wire.begin(SDA,SCL);
   //reset sensor by sending 0xFE command to the Si7021 address
  Wire.beginTransmission(si7021Addr);
  Wire.write(0xFE); // Write reset command
  Wire.endTransmission();
  delay(15); // Default = 15ms
  
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
  sendTempData();
}
 
void loop() {
}

void sendTempData(){
  //sensor returns 2 bytes via I2C. It will be converted to temperature or humidity later
  unsigned int data[2];
   //Send humidity measurement command and get response into the array 'data'
  getSiData(data, 0xE5);

  // Convert the data
  float humidity  = ((data[0] * 256.0) + data[1]);
  humidity = ((125 * humidity) / 65536.0) - 6;

  // Send temperature measurement command
  getSiData(data, 0xE3);

  // Convert the data
  float temp  = ((data[0] * 256.0) + data[1]);
  float celsTemp = ((175.72 * temp) / 65536.0) - 46.85;
  float fahrTemp = celsTemp * 1.8 + 32;
  // Set values to send
  strcpy(myData.systemUUID, "75a8c399-10e6-4311-b8f7-481fd2a2a14e");
  strcpy(myData.sensorType, "temp");
  strcpy(myData.sensorId, "1");
  myData.temp = fahrTemp;
  myData.hum = humidity;  
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
}


void getSiData(unsigned int *_ret_data, byte _i2c_command)
{
  Wire.beginTransmission(si7021Addr);
  Wire.write(_i2c_command);
  Wire.endTransmission();
  delay(85);
 
  Wire.requestFrom(si7021Addr, 2);

  if(Wire.available() == 2)
  {
    _ret_data[0] = Wire.read();
    _ret_data[1] = Wire.read();
  }
}
