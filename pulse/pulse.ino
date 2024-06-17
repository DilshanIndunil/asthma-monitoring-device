#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define PULSE_SENSOR_PIN A0  // Analog pin where pulse sensor is connected

// Insert your network credentials
#define WIFI_SSID "AndroidAP447D"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAM6iSJ8rOnKZ-7KHa6lSPYTooauJe6pus"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://iotproj-53a7b-default-rtdb.firebaseio.com/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

// Function to map the pulse sensor values to the desired range
int mapPulseValue(int rawValue) {
  // Map the rawValue from the pulse sensor (0-1023) to the range 60-100
  return map(rawValue, 0, 1023, 60, 70);
}

void setup(){
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
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
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    
    // Read pulse sensor value
    int pulseValue = analogRead(PULSE_SENSOR_PIN);
    // Map pulse value
    int mappedValue = mapPulseValue(pulseValue);

    // Write the mapped pulse value to Firebase
    if (Firebase.RTDB.setInt(&fbdo, "pulse_sensor_data", mappedValue)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    //Delay for 15 seconds before sending the next reading
    delay(15000);
  }
}
