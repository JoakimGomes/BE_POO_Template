#include "Application.h"
#include <Arduino.h>

#define BUZZER_PIN D5

Application::Application()
{
  SensorSon = nullptr;
  buzzerPin = BUZZER_PIN;
}
  
Application::~Application()
{
  if (SensorSon != nullptr)
    delete SensorSon;
}  

void Application::init(void)
{
  Serial.begin(115200);
  Serial.println("Initialisation du système...");
  SensorSon = new UltrasonicSensor(D7);  // pin D7 pour ton capteur Grove

  delay(1000);

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // buzzer éteint au démarrage
}

void Application::run(void)
{
  if (SensorSon == nullptr)
    return;

  float distance = SensorSon->measureDistance();
  if (distance != -1) {
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
    // Si objet proche, bipper plus vite
    if (distance < 100) { // sous 1 mètre
      // mapping de la distance vers la fréquence des bips
      int delayTime = map((int)distance, 5, 100, 50, 800); // 5cm→50ms, 100cm→800ms
      int beepDuration = 40; // durée du bip

      // Pour buzzer actif :
      digitalWrite(buzzerPin, HIGH);
      delay(beepDuration);
      digitalWrite(buzzerPin, LOW);
      delay(delayTime);

      // Pour buzzer passif (décommente si besoin) :
      // tone(buzzerPin, 2000);
      // delay(beepDuration);
      // noTone(buzzerPin);
      // delay(delayTime);
    } else {
      digitalWrite(buzzerPin, LOW); // pas de bip si loin
    }
  } else {
    Serial.println("No echo received");
    digitalWrite(buzzerPin, LOW);
  }

  delay(100);
}