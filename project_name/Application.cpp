#include <Arduino.h>

class UltrasonicSensor {
private:
    int trigPin;
    int echoPin;

public:
    UltrasonicSensor(int trig, int echo) : trigPin(trig), echoPin(echo) {
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }

    float measureDistance() {
        // Envoi impulsion 10 µs
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        // Mesure du retour
        long duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30 ms
        if (duration == 0) return -1;
        return duration * 0.0343 / 2.0; // Conversion en cm
    }
};

class Buzzer {
private:
    int pin;

public:
    Buzzer(int p) : pin(p) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    void on() {
        digitalWrite(pin, HIGH);
    }

    void off() {
        digitalWrite(pin, LOW);
    }
};

class AntiCollisionSystem {
private:
    UltrasonicSensor& sensor;
    Buzzer& buzzer;
    float lastDistance;
    unsigned long lastTime;

    // Seuils
    const float distMin = 10.0;   // cm
    const float deltaMax = -10.0; // cm/s
    const unsigned long interval = 100; // ms

public:
    AntiCollisionSystem(UltrasonicSensor& s, Buzzer& b)
        : sensor(s), buzzer(b), lastDistance(0), lastTime(0) {}

    void update() {
        unsigned long currentTime = millis();
        if (currentTime - lastTime >= interval) {
            float distance = sensor.measureDistance();
            if (distance > 0) {
                float deltaT = (currentTime - lastTime) / 1000.0;
                float vitesse = (distance - lastDistance) / deltaT; // cm/s

                Serial.print("Distance: ");
                Serial.print(distance);
                Serial.print(" cm | Vitesse: ");
                Serial.print(vitesse);
                Serial.println(" cm/s");

                // Condition de danger
                if (distance < distMin || vitesse < deltaMax) {
                    buzzer.on();
                } else {
                    buzzer.off();
                }

                lastDistance = distance;
            }
            lastTime = currentTime;
        }
    }
};

// --- Instanciation des objets ---
UltrasonicSensor sensor(D5, D6);
Buzzer buzzer(D7);
AntiCollisionSystem system(sensor, buzzer);

void setup() {
    Serial.begin(115200);
    Serial.println("Système anti-collision (orienté objet) prêt !");
}

void loop() {
    system.update();
}
