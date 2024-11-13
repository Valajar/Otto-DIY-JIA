#include <SoftwareSerial.h>

// Definiere die Pins für SoftwareSerial
#define BT_RX_PIN 10  // RX Pin (Empfang)
#define BT_TX_PIN 11  // TX Pin (Senden)

// Erstelle eine SoftwareSerial-Instanz für das Bluetooth-Modul
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

void setup() {
  // Beginne die serielle Kommunikation mit dem PC
  Serial.begin(9600);  // Standard-Serial für den Monitor
  // Beginne die serielle Kommunikation mit dem Bluetooth-Modul
  BTSerial.begin(9600);  // HC-05/HC-06 kommuniziert normalerweise mit 9600 Baud

  Serial.println("Bluetooth is ready!");
}

void loop() {
  // Wenn Daten vom Bluetooth-Modul empfangen werden
  if (BTSerial.available()) {
    char command = BTSerial.read();  // Lese den empfangenen Befehl
    Serial.print("Empfangener Befehl: ");
    Serial.println(command);

    // Verarbeite den Befehl und steuere Otto oder andere Aktionen
    if (command == 'W') {
      Serial.println("Vorwärts!");
      // Hier kannst du Otto dazu anweisen, vorwärts zu gehen
    } else if (command == 'S') {
      Serial.println("Stop!");
      // Hier kannst du Otto anweisen, zu stoppen
    }
    // Weitere Befehle können hier hinzugefügt werden...
  }

  // Wenn du auch vom PC senden möchtest:
  if (Serial.available()) {
    char pcCommand = Serial.read();
    BTSerial.write(pcCommand);  // Sende den Befehl an das Bluetooth-Modul
  }
}
