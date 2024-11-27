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
long Entfernung = 0;
unsigned long previousMillis = 0;
const long interval = 500; 

// Bluetooth Pins
#define BT_RX_PIN 10 
#define BT_TX_PIN 11 
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

// Modussteuerung
bool autoMode = true;

int melody[] = { /* Melodie-Daten */ };
int durations[] = { /* Dauer-Daten */ };

// Funktion zum Abspielen der Melodie
void playMelody() {
  int size = sizeof(durations) / sizeof(int);
  for (int note = 0; note < size; note++) {
    int duration = 1000 / durations[note];
    tone(BUZZER_PIN, melody[note], duration);
    delay(duration * 1.30);
    noTone(BUZZER_PIN);
  }
}

void processCommand(char command) {
  if (command == 'm') {
    autoMode = !autoMode;
    Serial.println(autoMode ? "Automatischer Modus aktiviert." : "Manueller Modus aktiviert.");
    return;
  }

  // Befehle zur manuellen Steuerung
  if (autoMode) return;  // Nur im manuellen Modus Befehle verarbeiten
  
  switch (command) {
    case 'w':
      Serial.println("Vorwärts!");
      Otto.walk(5, 1000, 1);
      break;
    case 's':
      Serial.println("Rückwärts!");
      Otto.walk(5, 1000, -1);
      break;
    case 'a':
      Serial.println("Links!");
      Otto.turn(5, 1000, 1);
      break;
    case 'd':
      Serial.println("Rechts!");
      Otto.turn(5, 1000, -1);
      break;
    case 'p':
      playMelody();
      break;
    case 't':
      Serial.println("Springen!");
      Otto.jump(1, 500);
      break;
    default:
      Serial.print("Unbekannter Befehl: ");
      Serial.println(command);
  }
}

void setup() {
  Otto.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true, BUZZER_PIN);
  Otto.initMATRIX(DIN, CS, CLK, Orientation);
  Otto.home();
  delay(50);

  pinMode(SENDEN, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
  BTSerial.begin(9600);

  Serial.println("Verbindung hergestellt. Bluetooth ist bereit und serielle Verbindung aktiv!");
}

void loop() {
  // Überprüfen, ob Daten von Bluetooth oder Serial kommen
  if (Serial.available()) {
    char command = Serial.read();
    processCommand(command);
  }

  if (BTSerial.available()) {
    char command = BTSerial.read();
    Serial.print("Bluetooth empfangen: ");
    Serial.println(command);
    processCommand(command);
  }

  // Automatischer Modus
  if (autoMode) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      digitalWrite(SENDEN, LOW);
      delay(5);
      digitalWrite(SENDEN, HIGH);
      delayMicroseconds(10);
      digitalWrite(SENDEN, LOW);

      long Zeit = pulseIn(ECHO, HIGH);
      Entfernung = (Zeit / 2) * 0.03432;

      if (Entfernung < 500) {
        Serial.print("Entfernung in cm: ");
        Serial.println(Entfernung);
      }

      if (Entfernung < 25) {
        Otto.turn(3, 800, -1);
        delay(500);
      } else {
        Otto.walk(3, 600, 1);
      }
    }
  }
}
