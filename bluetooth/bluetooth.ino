#include <pitches.h>
#include <SoftwareSerial.h>
#include <Otto.h>
Otto Otto;

#define LeftLeg 2 
#define RightLeg 3
#define LeftFoot 4 
#define RightFoot 5 
#define BUZZER_PIN 13 
#define DIN A3 // Data In pin
#define CS A2  // Chip Select pin
#define CLK A1 // Clock pin
#define Orientation 1 // 8x8 LED Matrix orientation  Top = 1, Bottom = 2, Left = 3, Right = 4 

int melody[] = {
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, REST,
  NOTE_A4, NOTE_G4, NOTE_A4, REST,
  
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, REST,
  NOTE_A4, NOTE_G4, NOTE_A4, REST,
  
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, REST,
  NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, REST,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, REST,
  
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_D5, NOTE_E5, NOTE_A4, REST,
  NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, REST,
  NOTE_C5, NOTE_A4, NOTE_B4, REST,
  
  NOTE_A4, NOTE_A4,
  //Repeat of first part
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, REST,
  NOTE_A4, NOTE_G4, NOTE_A4, REST,
  
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, REST,
  NOTE_A4, NOTE_G4, NOTE_A4, REST,
  
  NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, REST,
  NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, REST,
  NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, REST,
  NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, REST,
  
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, REST,
  NOTE_D5, NOTE_E5, NOTE_A4, REST,
  NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, REST,
  NOTE_C5, NOTE_A4, NOTE_B4, REST,
  //End of Repeat
  
  NOTE_E5, REST, REST, NOTE_F5, REST, REST,
  NOTE_E5, NOTE_E5, REST, NOTE_G5, REST, NOTE_E5, NOTE_D5, REST, REST,
  NOTE_D5, REST, REST, NOTE_C5, REST, REST,
  NOTE_B4, NOTE_C5, REST, NOTE_B4, REST, NOTE_A4,
  
  NOTE_E5, REST, REST, NOTE_F5, REST, REST,
  NOTE_E5, NOTE_E5, REST, NOTE_G5, REST, NOTE_E5, NOTE_D5, REST, REST,
  NOTE_D5, REST, REST, NOTE_C5, REST, REST,
  NOTE_B4, NOTE_C5, REST, NOTE_B4, REST, NOTE_A4
};

int durations[] = {
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8,
  
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8,
  
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 8, 4, 8,
  
  8, 8, 4, 8, 8,
  4, 8, 4, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 4,
  
  4, 8,
  //Repeat of First Part
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8,
  
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8,
  
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 8, 8,
  8, 8, 8, 4, 8,
  
  8, 8, 4, 8, 8,
  4, 8, 4, 8,
  8, 8, 4, 8, 8,
  8, 8, 4, 4,
  //End of Repeat
  
  4, 8, 4, 4, 8, 4,
  8, 8, 8, 8, 8, 8, 8, 8, 4,
  4, 8, 4, 4, 8, 4,
  8, 8, 8, 8, 8, 2,
  
  4, 8, 4, 4, 8, 4,
  8, 8, 8, 8, 8, 8, 8, 8, 4,
  4, 8, 4, 4, 8, 4,
  8, 8, 8, 8, 8, 2
};


// Definiere die Pins für SoftwareSerial
#define BT_RX_PIN 10  // RX Pin (Empfang)
#define BT_TX_PIN 11  // TX Pin (Senden)

// Erstelle eine SoftwareSerial-Instanz für das Bluetooth-Modul
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

void setup() {
  Otto.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true, BUZZER_PIN); //Set the servo pins and Buzzer pin
  Otto.initMATRIX(DIN, CS, CLK, Orientation);
  Otto.home();
  delay(50);

  pinMode(BUZZER_PIN, OUTPUT);
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
  } else if (command == 'p') {
    int size = sizeof(durations) / sizeof(int);
  for (int note = 0; note < size; note++) {
    //to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int duration = 1000 / durations[note];
    tone(BUZZER_PIN, melody[note], duration);

    //to distinguish the notes, set a minimum time between them.
    //the note's duration + 30% seems to work well:
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);

    //stop the tone playing:
    noTone(BUZZER_PIN);
  }
  }
  // Weitere Befehle können hier hinzugefügt werden...
}
