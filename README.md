# Wi-Fi Configuration and Weather Data Retrieval with ESP32

This project allows an ESP32 board to connect to a Wi-Fi network and fetch weather data from an API. The configuration is done through a web portal, where users can input their API credentials and city coordinates. The retrieved temperature data is displayed on an OLED screen.

## Getting Started

This project enables you to configure the ESP32 via a web portal to interact with a weather API. The API requests return the current temperature, which is displayed on an OLED screen.

### Prerequisites

- **ESP32** development board
- **Arduino IDE** (with necessary libraries installed)
- **OLED display** (e.g., SSD1306, 128x64 pixels)
- **Wi-Fi network**
- Access to a weather API (e.g., Meteomatics API) to retrieve weather data

## Configuration

### 1. **Prepare the ESP32**
   - Ensure the Arduino IDE is set up correctly to program the ESP32.
   - Install the required libraries:
     - `WiFi.h`
     - `HTTPClient.h`
     - `SPIFFS.h`
     - `WiFiManager.h`
     - `Adafruit_GFX.h`
     - `Adafruit_SSD1306.h`
     - `ArduinoJson.h`

### 2. **Create a Free Account on Meteomatics API**
   - Visit [http://api.meteomatics.com](http://api.meteomatics.com).
   - Create a free account to access the API.
   - After registration, you will receive an **API Username** and **API Key** via email. These credentials are necessary to authenticate your requests to the weather API.

### 3. **Connect to the Configuration Wi-Fi Network**
   - Power on the ESP32, and it will automatically create a Wi-Fi network called "ConfigPortal."
   - Connect to this network (default password: `password`).
   - Once connected, a web portal will open in your browser where you can input your API credentials and city coordinates.

### 4. **Enter Configuration Details**
   - In the web portal, enter the following details:
     - **Wi-Fi Network Name (SSID)**
     - **Wi-Fi Network Password**
     - **API Username**
     - **API Key**
     - **City Coordinates** (in "latitude,longitude" format)
   - Once the details are entered, the ESP32 will save the configuration and connect to your Wi-Fi network.

### 5. **Save Configuration**
   - After entering the required details, the configuration will be saved, and the ESP32 will reconnect to your Wi-Fi network.
   - The ESP32 will then display its IP address on the OLED screen and start fetching temperature data from the API every 10 minutes.

### 6. **Weather Data Retrieval and Display**
   - Every 10 minutes, the ESP32 will make an HTTP request to the weather API and display the temperature on the OLED screen.
   - If there is an issue with the API connection or the Wi-Fi, an error will be shown on the display.

## Code Explanation

### SPIFFS
The ESP32 stores configuration data (API username, API key, and city coordinates) in an internal file system called SPIFFS (SPI Flash File System). This ensures that the settings are preserved even after a reboot of the ESP32.

### WiFiManager
WiFiManager ensures that the ESP32 creates a web portal that allows users to configure their Wi-Fi credentials and API details without needing to manually modify the code.

### OLED Display
The OLED display shows the device's connection status, such as the connected Wi-Fi SSID and IP address, and displays the current temperature fetched from the API.

## Files

- **config.json**: This file stores the API configuration and coordinates entered by the user in the SPIFFS file system.
- **Wi-Fi Settings**: During startup, the ESP32 will configure the Wi-Fi connection via the web portal.

## Example Output on OLED Display
- **Successful Connection**: The display shows the Wi-Fi SSID and IP address.
- **Temperature Display**: The temperature (e.g., "Temp: 22.5 C") is updated every 10 minutes.

## Possible Improvements

- **Error Handling**: The implementation could be expanded with better error handling for failed API responses or network issues.
- **Additional APIs**: More weather data or different APIs could be integrated to show more information (e.g., humidity, weather forecast).
- **User Interface**: The web portal could be enhanced with more features and a more user-friendly interface.
