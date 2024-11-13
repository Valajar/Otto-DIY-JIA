#include <SoftwareSerial.h>
#include <Otto.h>
Otto Otto;


#define LeftLeg 2 
#define RightLeg 3
#define LeftFoot 4 
#define RightFoot 5 
#define Buzzer  13 
#define DIN A3 // Data In pin
#define CS A2  // Chip Select pin
#define CLK A1 // Clock pin
#define Orientation 1 // 8x8 LED Matrix orientation  Top = 1, Bottom = 2, Left = 3, Right = 4 


// Definiere die Pins für SoftwareSerial
#define BT_RX_PIN 10  // RX Pin (Empfang)
#define BT_TX_PIN 11  // TX Pin (Senden)

// Erstelle eine SoftwareSerial-Instanz für das Bluetooth-Modul
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

void setup() {
  Otto.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true, Buzzer); //Set the servo pins and Buzzer pin
  Otto.initMATRIX(DIN, CS, CLK, Orientation);
  Otto.home();
  delay(50);

  
  // Beginne die serielle Kommunikation mit dem PC
  Serial.begin(9600);  // Standard-Serial für den Monitor
  // Beginne die serielle Kommunikation mit dem Bluetooth-Modul
  BTSerial.begin(9600);  // HC-05/HC-06 kommuniziert normalerweise mit 9600 Baud
  Serial.println("Verbindung hergestellt. Bluetooth ist bereit und serielle Verbindung aktiv!");
}

void loop() {
  // Wenn Daten vom Bluetooth-Modul empfangen werden
  if (BTSerial.available()) {
    char command = BTSerial.read();  // Lese den empfangenen Befehl
    Serial.print("Bluetooth-Befehl empfangen: ");
    Serial.println(command);
    processCommand(command);  // Verarbeite den Befehl
  }

  // Wenn Daten vom seriellen Monitor (PC über USB) empfangen werden
  if (Serial.available()) {
    char pcCommand = Serial.read();
    Serial.print("USB-Befehl empfangen: ");
    Serial.println(pcCommand);
    processCommand(pcCommand);  // Verarbeite den Befehl
  }
}

// Funktion zur Verarbeitung der Bewegungsbefehle
void processCommand(char command) {
  // Verarbeite den Befehl und steuere Otto entsprechend
  if (command == 'w') {
    Serial.println("Vorwärts!");
    Otto.walk(5, 1000, 1);
    // Funktion für Otto DIY vorwärts
  } else if (command == 's') {
    Serial.println("Rückwärts!");
    Otto.walk(5, 1000, -1); 
    // Funktion für Otto DIY rückwärts laufen
  } else if (command == 'a') {
    Serial.println("Links!");
    Otto.turn(15, 1000, 1);
    // Funktion für Otto DIY links drehen
  } else if (command == 'd') {
    Serial.println("Rechts!");
    Otto.turn(15, 1000, -1);
    // Funktion für Otto DIY rechts drehen
  }
}
