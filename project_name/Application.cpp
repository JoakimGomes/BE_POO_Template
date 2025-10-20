#include "Application.hpp"
#include "UltraSonicSensor.hpp"
#include <Arduino.h>


Application::Application()
{
  // Code
  ; 
}
  
Application::~Application()
{
  // Code
  ;
}  

void Application::init(void)
{
    UltrasonicSensor* SensorSon;
    SensorSon = new UltrasonicSensor(7);  // Initialisation du capteur ultrason sur la pin 7
}


void Application::run(void)
{
    UltrasonicSensor* SensorSon;
    float distance = SensorSon->measureDistance();
    if (distance != -1) {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");
    } else {
        Serial.println("No echo received");
    }
    delay(100); // Attendre 1 seconde avant la prochaine mesure
}