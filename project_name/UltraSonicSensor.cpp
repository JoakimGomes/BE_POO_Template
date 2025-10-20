#include "UltrasonicSensor.hpp"
#include <Arduino.h>

UltrasonicSensor::UltrasonicSensor(int pin) {
  signalPin = pin;
  pinMode(signalPin, OUTPUT);
}

float UltrasonicSensor::measureDistance() {
  long duration;
  float distance;

  // Étape 1 : envoyer un trigger court
  pinMode(signalPin, OUTPUT);
  digitalWrite(signalPin, LOW);
  delayMicroseconds(2);
  digitalWrite(signalPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(signalPin, LOW);

  // Étape 2 : écouter le retour (écho)
  pinMode(signalPin, INPUT);
  duration = pulseIn(signalPin, HIGH, 30000UL); // timeout 30 ms

  // Étape 3 : calculer la distance
  if (duration == 0) {
    return -1; // pas d’écho reçu
  }
  distance = (duration * 0.0343) / 2.0; // en cm
  return distance;
}