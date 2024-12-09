#include <WiFi.h> // Für ESP32
#include <HTTPClient.h>
#include <SPIFFS.h>      // Für die persistente Speicherung
#include <WiFiManager.h> // Für die Konfiguration per Webportal
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h> // Zum Speichern der Konfiguration

// OLED-Display-Einstellungen
#define OLED_RESET 0 // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

// Globale Variablen für API-Parameter
char apiUsername[40] = "";  // Speicher für den API-Benutzernamen
char apiKey[40] = "";       // Speicher für den API-Key
char cityCoords[40] = "";   // Speicher für die Koordinaten

void setup() {
  Serial.begin(115200);

  // SPIFFS initialisieren
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS konnte nicht gestartet werden.");
    return;
  }

  // Bestehende Konfigurationsdaten laden (falls vorhanden)
  loadConfig();

  // OLED-Display initialisieren
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C ist die Standard-I2C-Adresse
    Serial.println("Fehler beim Initialisieren des OLED-Displays");
    while (true); // Halte an, falls das Display nicht funktioniert
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("WLAN konfigurieren...");
  display.display();

  // WiFiManager initialisieren
  WiFiManager wifiManager;

  // Benutzerdefinierte Felder hinzufügen
  WiFiManagerParameter customApiUsername("apiUser", "API Username", apiUsername, 40);
  WiFiManagerParameter customApiKey("api", "API Key", apiKey, 40);
  WiFiManagerParameter customCoords("coords", "City Coordinates (lat,lon)", cityCoords, 40);
  wifiManager.addParameter(&customApiUsername);
  wifiManager.addParameter(&customApiKey);
  wifiManager.addParameter(&customCoords);

  // WLAN konfigurieren oder Verbindung herstellen
  if (!wifiManager.autoConnect("ConfigPortal", "password")) {
    Serial.println("Verbindung konnte nicht hergestellt werden.");
    delay(3000);
    ESP.restart();
  }

  // Speichern der eingegebenen Werte
  strcpy(apiUsername, customApiUsername.getValue());
  strcpy(apiKey, customApiKey.getValue());
  strcpy(cityCoords, customCoords.getValue());

  // Konfigurationsdaten speichern
  saveConfig();

  // Erfolgreich verbunden
  Serial.println("Verbunden mit WLAN!");
  Serial.print("API Username: ");
  Serial.println(apiUsername);
  Serial.print("API Key: ");
  Serial.println(apiKey);
  Serial.print("Koordinaten: ");
  Serial.println(cityCoords);

  // Anzeigen der IP-Adresse auf dem OLED-Display
  display.clearDisplay();
  display.setCursor(40, 10);
  display.println("Verbunden mit:");
  display.println(WiFi.SSID());
  display.println("IP:");
  display.println(WiFi.localIP().toString());
  display.display();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Dynamische API-URL
    String apiUrl = "http://api.meteomatics.com/now/t_2m:C/" + String(cityCoords) + "/csv";

    http.begin(apiUrl); // Verbindung zur API herstellen
    http.setAuthorization(apiUsername, apiKey); // Dynamischer Benutzername und Key

    int httpResponseCode = http.GET(); // GET-Anfrage senden

    if (httpResponseCode > 0) {
      String response = http.getString(); // Antwort der API speichern
      String temperature = extractTemperature(response); // Temperatur extrahieren

      // Temperatur auf dem OLED-Display anzeigen
      display.clearDisplay();
      display.setCursor(40, 10);
      display.println("Temp:");
      display.setCursor(40, 20);
      if (temperature != "Fehler") {
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

// Temperatur aus der API-Antwort extrahieren
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

// Speichern der Konfigurationsdaten in SPIFFS
void saveConfig() {
  File configFile = SPIFFS.open("/config.json", FILE_WRITE);
  if (!configFile) {
    Serial.println("Fehler beim Öffnen der Konfigurationsdatei.");
    return;
  }

  StaticJsonDocument<256> doc;
  doc["apiUser"] = apiUsername;
  doc["api"] = apiKey;
  doc["coords"] = cityCoords;

  if (serializeJson(doc, configFile) == 0) {
    Serial.println("Fehler beim Speichern der JSON-Daten.");
  }
  configFile.close();
}

// Laden der Konfigurationsdaten aus SPIFFS
void loadConfig() {
  File configFile = SPIFFS.open("/config.json", FILE_READ);
  if (!configFile) {
    Serial.println("Keine Konfigurationsdatei gefunden.");
    return;
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Serial.println("Fehler beim Laden der JSON-Daten.");
    return;
  }

  strcpy(apiUsername, doc["apiUser"]);
  strcpy(apiKey, doc["api"]);
  strcpy(cityCoords, doc["coords"]);

  configFile.close();
}

