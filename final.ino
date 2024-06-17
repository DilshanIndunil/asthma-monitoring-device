#include <Wire.h>
#include "DHT.h"

// Include variables from the first sketch
#define CALIBRATION_SAMPLES 50
#define READ_SAMPLES 10
#define ADC_MAX 1023
#define VOLTAGE_MAX 5.0
#define RL 10.0
#define RO 76.63
#define CO2_SLOPE -0.45
#define CO2_INTERCEPT 1.49
#define ATMOSPHERIC_CO2 300.0
#define ATMOSPHERIC_CO2_VOLTAGE 0.324

#define ANALOG_PIN A0
#define SMOKE_THRESHOLD 700 // Adjust this threshold value based on your sensor's sensitivity
#define DHTPIN D1
#define DHTTYPE DHT11

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "AndroidAP447D"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAM6iSJ8rOnKZ-7KHa6lSPYTooauJe6pus"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://iotproj-53a7b-default-rtdb.firebaseio.com/"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

DHT dht(DHTPIN, DHTTYPE);

double MQ135_calculateR0() {
  double r0 = 0;
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    r0 += analogRead(ANALOG_PIN);
    delay(100);
  }
  r0 /= CALIBRATION_SAMPLES;
  r0 = ADC_MAX * VOLTAGE_MAX / r0 - RL;
  return r0;
}

double MQ135_readCO2() {
  double vout = 0;
  for (int i = 0; i < READ_SAMPLES; i++) {
    vout += analogRead(ANALOG_PIN);
    delay(100);
  }
  vout /= READ_SAMPLES;
  vout = vout * VOLTAGE_MAX / ADC_MAX;
  double rs = (VOLTAGE_MAX - vout) * RL / vout;
  double co2 = CO2_SLOPE * log(rs / RO) + CO2_INTERCEPT;
  co2 = (co2 - ATMOSPHERIC_CO2) * (ATMOSPHERIC_CO2_VOLTAGE / VOLTAGE_MAX);
  return co2;
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    // Upload CO2 data
    double co2 = MQ135_readCO2();
    if (Firebase.RTDB.setDouble(&fbdo, "sensors/co2", co2)) {
      Serial.println("CO2 data uploaded successfully");
    } else {
      Serial.println("Failed to upload CO2 data");
    }

    // Upload smoke level data
    int smokeLevel = analogRead(ANALOG_PIN);
    if (Firebase.RTDB.setInt(&fbdo, "sensors/smoke", smokeLevel)) {
      Serial.println("Smoke level data uploaded successfully");
    } else {
      Serial.println("Failed to upload smoke level data");
    }

    // Detect smoke
    if (smokeLevel > SMOKE_THRESHOLD) {
      if (Firebase.RTDB.setString(&fbdo, "sensors/smoke_detected", "Yes")) {
        Serial.println("Smoke detected!");
      } else {
        Serial.println("Failed to upload smoke detection status");
      }
    } else {
      if (Firebase.RTDB.setString(&fbdo, "sensors/smoke_detected", "No")) {
        Serial.println("No smoke detected");
      } else {
        Serial.println("Failed to upload smoke detection status");
      }
    }

    // Upload temperature and humidity data
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
      if (Firebase.RTDB.setFloat(&fbdo, "sensors/temperature", t)) {
        Serial.println("Temperature data uploaded successfully");
      } else {
        Serial.println("Failed to upload temperature data");
      }

      if (Firebase.RTDB.setFloat(&fbdo, "sensors/humidity", h)) {
        Serial.println("Humidity data uploaded successfully");
      } else {
        Serial.println("Failed to upload humidity data");
      }
    } else {
      Serial.println("Failed to read DHT sensor data");
    }
  }
  
  delay(1000);
}
