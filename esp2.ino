#include <WiFi.h>
#include <esp_now.h>

#define SENSOR_PIN 27

// MAC-Adresse von ESP2
uint8_t esp2Address[] = {0x00, 0x70, 0x07, 0x1D, 0x92, 0x7C};

typedef struct struct_message {
  int state;
} struct_message;

struct_message data;

// Neue Callback-Version für aktuelle ESP32 Boards
void OnDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {

  Serial.print("Send Status: ");

  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("OK");
  } else {
    Serial.println("FAIL");
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(SENSOR_PIN, INPUT);

  WiFi.mode(WIFI_STA);

  Serial.print("ESP1 MAC: ");
  Serial.println(WiFi.macAddress());

  // ESP-NOW starten
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Fehler");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // Peer hinzufügen
  esp_now_peer_info_t peerInfo = {};

  memcpy(peerInfo.peer_addr, esp2Address, 6);

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer Fehler");
    return;
  }
}

void loop() {

  // PIR oder KY-024 lesen
  data.state = digitalRead(SENSOR_PIN);

  // Daten senden
  esp_now_send(esp2Address, (uint8_t *) &data, sizeof(data));

  Serial.print("Gesendet: ");
  Serial.println(data.state);

  delay(300);
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
