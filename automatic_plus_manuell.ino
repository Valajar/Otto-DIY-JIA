#include <Otto.h>
#include <pitches.h>
#include <SoftwareSerial.h>

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

// Ultraschallsensor Pins
#define SENDEN 8
#define ECHO 9
long Entfernung = 0;  // Entfernungsvariable
unsigned long previousMillis = 0;
const long interval = 500;  // alle 500 ms den Ultraschallsensor auslesen

// Bluetooth Pins
#define BT_RX_PIN 10  // RX Pin (Empfang)
#define BT_TX_PIN 11  // TX Pin (Senden)
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

// Modussteuerung
bool autoMode = true;  // Start im automatischen Modus

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
  // Weitere Melodie...


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

void setup() {
  Otto.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true, BUZZER_PIN); // Otto initialisieren
  Otto.initMATRIX(DIN, CS, CLK, Orientation);
  Otto.home();
  delay(50);

  pinMode(SENDEN, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);  // Serielle Kommunikation starten
  BTSerial.begin(9600);  // Bluetooth Kommunikation starten

  Serial.println("Verbindung hergestellt. Bluetooth ist bereit und serielle Verbindung aktiv!");
}

void loop() {
  // Überprüfen, ob serielle Daten vom PC oder Bluetooth-Modul empfangen wurden
  if (Serial.available()) {
    char command = Serial.read();  // Befehl vom seriellen Monitor oder Bluetooth lesen
    processCommand(command);  // Befehl verarbeiten
  }

  if (BTSerial.available()) {
    char command = BTSerial.read();  // Befehl vom Bluetooth lesen
    processCommand(command);  // Befehl verarbeiten
  }

  // Automatischer Modus: Ultraschallsensor und Bewegung
  if (autoMode) {
    unsigned long currentMillis = millis();  // Zeitstempel der aktuellen Zeit

    // Nur alle 500ms den Ultraschallsensor auslesen
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;  // Aktualisiere den Zeitstempel

      // Ultraschallsignal senden
      digitalWrite(SENDEN, LOW);
      delay(5);
      digitalWrite(SENDEN, HIGH);
      delayMicroseconds(10);
      digitalWrite(SENDEN, LOW);

      // Zeit messen, bis Signal zurückkommt
      long Zeit = pulseIn(ECHO, HIGH);

      // Entfernung berechnen
      Entfernung = (Zeit / 2) * 0.03432;

      // Entfernung anzeigen
      if (Entfernung < 500) {
        Serial.print("Entfernung in cm: ");
        Serial.println(Entfernung);
      }

      // Wenn die Entfernung unter 25 cm liegt, dreht Otto nach rechts
      if (Entfernung < 25) {
        Otto.turn(3, 800, -1);  // Dreht nach rechts
        delay(500);
        Otto.jump(1, 1000);
      } else {
        Otto.walk(3, 600, 1);  // Otto geht vorwärts
        Otto._tone(10, 3, 1);
        Otto.bendTones(100, 200, 1.04, 10, 10);
      }
    }
  }
}

// Funktion zur Verarbeitung der Bewegungsbefehle
void processCommand(char command) {
  // Wenn 'm' empfangen wird, Modus umschalten
  if (command == 'm') {
    autoMode = !autoMode;
    if (autoMode) {
      Serial.println("Automatischer Modus aktiviert.");
    } else {
      Serial.println("Manueller Modus aktiviert.");
    }
  }
  if (BTSerial.available()) {
    char command = BTSerial.read();
    Serial.print("Bluetooth empfangen: ");
    Serial.println(command);
}
if (BTSerial.available()) {
    char command = BTSerial.read();
    Serial.print("Bluetooth empfangen: ");
    Serial.println(command);
}



  // Befehle zur manuellen Steuerung
  if (command == 'w') {
    Serial.println("Vorwärts!");
    Otto.walk(5, 1000, 1);
  } else if (command == 's') {
    Serial.println("Rückwärts!");
    Otto.walk(5, 1000, -1); 
  } else if (command == 'a') {
    Serial.println("Links!");
    Otto.turn(15, 1000, 1);
  } else if (command == 'd') {
    Serial.println("Rechts!");
    Otto.turn(15, 1000, -1);
  } else if (command == 'p') {
    playMelody();  // Melodie abspielen
  }
  else if (command == 't') {
    Otto.jump(1, 500);
  }
}

// Funktion zum Abspielen der Melodie
void playMelody() {
  int size = sizeof(durations) / sizeof(int);
  for (int note = 0; note < size; note++) {
    int duration = 1000 / durations[note];
    tone(BUZZER_PIN, melody[note], duration);
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN);
  }
}
