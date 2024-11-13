#include <Otto.h>
Otto Otto;  //This is Otto!

#define LeftLeg 2 
#define RightLeg 3
#define LeftFoot 4 
#define RightFoot 5 
#define Buzzer  13 
#define DIN A3 // Data In pin
#define CS A2  // Chip Select pin
#define CLK A1 // Clock pin
#define Orientation 1 // 8x8 LED Matrix orientation  Top = 1, Bottom = 2, Left = 3, Right = 4 

int SENDEN = 8;

// Pin für das vom Objekt reflektierte Signal
int ECHO = 9;

// Variable für die Speicherung der Entfernung
long Entfernung = 0;

// Variablen für die nicht-blockierende Zeitsteuerung
unsigned long previousMillis = 0;
const long interval = 500;  // alle 500 ms den Ultraschallsensor auslesen

///////////////////////////////////////////////////////////////////
//-- Setup ------------------------------------------------------//
///////////////////////////////////////////////////////////////////
void setup(){
  Otto.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true, Buzzer); //Set the servo pins and Buzzer pin
  Otto.initMATRIX(DIN, CS, CLK, Orientation);
  Otto.sing(S_connection); // Otto wake up!
  Otto.home();
  delay(50);
  Otto.playGesture(OttoHappy);
  
  pinMode(SENDEN, OUTPUT);
  pinMode(ECHO, INPUT);

  // Seriellen Monitor starten
  Serial.begin(9600);
}

///////////////////////////////////////////////////////////////////
//-- Principal Loop ---------------------------------------------//
///////////////////////////////////////////////////////////////////

void loop() {
  unsigned long currentMillis = millis();  // Zeitstempel der aktuellen Zeit

  // Nur alle 500ms den Ultraschallsensor auslesen
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Aktualisiere den Zeitstempel

    // Sender kurz ausschalten um Störungen des Signals zu vermeiden
    digitalWrite(SENDEN, LOW);
    delay(5);

    // Signal für 10 Mikrossekunden senden, danach wieder ausschalten
    digitalWrite(SENDEN, HIGH);
    delayMicroseconds(10);
    digitalWrite(SENDEN, LOW);

    // pulseIn -> Zeit messen, bis Signal zurückkommt
    long Zeit = pulseIn(ECHO, HIGH);

    // Entfernung in cm berechnen
    Entfernung = (Zeit / 2) * 0.03432;

    // nur Entfernungen < 100 anzeigen
    if (Entfernung < 500) {
      Serial.print("Entfernung in cm: ");
      Serial.println(Entfernung);
    }
  }

  // Otto geht, dreht und macht Geräusche
  Otto.walk(3, 1000, 1);  // 2 Schritte, "TIME". Höherer Wert = langsamer
  Otto.turn(2, 1000, 1);   // 3 Schritte nach links drehen
  Otto._tone(10, 3, 1);
  Otto.bendTones(100, 200, 1.04, 10, 10);
  Otto.home();
  
  delay(100);  // Kleine Pause zwischen den Bewegungen

  // Wenn die Entfernung kleiner als 10 cm ist, dreht Otto nach rechts
  while (Entfernung < 10) {
    Otto.turn(1, 1000, -1);  // 1 Schritt nach rechts drehen

    // Ultraschallsensor erneut abfragen
    digitalWrite(SENDEN, HIGH);
    delayMicroseconds(10);
    digitalWrite(SENDEN, LOW);

    long Zeit = pulseIn(ECHO, HIGH);
    Entfernung = (Zeit / 2) * 0.03432;
    Serial.print("Entfernung in cm: ");
    Serial.println(Entfernung);
    delay(500);
  }
}
