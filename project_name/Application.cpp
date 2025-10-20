#include "Application.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define BUZZER_PIN D5
#define LED_PIN    D6
#define NUM_LEDS   10

// ==== Config WiFi ====
const char* ssid = "wifierwan";
const char* password = "erwan58";

// ==== Serveur web ====
ESP8266WebServer server(80);

// ==== Bandeau LED ====
Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ==== Variables globales ====
float lastDistance = 0;
unsigned long lastMeasureTime = 0;
unsigned long lastBeepTime = 0;
bool buzzerOn = false;

Application::Application() {
  SensorSon = nullptr;
  buzzerPin = BUZZER_PIN;
}
  
Application::~Application() {
  if (SensorSon != nullptr)
    delete SensorSon;
}  

void handleDistanceRequest(); // déclaration anticipée

void Application::init(void) {
  Serial.begin(115200);
  Serial.println("Initialisation du système...");
  SensorSon = new UltrasonicSensor(D7);  // pin D7 pour ton capteur Grove

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  leds.begin();
  leds.show();

  // === Création du Wi-Fi AP ===
  const char* ap_ssid = "access_point_wan";
  const char* ap_password = "12345678";
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point started. Connect to ");
  Serial.println(ap_ssid);
  Serial.print("IP address: ");
  Serial.println(IP);

  // === Routes HTTP ===
  server.on("/distance", handleDistanceRequest);

  // === Page web interactive ===
  server.on("/", []() {
    String page = R"(
    <html>
      <head>
        <meta charset="UTF-8">
        <title>Distance ESP8266</title>
        <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate">
        <meta http-equiv="Pragma" content="no-cache">
        <meta http-equiv="Expires" content="0">
        <style>
          body { font-family: Arial; text-align: center; background: #111; color: #0f0; }
          h1 { font-size: 3em; margin-top: 2em; }
        </style>
        <script>
          async function updateDistance() {
            try {
              const url = '/distance?t=' + new Date().getTime();
              const res = await fetch('/distance');
              const data = await res.json();
              document.getElementById('value').textContent = data.distance.toFixed(2) + ' cm';
            } catch(e) {
              document.getElementById('value').textContent = 'Erreur';
            }
          }
          setInterval(updateDistance, 1000); // ✅ refresh toutes les 1s
          window.onload = updateDistance;
        </script>
      </head>
      <body>
        <h1>Distance: <span id="value">---</span></h1>
      </body>
    </html>
    )";
    server.send(200, "text/html", page);
  });

  server.begin();
  Serial.println("HTTP server started.");
}

// === Boucle principale non bloquante ===
void Application::run(void) {
  server.handleClient();
  unsigned long now = millis();

  // ==== Mesure du capteur toutes les 200 ms ====
  if (now - lastMeasureTime >= 100) {
    lastMeasureTime = now;
    float distance = SensorSon->measureDistance();
    if (distance > 0) {
      lastDistance = distance;

      // === LEDs ===
      int ledsToLight = map((int)distance, 5, 100, NUM_LEDS, 0);
      ledsToLight = constrain(ledsToLight, 0, NUM_LEDS);
      for (int i = 0; i < NUM_LEDS; i++) {
        if (i < ledsToLight) {
          uint8_t r = map(i, 0, NUM_LEDS - 1, 0, 255);
          uint8_t g = map(i, 0, NUM_LEDS - 1, 255, 0);
          leds.setPixelColor(i, leds.Color(r, g, 0));
        } else {
          leds.setPixelColor(i, 0, 0, 0);
        }
      }
      leds.show();
    }
  }

  // ==== Gestion buzzer toutes les X ms ====
  if (lastDistance < 100 && lastDistance > 0) {
    int interval = map((int)lastDistance, 5, 100, 100, 1000); // plus long
    if (now - lastBeepTime >= interval) {
      lastBeepTime = now;
      buzzerOn = !buzzerOn;
      digitalWrite(buzzerPin, buzzerOn);
    }
  } else {
    digitalWrite(buzzerPin, LOW);
    buzzerOn = false;
  }
}

// === Réponse HTTP pour la distance ===
void handleDistanceRequest() {
  String response = "{ \"distance\": ";
  response += String(lastDistance, 2);
  response += " }";
  server.send(200, "application/json", response);
}
