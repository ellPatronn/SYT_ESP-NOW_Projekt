# SYT_ESP-NOW_Projekt
ITP Projekt IoT und ESP-NOW
Bewegungserkennungs- und Übertragungssystem

Gruppenmitglieder: Emin Kizilirmak, Al Mafrachi Hussein
Datum: 20.05.2026

1. Einführung

Im Umfeld moderner IoT-Anwendungen spielt die drahtlose Kommunikation zwischen Mikrocontrollern eine zentrale Rolle. Besonders ESP-NOW bietet hier eine einfache Möglichkeit, Daten ohne klassische WLAN-Verbindung direkt zwischen ESP32-Geräten auszutauschen.

Im vorliegenden Projekt wird ein System umgesetzt, das Bewegungen erkennt und diese Information drahtlos an ein zweites Gerät überträgt. Dort wird der aktuelle Status für den Benutzer sichtbar dargestellt.

2. Projektbeschreibung

Ziel des Projekts ist die Entwicklung eines einfachen, aber funktionalen IoT-Systems zur Bewegungserkennung.

Ein ESP32 ist mit einem PIR-Sensor verbunden, der Änderungen im Infrarotbereich registriert und dadurch Bewegungen erkennt. Der ermittelte Zustand wird anschließend per ESP-NOW an einen zweiten ESP32 gesendet.

Dieser zweite Mikrocontroller empfängt die Daten und zeigt den aktuellen Zustand auf einem OLED-Display an. Dadurch kann der Nutzer jederzeit erkennen, ob eine Bewegung erkannt wurde oder nicht.

3. Technische Grundlagen
IoT (Internet of Things)

Das Internet der Dinge beschreibt die Vernetzung physischer Geräte, die Daten erfassen und untereinander austauschen können. Mikrocontroller wie der ESP32 eignen sich dafür besonders, da sie kompakt, günstig und leistungsfähig sind.

ESP-NOW Kommunikation

ESP-NOW ist ein von Espressif entwickeltes Funkprotokoll, das eine direkte Verbindung zwischen ESP-Geräten ermöglicht. Ein Router ist dabei nicht erforderlich, wodurch schnelle und stromsparende Datenübertragung möglich wird.

PIR-Bewegungssensor

Der verwendete PIR-Sensor reagiert auf Veränderungen der Infrarotstrahlung. Sobald sich eine Wärmequelle (z. B. ein Mensch) bewegt, wird ein digitales Signal ausgelöst.

OLED Anzeige

Das OLED-Display dient zur Ausgabe der empfangenen Informationen. Es stellt den aktuellen Zustand klar und einfach dar.

4. Vorgehensweise

Zu Beginn wurden alle benötigten Bauteile zusammengestellt, darunter zwei ESP32-Module, ein PIR-Sensor sowie ein OLED-Display.

Im nächsten Schritt wurde der PIR-Sensor an einen ESP32 angeschlossen und so programmiert, dass er bei erkannter Bewegung ein Signal ausgibt.

Anschließend wurde ESP-NOW auf beiden Mikrocontrollern eingerichtet. Ein Gerät übernimmt dabei die Rolle des Senders, das andere die des Empfängers.

Der Sender überträgt regelmäßig den aktuellen Sensorzustand. Der Empfänger verarbeitet diese Daten und stellt sie anschließend auf dem Display dar.

Zum Abschluss wurde das System getestet und auf stabile Datenübertragung überprüft.
   
#include <WiFi.h>
#include <esp_now.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

typedef struct struct_message {
  int state;
} struct_message;

struct_message data;

// ESP-NOW Empfang (neue API)
void OnDataRecv(const esp_now_recv_info_t *info,
                const uint8_t *incomingData,
                int len) {

  memcpy(&data, incomingData, sizeof(data));

  Serial.print("Empfangen: ");
  Serial.println(data.state);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);

  if (data.state == 1) {
    display.println("BEWEGUNG");
  } else {
    display.println("KEINE");
  }

  display.display();
}

void setup() {

  Serial.begin(115200);

  // OLED starten
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED nicht gefunden");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("READY");
  display.display();

  // ESP-NOW starten
  WiFi.mode(WIFI_STA);

  Serial.print("ESP2 MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Fehler");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // nichts nötig
}
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
}       

6. Fazit

Das Projekt zeigt, wie sich einfache IoT-Systeme mit ESP32 und ESP-NOW realisieren lassen. Die Bewegungserkennung funktioniert zuverlässig und die Daten werden ohne Verzögerung an ein zweites Gerät übertragen.

Besonders praktisch ist dabei die direkte Kommunikation ohne WLAN-Router, wodurch das System flexibel einsetzbar bleibt.
