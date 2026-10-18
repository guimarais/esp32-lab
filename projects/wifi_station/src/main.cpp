#include <Arduino.h>
#include <WiFi.h>

#include "secrets.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.printf("\nConnecting to %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String currentLine = "";
  while (client.connected()) {
    if (!client.available()) continue;
    char c = client.read();
    if (c == '\n') {
      if (currentLine.length() == 0) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println();
        client.print("Click <a href=\"/H\">here</a> to turn the LED on.<br>");
        client.print("Click <a href=\"/L\">here</a> to turn the LED off.<br>");
        client.println();
        break;
      }
      currentLine = "";
    } else if (c != '\r') {
      currentLine += c;
    }

    if (currentLine.endsWith("GET /H"))
    {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("LED ON");
    }
    if (currentLine.endsWith("GET /L"))
    {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("LED OFF");
    }
  }
  client.stop();
}
