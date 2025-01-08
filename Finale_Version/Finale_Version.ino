#include <Otto.h>
#include <SoftwareSerial.h>

// Otto Initialisierung
Otto Otto;

#define LeftLeg 2 
#define RightLeg 3
#define LeftFoot 4 
#define RightFoot 5 
#define BUZZER_PIN 13 
#define DIN A3 // Data In pin
#define CS A2  // Chip Select pin
#define CLK A1 // Clock pin
#define Orientation 1

// Ultraschallsensor Pins
#define SENDEN 8
#define ECHO 9
long Entfernung = 0;

// Bluetooth Pins
#define BT_RX_PIN 10 
#define BT_TX_PIN 11 
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

// Timing-Variablen
unsigned long lastWalkTime = 0;
unsigned long lastDanceTime = 0;
unsigned long lastDistanceCheck = 0;
unsigned long danceDelay = 0;  // Zufällige Tanzverzögerung
const unsigned long walkInterval = 50;
const unsigned long distanceInterval = 500;

// Zufälliger Modus
bool autoMode = true;

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

  // Startzeit setzen
  lastWalkTime = millis();
  lastDanceTime = millis();
  danceDelay = random(10000, 20000); // 10-20 Sekunden für den nächsten Tanz

  Serial.println("Otto ist bereit!");
}

void loop() {
  unsigned long currentMillis = millis();

  // Automodus aktiv
  if (autoMode) {
    // 1. Abstand prüfen (alle 500 ms)
    if (currentMillis - lastDistanceCheck >= distanceInterval) {
      lastDistanceCheck = currentMillis;

      Entfernung = measureDistance();
      if (Entfernung < 25) {
        Otto.turn(3, 800, -1);  // Dreht sich bei Hindernis
        Serial.println("Otto Dreht sich im Kreis ");
        delay(500);
      }
    }

    // 2. Otto läuft (alle 50 ms)
    if (currentMillis - lastWalkTime >= walkInterval) {
      lastWalkTime = currentMillis;
      Otto.walk(3, 600, 1);  // Gehen
    }

    // 3. Otto tanzt zu einer zufälligen Zeit
    if (currentMillis - lastDanceTime >= danceDelay) {
      lastDanceTime = currentMillis;
      danceDelay = random(3000, 10000); // Neues zufälliges Intervall setzen
      Otto.walk(2, 1000, 1);   // Zwei Schritte vorwärts
      Otto.walk(2, 1000, -1);  // Zwei Schritte rückwärts
      Otto.turn(3, 800, 1);    // Dreht sich nach links
      Otto.turn(3, 800, -1);   // Dreht sich nach rechts

      Otto.swing(2, 1000, 20); // Zweimal seitlich schwingen
      Otto.bend(1, 500, 20);   // Einmal bücken
      Serial.println("Otto tanzt! ");
    }
  }
}

// Funktion zum Messen der Entfernung
long measureDistance() {
  digitalWrite(SENDEN, LOW);
  delay(5);
  digitalWrite(SENDEN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SENDEN, LOW);

  long duration = pulseIn(ECHO, HIGH);
  return (duration / 2) * 0.03432; // Entfernung in cm
}

// Funktion zur Verarbeitung von Befehlen
void processCommand(char command) {
  if (command == 'm') { //sobald wir den command "m" eingeben wechselt otto von dem Automatischen Modus in den Manuellen modus, woraufhin er ohne befehle nicht mehr läuft. Beim erneuten betätigen von "m" wechselt otto zurück in den Automatischen Modus
    autoMode = !autoMode;
    Serial.println(autoMode ? "Automatischer Modus aktiviert." : "Manueller Modus aktiviert.");
    return;
  }

  if (autoMode) return; // Keine manuellen Befehle im Automodus
/*
Das sind die Tastaturbefehle, mit denen man per Kabelverbindung Otto Steuern kann. 
 */
  switch (command) { //hier werden jetzt alle befehle die wir im Seriellen monitor eingeben können aufgelistet. Es funkionieren die Befehle W;A;S;D;T sollte eine andere taste eingegeben werden, wird er als nicht funktionierender Befehl erkannt
                      //zudem können mehrere Befehle aufgereit werden, die nach dem Bestätigen mit enter nacheinander ausgeführt werden
    case 'w':
      Otto.walk(5, 1000, 1);
      break;
    case 's':
      Otto.walk(5, 1000, -1);
      break;
    case 'a':
      Otto.turn(5, 1000, 1);
      break;
    case 'd':
      Otto.turn(5, 1000, -1);
      break;
    case 't':
      Otto.jump(1, 500);
      break;
    default:
      Serial.println("Unbekannter Befehl!");
  }
}
