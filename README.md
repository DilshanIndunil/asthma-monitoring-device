# Pulse Sensor with Firebase on ESP8266/ESP32

This project demonstrates how to connect an ESP8266/ESP32 to a pulse sensor and send the pulse readings to Firebase Realtime Database. The sensor readings are mapped to a specific range and periodically uploaded to the Firebase database.

## Hardware Requirements

- ESP8266 or ESP32 board
- Pulse Sensor
- Jumper wires
- Breadboard (optional)

## Software Requirements

- Arduino IDE
- ESP8266 or ESP32 board support installed in Arduino IDE
- Firebase ESP Client library for Arduino

## Circuit Diagram

Connect the pulse sensor to your ESP8266/ESP32 as follows:

- VCC to 3.3V or 5V (depending on your sensor)
- GND to GND
- Signal to Analog pin (A0)

## Installation

### Install Arduino IDE

Download and install the [Arduino IDE](https://www.arduino.cc/en/software).

### Install Board Support

**For ESP8266:**

1. Go to File > Preferences.
2. Add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to the Additional Board Manager URLs.
3. Go to Tools > Board > Boards Manager and install the `esp8266` package.

**For ESP32:**

1. Go to File > Preferences.
2. Add `https://dl.espressif.com/dl/package_esp32_index.json` to the Additional Board Manager URLs.
3. Go to Tools > Board > Boards Manager and install the `esp32` package.

### Install Libraries

Install the following libraries via the Arduino Library Manager (Sketch > Include Library > Library Manager):

- Firebase ESP Client
- WiFi (if not already installed)

## Usage

### Clone the Repository

git clone https://github.com/dilshanindunil/asthma-monitoring-device
cd your-repo-name
Open the Code in Arduino IDE
Open the final.ino file in Arduino IDE.

Configure Wi-Fi and Firebase
In the final.ino file, update the following lines with your credentials:


Copy code
#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"
#define API_KEY "Your_Firebase_API_Key"
#define DATABASE_URL "Your_Firebase_Database_URL"
Upload the Code
Select your board and port from Tools > Board and Tools > Port, then click the Upload button.

Monitor the Serial Output
Open the Serial Monitor from Tools > Serial Monitor to see the pulse sensor readings and Firebase upload status.

Code Explanation
The provided code performs the following steps:

Connects to the specified Wi-Fi network.
Initializes Firebase with the provided API key and database URL.
Reads the pulse sensor value from the analog pin.
Maps the pulse sensor value to a desired range (60-70).
Uploads the mapped pulse value to Firebase Realtime Database every 15 seconds.
Troubleshooting
Ensure your Wi-Fi credentials are correct.
Check your Firebase project configuration.
Make sure your pulse sensor is connected correctly.
License
This project is licensed under the MIT License. See the LICENSE file for details.

Acknowledgements
Firebase ESP Client Library
Arduino
Contributing
Contributions are welcome! Please open an issue or submit a pull request for any improvements or additions.

Contact
For any questions or suggestions, feel free to reach out at [dilshanindunilrajapaksha1@gmail.com].
