/*
Vor dem Hochladen:
1. W-Lan SSID und Passwort in Zeile 20 und 21 eintragen
2. Accout unter "https://www.meteomatics.com/en/api/getting-started/" erstellen
3. Benutzername und Passwort des Accounts in Zeile 65 in die setAuthorization eitragen
4. W-Lan SSID und Passwort in Zeile 20 und 21 eintragen
5. In die http in Zeile 60 die Zielkoordinaten wie angegeben einfügen (!! keine Leerzeichen)
6. In C3-Mini hochladen
*/


#include <WiFi.h> // Für ESP32 (für ESP8266: #include <ESP8266WiFi.h>)
#include <HTTPClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WLAN-Daten
const char* ssid = "your SSID";  // WLAN-Name
const char* password = "your password"; // WLAN-Passwort

// OLED-Display-Einstellungen
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  Serial.begin(115200);

  // WLAN verbinden
  Serial.println("Mit WLAN verbinden...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nVerbunden mit WLAN");
  Serial.println("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  // OLED-Display initialisieren
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // 0x3C ist die Standard-I2C-Adresse
    Serial.println("Fehler beim Initialisieren des OLED-Displays");
    while (true); // Halte an, falls das Display nicht funktioniert
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // API-URL mit allen Parametern
    String apiUrl = "http://api.meteomatics.com/2024-11-30T00:00:00Z/t_2m:C/51.538300,9.933812/csv";
    //Koordinaten an Zielregion anpassen Form: 51.538300, 9.933812

    http.begin(apiUrl); // Verbindung zur API herstellen
    // Authentifizierung (Benutzername und Passwort)
    http.setAuthorization("Benutzername", "Passwort");

    int httpResponseCode = http.GET(); // GET-Anfrage senden

    if (httpResponseCode > 0) {
      String response = http.getString(); // Antwort der API speichern
      String temperature = extractTemperature(response); // Temperatur extrahieren

      // Temperatur auf dem OLED-Display anzeigen
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(40, 10);
      display.println("Temp:");
      display.setCursor(40, 20);
      if (temperature != "Fehler") {
        display.setTextSize(1);
        display.print(temperature);
        display.println(" C");
      } else {
        display.println("Fehler");
      }
      display.display();
      
      // Serielle Ausgabe der Temperatur
      Serial.println("Temperatur:");
      Serial.println(temperature);
    } else {
      Serial.print("Fehler bei der HTTP-Anfrage, Code: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Verbindung schließen
  } else {
    Serial.println("Nicht mit WLAN verbunden.");
  }

  delay(600000); // 10 Minuten warten, bevor erneut abgefragt wird
}

String extractTemperature(String response) {
  int lineBreak = response.indexOf('\n'); // Erstes Zeilenende finden
  if (lineBreak != -1) {
    String secondLine = response.substring(lineBreak + 1); // Zweite Zeile extrahieren
    int semicolon = secondLine.indexOf(';'); // Semikolon-Position finden
    if (semicolon != -1) {
      return secondLine.substring(semicolon + 1); // Temperaturwert extrahieren
    }
  }
  return "Fehler"; // Falls keine zweite Zeile oder kein Semikolon gefunden wird
}
